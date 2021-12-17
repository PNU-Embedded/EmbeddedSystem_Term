#include "buzzer.h"

/*
  x��ǥ�� dma�� �޾ƿ���, z��ǥ�� ��û �� ���� ��ǥ�� �о��.
  dma2�� adc3�� ������ �����ѵ� ���忡�� adc3�� �������� �ʱ⶧����
  dma2 - adc2 ������ ���� ��ü���� �Ұ����ϱ� �����Ͽ� dma1-adc1�� ���� ��.
  �ִ��� ���� �� Ȱ���ϰ��� dma���.
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