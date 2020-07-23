#ifndef __touch_spi_H
#define __touch_spi_H

#ifdef __cplusplus
 extern "C" {
#endif


#include "includes.h"
#include "main.h"

#define USE_HAL_SPI_REGISTER_CALLBACKS	1U



#define		TOUCH_READS			(uint8_t)16
#define		TOUCH_BUFF_SIZE		(uint8_t)6*TOUCH_READS
	 
typedef enum
{
	TS_FREE,
	TS_WORKED,
	TS_PREPRESSED,
	TS_SPRESSED,
	TS_LPRESSED,
	TS_SRELEASED,
	TS_LRELEASED,
} TOUCH_STATES;

typedef struct
{
	TOUCH_STATES	state;
	uint8_t			time;
	uint16_t		xc;
	uint16_t		yc;
	uint16_t		x_min;
	uint16_t		x_max;
	uint16_t		y_min;
	uint16_t		y_max;
	
} TOUCH_INFO;

typedef struct
{
	uint16_t		xc;
	uint16_t		yc;
} TOUCH_POINT;

void		_touch_ReadCoords();
void 		_touch_RefreshState();

void		Touch_Init(void);
void		Touch_Enable();
void		Touch_Disable();

TOUCH_STATES	Touch_GetState();
void		Touch_SetState(TOUCH_STATES newstate);
void		Touch_GetCoords(TOUCH_POINT *pt);


#ifdef __cplusplus
 }
#endif
#endif /*__touch_spi_H */

