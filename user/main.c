#include <ctype.h>
#include <stdio.h>
#include "init.h"
#include "common.h"
#include "command_handler.h"
#include "password_handler.h"

// function prototype
void USART2_IRQHandler(void);
void Command_Run(void);
//////////////////////////////////////////////////////////////////////////////////////

Program_Status program_current_status = STATE_INIT;
Command_Handler command_handler;
Password_Handler password_handler;

void USART2_IRQHandler(void) {
  uint16_t word;
  if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET) {
    USART_ClearITPendingBit(USART2,USART_IT_RXNE); 
    word = USART_ReceiveData(USART2);
    
    // 금고 열린 상태면 명령 무시
    if (program_current_status == STATE_OPEN) {
      printf("OPEN STATUS!!\n");
      return;
    }
    
    if (word == 255) {
      Command_Reset(&command_handler);
    }
    else if (word == 0){
      Command_Decide(&command_handler);
      Command_Run();
    }
    else {
      Command_Append(&command_handler, word);
    }
  }
}

void Command_Run(void) {
  switch(command_handler.command) {
    case CMD_NUMBER: {
      Password_Type(&password_handler, command_handler.command_buffer[0]);
      Bluetooth_SendString(password_handler.current_buffer);
      break;
    }
    
    case CMD_DELETE: {
      Password_Delete(&password_handler);
      Bluetooth_SendString(password_handler.current_buffer);
      break;
    }
  
    case CMD_ENTER: {
      switch(program_current_status) {
        case STATE_INIT: {
          bool update_result = Password_Update(&password_handler);
          if (update_result == false)
            return;

          Bluetooth_SendString(password_handler.current_buffer);
          program_current_status = STATE_CLOSE;
          printf("Initiated!\n");
          break;
        }
        case STATE_CLOSE: {
          bool enter_result = Password_Enter(&password_handler);
          if (enter_result == false)
            return;
          
          Bluetooth_SendString(password_handler.current_buffer);
          program_current_status = STATE_OPEN;
          printf("opened!\n");
          break;
        }
      }
      break;
    }
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
  //LCD_Configure();
  USART2_Init();
  
  while (1) {
  }
}
