#include "magnetic.h"

void MAGNETIC_Configure(void){
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

Magnetic_Status MAGNETIC_Get_Status(void) {
  return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8) == 0 ? MAGNETIC_CLOSED : MAGNETIC_OPENED;
}