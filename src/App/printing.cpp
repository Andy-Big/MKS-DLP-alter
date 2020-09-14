#include "printing.h"
#include "sys_timer.h"
#include "cpld_utils.h"
#include "uvdisplay.h"



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
uint32_t				resX = 0, resY = 0;




extern uint8_t			Line_Pixel[CPLD_Y_RATIO + CPLD_FILLCODE * 2];

extern DLP_BMP			cpld_bmp;



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
//	UVLED_On();
	_cpld_bank2disp_enable(WORK_USED_BANK, 0, 0);

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
			resX = PFILE_GetResolutionX();
			resY = PFILE_GetResolutionY();

			if (f_lseek(&ufile, ldata_offset_begin) != FR_OK)
				return 0;
			uint32_t			toread = LAYERBUFF_SIZE;
			uint32_t			rd = 0;
			uint32_t			line = 0;


			// original method
/*
{
			uint8_t		color;
			uint16_t	length, length_sum;
			uint16_t	curpoint;
			uint8_t		*p;
			uint8_t		remaining;
			uint8_t		rc;
			BYTE		sd_char;

			cpld_bmp.current_line = 0;
			p = Line_Pixel;
			curpoint = 0;
			for(uint32_t i = 0; i < ldata_length, cpld_bmp.current_line < resY; i++)
			{
				f_read(&ufile, &sd_char, 1, &rd);
				color = (sd_char & 0x80) >> 7;
				length = sd_char & 0x7f;
				
				for(uint32_t j = 0; j < length; j++)
				{
					if(curpoint < CPLD_Y_RATIO / 2)
					{
						*p = color;
					}
					else
					{
						*(p + CPLD_FILLCODE) = color;
					}
					p++;
					curpoint++;
					
					if(curpoint > resX)
					{
						_cpld_line_gen_data(cpld_bmp.current_line, WORK_USED_BANK);
//						if((work_bank == BANK_USED_ID0) || work_bank == WORK_USED_BANK)
//							_cpld_line_gen_data_TFT(mksdlp.bmp.current_line);
						cpld_bmp.current_line++;
						curpoint = 0;
						memset(Line_Pixel, 0, sizeof(Line_Pixel));
						p = &Line_Pixel[0];
						remaining = length - j;//结尾多余颜色，放到下一行。
						for(uint32_t k = 0; k < remaining; k++)
						{
							*p = color;
							p++;
							curpoint++;
						}
						break;
					}
				}
			}
}
/*/
			// modified original method

{
			uint8_t		color;
			uint16_t	length, length_sum;
			uint16_t	curpoint;
			uint8_t		*p;
			uint8_t		remaining;
			uint8_t		rc;
			BYTE		sd_char;
			uint32_t	i = 0, j = 0, k = 0;

			cpld_bmp.current_line = 0;
			p = Line_Pixel;
			curpoint = 0;
			while(i++ < ldata_length && cpld_bmp.current_line < resY)
			{
				while (curpoint < CPLD_Y_RATIO / 2)
				{
					f_read(&ufile, &sd_char, 1, &rd);
					color = (sd_char & 0x80) >> 7;
					length = sd_char & 0x7f;
					
					if (curpoint + length > CPLD_Y_RATIO / 2)
					{
						uint32_t length1 = CPLD_Y_RATIO / 2 - curpoint;
						remaining = length - length1;
						length = length1;
					}
					memset(p, color, length);
					curpoint += length;
					p += length;
				}
				memset(p, 0, CPLD_FILLCODE);
				p += CPLD_FILLCODE;
				memset(p, color, remaining);
				p += remaining;
				curpoint = remaining;
				remaining = 0;
				while (curpoint < CPLD_Y_RATIO / 2)
				{
					f_read(&ufile, &sd_char, 1, &rd);
					color = (sd_char & 0x80) >> 7;
					length = sd_char & 0x7f;
					
					if (curpoint + length > CPLD_Y_RATIO / 2)
					{
						uint32_t length1 = CPLD_Y_RATIO / 2 - curpoint;
						remaining = length - length1;
						length = length1;
					}
					memset(p, color, length);
					curpoint += length;
					p += length;
				}
				memset(p, 0, CPLD_FILLCODE);
				_cpld_line_gen_data(cpld_bmp.current_line, WORK_USED_BANK);
				cpld_bmp.current_line++;
				p = &Line_Pixel[0];
				memset(p, color, remaining);
				p += remaining;
				curpoint = remaining;
				remaining = 0;
			}
}
/**/
/*
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

			ldata_offset_current = 0;
			while (ldata_offset_current < ldata_length)
			{
				if (f_read(&ufile, &prtLBuff, 1, &rd) != FR_OK || rd != 1)
					return 0;
				uint8_t		*buff = prtLBuff;
				uint8_t		val = 0, count = 0;
				for(uint32_t buff_pos = 0; buff_pos < 1; buff_pos++)
				{
					val = *buff >> 7;
					count = *buff & 0x7F;
				}
				
				ldata_offset_current++;
			}
*/
			_cpld_bank2disp_enable(WORK_USED_BANK, 1, 1);
		}
		break;
	}


	return 1;
}
//==============================================================================



