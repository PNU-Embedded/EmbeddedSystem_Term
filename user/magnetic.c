#include "magnetic.h"

#define MAGNETIC_GPIO GPIOC
#define MAGNETIC_GPIO_PIN GPIO_Pin_8

void Magnetic_Configure(void){
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = MAGNETIC_GPIO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(MAGNETIC_GPIO, &GPIO_InitStructure);
}

Magnetic_Status MAGNETIC_Get_Status(void) {
  return GPIO_ReadInputDataBit(MAGNETIC_GPIO, MAGNETIC_GPIO_PIN) == 0 ? MAGNETIC_CLOSED : MAGNETIC_OPENED;
}