#include "buzzer.h"

/*
  x좌표만 dma로 받아오고, z좌표는 요청 때 마다 좌표값 읽어옴.
  dma2는 adc3와 연결이 가능한데 보드에서 adc3를 지원하지 않기때문에
  dma2 - adc2 연결이 보드 자체에서 불가능하기 때문하여 dma1-adc1만 연결 함.
  최대한 배운거 다 활용하고자 dma사용.
*/

#define BUZZER_GPIO GPIOC
#define BUZZER_GPIO_PIN GPIO_Pin_9

static void Buzzer_RCCInit(void);
static void Buzzer_GPIOInit(void);
static void Buzzer_TIMInit(void);
static void Buzzer_NVIICInit(void);

static void Buzzer_RCCInit() {
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
}
static void Buzzer_GPIOInit() {
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin = BUZZER_GPIO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(BUZZER_GPIO, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

static void Buzzer_TIMInit(void) {
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  uint16_t prescale = SystemCoreClock / 10000;
  TIM_TimeBaseStructure.TIM_Period = 10000;
  TIM_TimeBaseStructure.TIM_Prescaler = prescale; 
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  
  TIM_ARRPreloadConfig(TIM3, ENABLE);
  TIM_Cmd(TIM3, ENABLE);
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
}

static void Buzzer_NVIICInit(void) {
  NVIC_InitTypeDef NVIC_InitStructure;
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void Buzzer_Configure() {
  Buzzer_RCCInit();
  Buzzer_GPIOInit();
  Buzzer_TIMInit();
  Buzzer_NVIICInit();
}

void Buzzer_On() {
  GPIO_SetBits(BUZZER_GPIO, BUZZER_GPIO_PIN);
}

void Buzzer_Off() {
  GPIO_ResetBits(BUZZER_GPIO, BUZZER_GPIO_PIN);
}