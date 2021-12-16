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

static void Buzzer_RCCInit() {
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
}
static void Buzzer_GPIOInit() {
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin = BUZZER_GPIO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(BUZZER_GPIO, &GPIO_InitStructure);
}

void Buzzer_Configure() {
  Buzzer_RCCInit();
  Buzzer_GPIOInit();
}

void Buzzer_On() {
  GPIO_SetBits(BUZZER_GPIO, BUZZER_GPIO_PIN);
}

void Buzzer_Off() {
  GPIO_ResetBits(BUZZER_GPIO, BUZZER_GPIO_PIN);
}