#ifndef __COMMON_H
#define __COMMON_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "stm32f10x_usart.h"

/* Exported types ------------------------------------------------------------*/
   
typedef enum {
 STATE_INIT, STATE_CLOSE, STATE_OPEN
} Program_Status;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

bool isNum(char* str);
void Bluetooth_SendString(char*);
void sleep(int);

#ifdef __cplusplus
}
#endif

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
