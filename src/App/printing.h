#ifndef __printing_H
#define __printing_H

#include "includes.h"
#include "main.h"
#include "tgui.h"
#include "tgui_printscreenfuncs.h"
#include "prtfiles.h"
#include "files_pws.h"
#include "config.h"
#include "usb_host.h"
#include "ff.h"
#include "rtc.h"
#include "datetime.h"





typedef struct
{
	uint32_t		time_begin;
	uint32_t		current_layer;
	uint32_t		current_sublayer;
	float			light_time_total;
} PRINT_STATE;




uint8_t		PRINT_Init();


#endif /*__printing_H */

