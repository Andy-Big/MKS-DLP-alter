#include "printing.h"
#include "sys_timer.h"
#include "cpld_utils.h"



extern TG_SCREEN		*tguiActiveScreen;
extern char				msg[512];
extern __no_init FIL	ufile @ "CCMRAM";

extern char				fv_filename[256];
extern char				fv_dirname[256];
extern FILES_TYPE		fv_filetype;
extern TCHAR			fv_tfilename[512];

PRINT_STATE				prtState;

uint8_t 				prtLBuff[LAYERBUFF_SIZE];
uint32_t				ldata_offset_begin = 0;
uint32_t				ldata_offset_current = 0;
uint32_t				ldata_length = 0;



uint8_t		PRINT_Init()
{
	systemInfo.print_current_layer = 0;
	systemInfo.print_current_sublayer = 0;
	systemInfo.print_time_begin = DTIME_GetCurrentUnixtime();
	systemInfo.print_light_time_total = 0;
	systemInfo.print_is_paused = 0;
	systemInfo.print_is_printing = 0;
	systemInfo.print_is_canceled = 0;
	systemInfo.print_pause_time = 0;


	if (f_open(&ufile, fv_tfilename, FA_OPEN_EXISTING | FA_READ) != FR_OK)
	{
		return 0;
	}

	return 1;
}
//==============================================================================



uint8_t		PRINT_Complete()
{
	if (f_close(&ufile) != FR_OK)
	{
		return 0;
	}

	return 1;
}
//==============================================================================



uint8_t		PRINT_ReadLayerBegin()
{
	switch (fv_filetype)
	{
		case FTYPE_PWS:
		{
			FPWS_LAYERSINFO		l_info;
			memset(&l_info, 0, sizeof(FPWS_LAYERSINFO));
			if (FPWS_GetLayerInfo(systemInfo.print_current_layer, &l_info) == 0)
				return 0;
			ldata_offset_begin = l_info.data_point;
			ldata_length = l_info.data_length;
			ldata_offset_current = 0;

			if (f_lseek(&ufile, ldata_offset_begin) != FR_OK)
				return 0;
			uint32_t			toread = LAYERBUFF_SIZE;
			uint32_t			rd = 0;
			uint32_t			line = 0;
			CPLD_DATA_FRAME		clpd_frame;
			clpd_frame.mark1 = 0x40 | (WORK_USED_BANK << 4) | ((line >> 8) & 0x0F);//tan ---写sdram
			clpd_frame.mark2 =  line & 0xFF;
			while (ldata_offset_current < ldata_length)
			{
				toread = LAYERBUFF_SIZE;
				rd = 0;
				if (toread > (ldata_length - ldata_offset_current))
					toread = ldata_length - ldata_offset_current;
				if (f_read(&ufile, &prtLBuff, toread, &rd) != FR_OK || rd != toread)
					return 0;
				uint8_t		*buff = prtLBuff;
				uint8_t		val = 0, count = 0;
				for(uint32_t buff_pos = 0; buff_pos < toread; buff_pos++)
				{
					val = *buff >> 7;
					count = *buff & 0x7F;
				}
				
				ldata_offset_current += toread;
			}

		}
		break;
	}


	return 1;
}
//==============================================================================



