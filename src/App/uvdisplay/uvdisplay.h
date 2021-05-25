#ifndef _uvdisplay_H_
#define _uvdisplay_H_

#include "stm32f4xx_hal.h"
#include "main.h"
#include "ssd_utils.h"
#include "cpld_utils.h"






uint8_t		UVD_Init();
void		UVD_Sleep();
void		UVD_Wakeup();
void		UVD_Off();
void		UVD_On();




#endif // _uvdisplay_H