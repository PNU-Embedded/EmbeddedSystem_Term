#include <stdio.h>
#include "common.h"
#include "bluetooth.h"
#include "lcd16.h" 
#include "magnetic.h"
#include "accelerator.h"
#include "buzzer.h"
#include "servo.h"
#include "command_handler.h"
#include "password_handler.h"

/* function prototype */
void USART2_IRQHandler(void);
void Command_Run(void);
//////////////////////////////////////////////////////////////////////////////////////

/* variables - hANDLE */
Program_Status program_current_status = STATE_INIT;

Command_Handler command_handler;
Password_Handler password_handler;

//////////////////////////////////////////////////////////////////////////////////////

void USART2_IRQHandler(void) {
  uint16_t word;
  if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET) {
    USART_ClearITPendingBit(USART2,USART_IT_RXNE); 
    word = USART_ReceiveData(USART2);
    
    // 금고 열린 상태면 명령 무시
    if (program_current_status == STATE_OPEN) {
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
      LCD16_ShowPassword(password_handler.current_buffer);
      break;
    }
    
    case CMD_DELETE: {
      Password_Delete(&password_handler);
      Bluetooth_SendString(password_handler.current_buffer);
      LCD16_ShowPassword(password_handler.current_buffer);
      break;
    }
  
    case CMD_ENTER: {
      switch(program_current_status) {
        case STATE_INIT: {
          bool update_result = Password_Update(&password_handler);
          //  short password
          if (update_result == false) {
            LCD16_ShowMessage("Short Password!");
            return;
          }

          Bluetooth_SendString(password_handler.current_buffer);
          LCD16_ShowPassword(password_handler.current_buffer);
          program_current_status = STATE_CLOSE;
          LCD16_ShowMessage("Password Setted!");
          break;
        }
        case STATE_CLOSE: {
          bool enter_result = Password_Enter(&password_handler);
          if (enter_result == false) {
            LCD16_ShowMessage("Wrong Password!");
            return;
          }
          
          Bluetooth_SendString(password_handler.current_buffer);
          LCD16_ShowPassword(password_handler.current_buffer);
          program_current_status = STATE_OPEN;
          LCD16_ShowMessage("OPEN!");
          break;
        }
      }
      break;
    }
  }
}

int main(void) {
  SystemInit();
  Bluetooth_Configure();
  LCD16_Configure();
  Magnetic_Configure();
  Accelerator_Configure();
  Buzzer_Configure();
  Servo_Configure();
  
  while (1) {
    //printf("%d\n", MAGNETIC_Get_Status());
    //printf("%d : %d\n", Accelerator_Get_X(), Accelerator_Get_Z());
    /* Buzzer_Off();
    sleep(10000000);
    Buzzer_On(); */
    /* Servo_Open();
    sleep(10000000);
    Servo_Close();
    sleep(10000000); */
  }
}
