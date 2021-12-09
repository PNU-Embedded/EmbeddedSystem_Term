#ifndef __NFC_H
#define __NFC_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"
#include <stdbool.h>
#include "common.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void NFC_Configure(void);
uint32_t NFC_getFirmwareVersion(void);

static uint8_t writeCommand(const uint8_t*, uint8_t, const uint8_t*, uint8_t);
static int8_t readACKFrame(void);
static int8_t receive(uint8_t*, int);
static int16_t readResponse(uint8_t*, uint8_t);

#ifdef __cplusplus
}
#endif

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
