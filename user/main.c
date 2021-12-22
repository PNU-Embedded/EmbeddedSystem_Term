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

#define COMMAND_START 255
#define COMMAND_END 0

#define COMMAND_CLOSE 253
#define COMMAND_ALERT_THEFT 251
#define COMMAND_ALERT_WRONG 247

#define COMMAND_REQUEST 249
#define COMMAND_RESET 248
/* function prototype */
void USART2_IRQHandler(void);
void Command_Run(void);
void chnageStateToInit(void);
void changeStateToClose(void); 
void changeStateToOpen(void);
void closeSafe(void);
void resetAll(void);

//////////////////////////////////////////////////////////////////////////////////////

Program_Status program_current_status = STATE_INIT;

Command_Handler command_handler;
Password_Handler password_handler;

bool buzzer_timer_en_status = false;
bool buzzer_status = false;

//////////////////////////////////////////////////////////////////////////////////////
void changeStateToClose() {
  Bluetooth_SendData(STATE_CLOSE); // CLOSE 상태로 전환 전송
  LCD16_ShowMessage("STATE: CLOSED"); // 상태 메시지 출력
  program_current_status = STATE_CLOSE; // CLOSE 상태로 전환
}

void changeStateToOpen() {
  Bluetooth_SendData(STATE_OPEN); // OPEN 상태로 전환 전송
  LCD16_ShowMessage("STATE: OPENED"); // 상태 메시지 출력
  program_current_status = STATE_OPEN; // OPEN 상태로 전환
}

void closeSafe() {
  bool magneticStatus = Magnetic_Get_Status();
  if (magneticStatus == MAGNETIC_OPENED) {
    Bluetooth_SendError("Before lock, please close the door");
  } 
  else {
    changeStateToClose();
    Servo_Close();
  }
}

void openSafe() {
  changeStateToOpen();
  Servo_Open();
}

void resetAll() {
  if (program_current_status == STATE_CLOSE) {
    Command_Reset(&command_handler);
    Password_Reset(&password_handler);
    program_current_status = STATE_INIT;
    buzzer_timer_en_status = false;
    buzzer_status = false;
    Buzzer_Off();
    Bluetooth_SendData(248); // 비밀번호 reset command
    Bluetooth_SendData(254); // INIT STATE command
    
    LCD16_ShowMessage("STATE: INIT");
    LCD16_ShowPassword(password_handler.current_buffer);
  } else {
    Bluetooth_SendError("Please close the door before reset");
  }
}

/* bluetooth 명령 제어 */
void USART2_IRQHandler(void) {
  uint16_t word;
  if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET) {
    USART_ClearITPendingBit(USART2,USART_IT_RXNE); 
    word = USART_ReceiveData(USART2);
    
    /* 리셋 명령어 */
    if (word == COMMAND_RESET) {
      resetAll();
      return;
    }
    
    /* 금고 시작 상태 */
    if (program_current_status == STATE_INIT ) {
      if (word == COMMAND_START) {
        Command_Reset(&command_handler);
      }
      else if (word == COMMAND_END){
        Command_Decide(&command_handler);
        Command_Run();
      }
      else if (isalnum(word)) {
        Command_Append(&command_handler, word);
      }
      else if (word == COMMAND_CLOSE) {
        Bluetooth_SendError("Should Type Initial Password First");
      }
      else if (word == COMMAND_REQUEST) {
        Bluetooth_SendData(COMMAND_REQUEST);
      }
    }
    /* 금고 닫힌 상태 */
    else if (program_current_status == STATE_CLOSE){
      if (word == COMMAND_START) {
        Command_Reset(&command_handler);
      }
      else if (word == COMMAND_END){
        Command_Decide(&command_handler);
        Command_Run();
      }
      else if (isalnum(word)) {
        Command_Append(&command_handler, word);
      }
      else if (word == COMMAND_CLOSE) {
        Bluetooth_SendError("Already Closed");
      }
      else if (word == COMMAND_REQUEST) {
        Bluetooth_SendData(program_current_status);
      }
    }
    /* 금고 열린 상태 */
    else if (program_current_status == STATE_OPEN){
       if (word == COMMAND_START) {
        Bluetooth_SendError("Could not type password at opened status");
      }
      else if (word == COMMAND_CLOSE) {
        closeSafe();
      }
      else if (word == COMMAND_REQUEST) {
        Bluetooth_SendData(program_current_status);
      }
    }
  }
}

