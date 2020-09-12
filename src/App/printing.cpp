#include "printing.h"
#include "sys_timer.h"



extern TG_SCREEN		*tguiActiveScreen;
extern char				msg[512];
extern __no_init FIL	ufile @ "CCMRAM";

extern char				fv_filename[256];
extern char				fv_dirname[256];
extern FILES_TYPE		fv_filetype;
extern TCHAR			fv_tfilename[512];



PRINT_STATE				prtState;



uint8_t		PRINT_Init()
{
	prtState.current_layer = 0;
	prtState.current_sublayer = 0;
	prtState.time_begin = DTIME_GetCurrentUnixtime();
	prtState.light_time_total = 0;

	if (f_open(&ufile, fv_tfilename, FA_OPEN_EXISTING | FA_READ) != FR_OK)
	{
		return 0;
	}

	return 1;
}
//==============================================================================



