#ifndef __PASSWORD_HANDLER_H
#define __PASSWORD_HANDLER_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "stm32f10x.h"
#include "init.h"
/* Exported types ------------------------------------------------------------*/

typedef struct {
 char password[5];
 char current_buffer[5];
 uint8_t current_buffer_index;
 uint8_t wrong_cnt;
} Password_Handler;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

bool Password_Update(Password_Handler*);
void Password_Type(Password_Handler*, char);
bool Password_Enter(Password_Handler*);
void Password_Delete(Password_Handler*);
void Password_Reset(Password_Handler*);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F10x_IT_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
