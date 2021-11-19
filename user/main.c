#include <ctype.h>
#include "init.h"
#include "command_handler.h"


// function prototype
void USART2_IRQHandler(void);
void Bluetooth_SendData(uint16_t word);
void Command_Run(Command_Handler* command_handler);
//////////////////////////////////////////////////////////////////////////////////////

Command_Handler command_handler;

void USART2_IRQHandler(void) {
  uint16_t word;
  if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET) {
    USART_ClearITPendingBit(USART2,USART_IT_RXNE); 
    word = USART_ReceiveData(USART2);
    if (word == '\n')
      return;
    if (word == '\r') {
      Command_Decide(&command_handler);
      Command_Run(&command_handler);
      return;
    }
    Command_Append(&command_handler, word);
  }
}

void Bluetooth_SendString(char* str) {
  while(*str) {
    USART_SendData(USART2, *str);
    ++str;
  }
}

void Command_Run(Command_Handler* command_handler) {
  switch(command_handler->command) {
  case CMD_NUMBER:
    Bluetooth_SendString(command_handler->command_buffer);
    break;
    
  case CMD_CLEAR:
    
    
    
    break;
    
  case CMD_DELETE:
    
    
    
    break;
    
  case CMD_ENTER:
    
    
    
    break;
    
  default:
    
    
    
    break;
  }
}

void Dubugger_ShowData(uint32_t data) {
  LCD_ShowNum(20, 40, data, 8, BLACK, WHITE);
}

int main(void) {
  SystemInit();
  RCC_Configure();
  GPIO_Configure();
  NVIC_Configure();
  LCD_Configure();
  USART2_Init();
        
  while (1) {
    
  }
}