void Command_Run(void) {
  switch(command_handler.command) {
    /* 패스워드 입력 */
    case CMD_NUMBER: {
      Password_Type(&password_handler, command_handler.command_buffer[0]);
      Bluetooth_SendPassword(password_handler.current_buffer, false);
      LCD16_ShowPassword(password_handler.current_buffer);
      break;
    }
    
    /* 삭제 명령어 */
    case CMD_DELETE: {
      Password_Delete(&password_handler);
      Bluetooth_SendPassword(password_handler.current_buffer, false);
      LCD16_ShowPassword(password_handler.current_buffer);
      break;
    }
  
    /* 엔터 명령어 */
    case CMD_ENTER: {
      /* 시작 상태일 때 */
      if (program_current_status == STATE_INIT) {
        bool update_result = Password_Update(&password_handler);
        /* 패스워드가 짧은 경우 */
        if (update_result == false) {
          Bluetooth_SendError("Wrong Password");
          return;
        }
        /* 패스워드 세팅이 정상적으로 된 경우 */
        Bluetooth_SendPassword(password_handler.current_buffer, false); // 빈 buffer 전송
        Bluetooth_SendData(STATE_CLOSE); // CLOSE 상태로 전환 전송
        
        LCD16_ShowPassword(password_handler.current_buffer); // 빈 buffer 출력
        LCD16_ShowMessage("STATE: CLOSED"); // 상태 메시지 출력  
        program_current_status = STATE_CLOSE; // CLOSE 상태로 전환
        
        /* 스마트폰 스토리지에 패스워드 저장 */
        Bluetooth_SendPassword(password_handler.password, true);
      }
      /* 닫힌 상태일 때 */
      else if (program_current_status == STATE_CLOSE) {
        bool enter_result = Password_Enter(&password_handler);
        /* 패스워드가 틀린 경우 */
        if (enter_result == false) {
          Password_Increase_Wrong_Cnt(&password_handler);
          char err_msg[20] = "Wrong Cnt: ";
          int err_len = strlen(err_msg);
          err_msg[err_len] = '0' + password_handler.wrong_cnt;
          err_msg[err_len + 1] = '\0';
          Bluetooth_SendError("Wrong Password!");
          LCD16_ShowMessage(err_msg);
          if (password_handler.wrong_cnt == 3) {
            Bluetooth_SendData(COMMAND_ALERT_WRONG);
            buzzer_timer_en_status = true;
          }
          return;
        }
        
        /* 패스워드가 맞은 경우 */
        Bluetooth_SendPassword(password_handler.current_buffer, false); // 빈 buffer 전송
        LCD16_ShowPassword(password_handler.current_buffer); // 빈 buffer 출력
        openSafe();
        buzzer_timer_en_status = false;
        Buzzer_Off();
        Password_Clear_Wrong_Cnt(&password_handler);
      }
      else if (program_current_status == STATE_OPEN) {
        /* 절대 발생할 수 없는 경우 */
        printf("open 상태에서 command_run 발생!\n");
      }
    }
  }
}

/* buzzer Timer */
void TIM4_IRQHandler(void) {
  if(TIM_GetITStatus(TIM4, TIM_IT_Update)!=RESET) {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    
    if (buzzer_timer_en_status) {
      if (buzzer_status) Buzzer_Off();
      else Buzzer_On();
      buzzer_status = !buzzer_status;
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
  
  LCD16_ShowMessage("STATE: INIT");
  
  while (1) {
    if (program_current_status == STATE_CLOSE) {
      int acceler = Accelerator_Get_X() + Accelerator_Get_Z();
      if (acceler >= 610 || acceler <= 560 ) {
        if (buzzer_timer_en_status == false) {
          Bluetooth_SendData(COMMAND_ALERT_THEFT);
        }
        buzzer_timer_en_status = true;
      }
    }
    else {
      buzzer_timer_en_status = false;
    }
  }
}
