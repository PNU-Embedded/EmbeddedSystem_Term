#ifndef __COMMAND_HANDLER_H
#define __COMMAND_HANDLER_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "stm32f10x.h"
/* Exported types ------------------------------------------------------------*/
typedef enum {
 CMD_CLEAR, CMD_DELETE, CMD_ENTER, CMD_NUMBER
} Command_Types;

typedef struct{
 char command_buffer[101];
 uint8_t command_buffer_index;
 Command_Types command;
} Command_Handler;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void Command_Append(Command_Handler* command_handler, char ch);
void Command_Decide(Command_Handler* command_handler);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F10x_IT_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
