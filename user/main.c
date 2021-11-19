#include <ctype.h>
#include "stm32f10x.h"
#include "core_cm3.h"
#include "misc.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_adc.h"
#include "lcd.h"
#include "touch.h"
#include "init.h"

uint8_t password[4] = {'0', '0', '0', '0'};
uint8_t current = 0;

void USART2_IRQHandler(void) {
  uint8_t word;
  if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET) {
    USART_ClearITPendingBit(USART2,USART_IT_RXNE); 
    word = USART_ReceiveData(USART2);
    if (word == '\n' || word == '\r')
      return;

    USART_SendData(USART2, isalpha(word) == 0 ? '0' : '1');
  }
}

void Debugger_Init(void) {
  LCD_Init();
  Touch_Configuration();
  //Touch_Adjust();
  LCD_Clear(WHITE);
}

void Dubugger_ShowData(uint32_t data) {
  LCD_ShowNum(20, 40, data, 8, BLACK, WHITE);
}

int main(void) {
  SystemInit();
  RCC_Configure();
  GPIO_Configure();
  NVIC_Configure();
  USART2_Init();
  
  Debugger_Init();
        
  while (1) {
    
  }
}
