#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
   
#include "common.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void Bluetooth_Configure(void);
void Bluetooth_SendPassword(char*, bool);
void Bluetooth_SendError(char*);
void Bluetooth_SendData(char);

#ifdef __cplusplus
}
#endif

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
