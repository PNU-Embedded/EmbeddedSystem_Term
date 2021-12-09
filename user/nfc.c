#include "nfc.h"

static int command;
static uint8_t packetbuffer[64];

static void NFC_RCCInit() {
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

static void NFC_GPIOInit() {
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* USART 1*/
  /* TX */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* RX */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

static void NFC_UARTInit() {
  USART_InitTypeDef USART_InitStructure;

  USART_Cmd(USART2, ENABLE);

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  USART_Init(USART2, &USART_InitStructure);
  //USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

static void NFC_NVICInit() {
  NVIC_InitTypeDef NVIC_InitStructure;
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  // UART1
  NVIC_EnableIRQ(USART2_IRQn);
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void USART2_IRQHandler() {
  char word;
  if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET){
      word = USART_ReceiveData(USART2);
      printf("word: %d\n", word);
      
      USART_ClearITPendingBit(USART2,USART_IT_RXNE);
  }
}

void NFC_Configure(void) {
  NFC_RCCInit();
  NFC_GPIOInit();
  NFC_UARTInit();
  //NFC_NVICInit();
  
/* 
  const unsigned char wake[24]={
	0x55, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x03, 0xfd, 0xd4, \
	0x14, 0x01, 0x17, 0x00
	};
  for(int i=0;i<24;i++) {
    USART_SendData(USART2, wake[i]);
    sleep(10);
}*/
  
  
  /* wakeup */
  USART_SendData(USART2, 0x55);
  
  for (int i = 0; i < 100; ++i);
  
  /* SAM Config */
  
  packetbuffer[0] = 0x14;
  packetbuffer[1] = 0x01;
  
  writeCommand(packetbuffer, 2, 0, 0);
  
  //readResponse(packetbuffer, sizeof(packetbuffer));
  
  
  printf("end\n");
}

uint32_t NFC_getFirmwareVersion(void) {
  uint32_t response;

  packetbuffer[0] = 0x02;

  if (writeCommand(packetbuffer, 1, 0, 0)) {
      return 0;
  }

  // read data packet
  int8_t status = readResponse(packetbuffer, sizeof(packetbuffer));
  if (0 > status) {
      return 0;
  }

  response = packetbuffer[0];
  response <<= 8;
  response |= packetbuffer[1];
  response <<= 8;
  response |= packetbuffer[2];
  response <<= 8;
  response |= packetbuffer[3];

  return response;
}

static uint8_t writeCommand(const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen) {
  command = header[0];
  
  /* start */
  USART_SendData(USART2, 0x00);
  USART_SendData(USART2, 0x00);
  USART_SendData(USART2, 0xFF);
  
  /* total length */
  uint8_t length = hlen + blen + 1;
  USART_SendData(USART2, length);
  USART_SendData(USART2, ~length + 1); // checksum of length
  
  /* write */
  USART_SendData(USART2, 0xD4);
  
  uint8_t sum = 0xD4;
  for (int i = 0; i < hlen; ++i) {
    USART_SendData(USART2, header[i]);
    sum += header[i];
  }
  
  for (int i = 0; i < blen; ++i) {
    USART_SendData(USART2, body[i]);
    sum += body[i];
  }
  
  USART_SendData(USART2, ~sum + 1); // checksum of data
  
  /* end */
  USART_SendData(USART2, 0x00);
  return readACKFrame();
}

static int8_t readACKFrame() {
  const uint8_t ack[6] = {0, 0, 0xFF, 0, 0xFF, 0};
  uint8_t ackBuf[6];
  
  receive(ackBuf, 6);
  if (memcmp(ackBuf, ack, 6))
    return -1;
  return 0;
}

static int8_t receive(uint8_t *buf, int len) {
  int index = 0;
  int ret;
  while (index < len) {
    /* wait ack */
    while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
    ret = USART_ReceiveData(USART2);
    
    if (ret == 0)
      return index;
    
    buf[index] = ret;
    printf("%x ", ret);
    printf("\n");
    index++;
  }
  return index;
}

static int16_t readResponse(uint8_t* buf, uint8_t len) {
  /* receive preamble */
  uint8_t tmp[3];
  receive(tmp, 3);
  
  /* receive length */
  uint8_t length[2];
  receive(length, 2);
  length[0] -= 2;
  
  /* receive command */
  uint8_t cmd = command + 1;
  receive(tmp, 2);
  receive(buf, length[0]);
  uint8_t sum = 0xD5 + cmd;
  for (int i = 0; i < length[0]; ++i)
    sum += buf[i];
  
  /* checksm and postamble */
  receive(tmp, 2);
  
  return length[0];
}