#include "bluetooth.h"

#define BLUETOOTH_USART USART2
#define BLUETOOTH_GPIO GPIOA
#define BLUETOOTH_GPIO_TX_PIN GPIO_Pin_2;
#define BLUETOOTH_GPIO_RX_PIN GPIO_Pin_3;

static void Bluetooth_RCCInit();
static void Bluetooth_GPIOInit();
static void Bluetooth_NVICInit();
static void Bluetooth_USARTInit();

static void Bluetooth_RCCInit() {
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}
static void Bluetooth_GPIOInit() {
  GPIO_InitTypeDef GPIO_InitStructure;
  
  //TX
  GPIO_InitStructure.GPIO_Pin = BLUETOOTH_GPIO_TX_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(BLUETOOTH_GPIO, &GPIO_InitStructure);
  
  //RX
  GPIO_InitStructure.GPIO_Pin = BLUETOOTH_GPIO_RX_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(BLUETOOTH_GPIO, &GPIO_InitStructure);
}

static void Bluetooth_NVICInit() {
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  NVIC_EnableIRQ(USART2_IRQn);
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

static void Bluetooth_USARTInit() {
  USART_InitTypeDef USART_InitStructure;

  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(BLUETOOTH_USART, &USART_InitStructure);
  USART_Cmd(BLUETOOTH_USART, ENABLE);
  USART_ITConfig(BLUETOOTH_USART, USART_IT_RXNE, ENABLE);
}

void Bluetooth_Configure(void) {
  Bluetooth_RCCInit();
  Bluetooth_GPIOInit();
  Bluetooth_NVICInit();
  Bluetooth_USARTInit();
}

void Bluetooth_SendPassword(char* pw, bool set) {
  USART_SendData(USART2, (uint16_t)255);
  sleep(200000);
  while(*pw != '\0') {
    USART_SendData(USART2, *pw);
    sleep(200000);
    ++pw;
  }
  USART_SendData(USART2, set ? 2 : 0);
  sleep(200000);
}

void Bluetooth_SendError(char* str) {
  USART_SendData(USART2, (uint16_t)250);
  sleep(200000);
  while(*str != '\0') {
    USART_SendData(USART2, *str);
    sleep(200000);
    ++str;
  }
  USART_SendData(USART2, 1);
  sleep(200000);
}

void Bluetooth_SendData(char ch) {
  USART_SendData(USART2, ch);
  sleep(200000);
}