#ifndef __spi_H
#define __spi_H


#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "main.h"

#define USE_HAL_SPI_REGISTER_CALLBACKS	1U



#define		TOUCH_READS			(uint8_t)16
#define		TOUCH_BUFF_SIZE		(uint8_t)6*TOUCH_READS

void		Touch_Init(void);
void		Touch_Enable();
void		Touch_Disable();

void		Touch_ReadCoords();
uint16_t	Touch_GetX();
uint16_t	Touch_GetY();

#ifdef __cplusplus
}
#endif
#endif /*__ spi_H */

