#ifndef __printing_H
#define __printing_H

#include "includes.h"
#include "main.h"
#include "tgui.h"
#include "tgui_printscreenfuncs.h"
#include "prtfiles.h"
#include "config.h"
#include "usb_host.h"
#include "ff.h"
#include "rtc.h"
#include "datetime.h"



#define LAYERBUFF_SIZE		4096


typedef struct
{
} PRINT_STATE;



uint8_t		PRINT_Init();
uint8_t		PRINT_Complete();

uint8_t		PRINT_ReadLayerInfo();
uint8_t		PRINT_ReadRLEDecode();
uint8_t		PRINT_ReadLayerBegin();
uint8_t		PRINT_ReadSublayerContinue();
void		PRINT_ClearLayerPreview();
void		PRINT_DrawLayerPreview();


#endif /*__printing_H */

