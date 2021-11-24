#ifndef __INIT_H
#define __INIT_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdbool.h>
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
#include "stdbool.h"

/* Exported types ------------------------------------------------------------*/
typedef enum {
 STATE_INIT, STATE_CLOSE, STATE_OPEN
} Program_Status;
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void RCC_Configure(void);
void GPIO_Configure(void);
void NVIC_Configure(void);
void LCD_Configure(void);
void USART2_Init(void);
bool isNum(char* str);
void Bluetooth_SendString(char*);
void sleep(int);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F10x_IT_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
