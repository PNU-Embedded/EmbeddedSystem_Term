#include "servo.h"

#define SERVO_GPIO GPIOA
#define SERVO_GPIO_PIN GPIO_Pin_6
#define SERVO_TIM TIM3

#define SERVO_OPEN 1500
#define SERVO_CLOSE 2400

static void Servo_RCCInit(void);
static void Servo_GPIOInit(void);
static void Servo_TIMInit(void);

static void Servo_RCCInit() {
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
}

static void Servo_GPIOInit() {
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin = SERVO_GPIO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(SERVO_GPIO, &GPIO_InitStructure);
}

static void Servo_TIMInit() {
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

  TIM_TimeBaseStructure.TIM_Period = 20000;
  TIM_TimeBaseStructure.TIM_Prescaler =  SystemCoreClock / 1000000; 
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
  TIM_TimeBaseInit(SERVO_TIM, &TIM_TimeBaseStructure);
  
  TIM_OCInitTypeDef TIM_OCInitStructure;
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = SERVO_CLOSE; // Pulse ????
  TIM_OC1Init(SERVO_TIM, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(SERVO_TIM, TIM_OCPreload_Disable);
  
  TIM_ARRPreloadConfig(SERVO_TIM, ENABLE);
  TIM_Cmd(SERVO_TIM, ENABLE);
}

void Servo_Configure(void) {
  Servo_RCCInit();
  Servo_GPIOInit();
  Servo_TIMInit();
}

void Servo_Open(void) {
  TIM_SetCompare1(SERVO_TIM, SERVO_OPEN);
}

void Servo_Close(void) {
  TIM_SetCompare1(SERVO_TIM, SERVO_CLOSE);
}
