#ifndef __MAGNETIC_H
#define __MAGNETIC_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
   
#include "common.h"

/* Exported types ------------------------------------------------------------*/
typedef enum {
 MAGNETIC_OPENED = 0, MAGNETIC_CLOSED = 1
} Magnetic_Status;
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void Magnetic_Configure(void);
Magnetic_Status Magnetic_Get_Status(void);

#ifdef __cplusplus
}
#endif

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
