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
#define COMMAND_ALERT 251

#define COMMAND_REQUEST 249
#define COMMAND_START_PW 248
#define COMMAND_END_PW 2

/* function prototype */
void USART2_IRQHandler(void);
void Command_Run(void);
void changeStateToClose(void); 
void changeStateToOpen(void);
void closeSafe(void);
//////////////////////////////////////////////////////////////////////////////////////

Program_Status program_current_status = STATE_INIT;

Command_Handler command_handler;
Password_Handler password_handler;

bool buzzer_timer_en_status = false;
bool buzzer_status = false;
int buzzer_cnt = 0;
//////////////////////////////////////////////////////////////////////////////////////

void changeStateToClose() {
  Bluetooth_SendData(STATE_CLOSE); // CLOSE ���·� ��ȯ ����
  LCD16_ShowMessage("STATE: CLOSED"); // ���� �޽��� ���
  program_current_status = STATE_CLOSE; // CLOSE ���·� ��ȯ
}

void changeStateToOpen() {
  Bluetooth_SendData(STATE_OPEN); // OPEN ���·� ��ȯ ����
  LCD16_ShowMessage("STATE: OPENED"); // ���� �޽��� ���
  program_current_status = STATE_OPEN; // OPEN ���·� ��ȯ
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

/* bluetooth ��� ���� */
void USART2_IRQHandler(void) {
  uint16_t word;
  if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET) {
    USART_ClearITPendingBit(USART2,USART_IT_RXNE); 
    word = USART_ReceiveData(USART2);
    
    /* �ݰ� ���� ���� */
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
    /* �ݰ� ���� ���� */
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
      else if (word == COMMAND_ALERT) {
        buzzer_timer_en_status = false;
      } 
      else if (word == COMMAND_REQUEST) {
        Bluetooth_SendData(program_current_status);
      }
    }
    /* �ݰ� ���� ���� */
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
    /* �н����� �Է� */
    case CMD_NUMBER: {
      Password_Type(&password_handler, command_handler.command_buffer[0]);
      Bluetooth_SendPassword(password_handler.current_buffer, false);
      LCD16_ShowPassword(password_handler.current_buffer);
      break;
    }
    
    /* ���� ��ɾ� */
    case CMD_DELETE: {
      Password_Delete(&password_handler);
      Bluetooth_SendPassword(password_handler.current_buffer, false);
      LCD16_ShowPassword(password_handler.current_buffer);
      break;
    }
  
    /* ���� ��ɾ� */
    case CMD_ENTER: {
      /* ���� ������ �� */
      if (program_current_status == STATE_INIT) {
        bool update_result = Password_Update(&password_handler);
        /* �н����尡 ª�� ��� */
        if (update_result == false) {
          Bluetooth_SendError("Wrong Password");
          LCD16_ShowMessage("Short Password!");
          return;
        }
        /* �н����� ������ ���������� �� ��� */
        Bluetooth_SendPassword(password_handler.current_buffer, false); // �� buffer ����
        Bluetooth_SendData(STATE_CLOSE); // CLOSE ���·� ��ȯ ����
        
        LCD16_ShowPassword(password_handler.current_buffer); // �� buffer ���
        LCD16_ShowMessage("STATE: CLOSED"); // ���� �޽��� ���  
        program_current_status = STATE_CLOSE; // CLOSE ���·� ��ȯ
        
        /* ����Ʈ�� ���丮���� �н����� ���� */
        Bluetooth_SendPassword(password_handler.password, true);
      }
      /* ���� ������ �� */
      else if (program_current_status == STATE_CLOSE) {
        bool enter_result = Password_Enter(&password_handler);
        /* �н����尡 Ʋ�� ��� */
        if (enter_result == false) {
          Bluetooth_SendError("Wrong Password");
          LCD16_ShowMessage("Wrong Password!");
          return;
        }
        
        /* �н����尡 ���� ��� */
        Bluetooth_SendPassword(password_handler.current_buffer, false); // �� buffer ����
        LCD16_ShowPassword(password_handler.current_buffer); // �� buffer ���
        openSafe();
      }
      else if (program_current_status == STATE_OPEN) {
        /* ���� �߻��� �� ���� ��� */
        printf("open ���¿��� command_run �߻�!\n");
      }
    }
  }
}

/* buzzer Timer */
void TIM3_IRQHandler(void) {
  if(TIM_GetITStatus(TIM3, TIM_IT_Update)!=RESET) {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    
    if (buzzer_timer_en_status) {
      ++buzzer_cnt;
      buzzer_cnt %= 5;
      if (buzzer_cnt == 0) {
        if (buzzer_status) Buzzer_Off();
        else ;//Buzzer_On();
        buzzer_status = !buzzer_status;
      }
    }
    else {
      buzzer_cnt = 0;
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
  
  uint16_t prv_pos_x = Accelerator_Get_X();
  uint16_t prv_pos_z = Accelerator_Get_Z();
  
  while (1) {
    if (program_current_status == STATE_CLOSE) {
      int cur_pos_x = Accelerator_Get_X();
      int cur_pos_z = Accelerator_Get_Z();
      int diff = (cur_pos_x - prv_pos_x) * (cur_pos_x - prv_pos_x) + (cur_pos_z - prv_pos_z) * (cur_pos_z - prv_pos_z);
      if (diff >= 1000) {
        if (buzzer_timer_en_status == false) {
          Bluetooth_SendData(COMMAND_ALERT);
        }
        buzzer_timer_en_status = true;
      }
      prv_pos_x = cur_pos_x;
      prv_pos_z = cur_pos_z;
    }
  }
}
