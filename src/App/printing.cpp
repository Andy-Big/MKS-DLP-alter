#include "printing.h"
#include "gpio.h"
#include "sys_timer.h"
#include "cpld_utils.h"
#include "uvdisplay.h"
#include "tgui.h"
#include "tgui_printscreenfuncs.h"
#include "motor.h"
#include "z_endstops.h"
#include "files_pws.h"
#include "files_photon.h"



extern TG_SCREEN		*tguiActiveScreen;
extern char				msg[512];
extern __no_init FIL	ufile PLACE_TO_CCMRAM;

extern char				fv_filename[256];
extern char				fv_dirname[256];
extern FILES_TYPE		fv_filetype;
extern TCHAR			fv_tfilename[512];

extern uint8_t			tguiScreenTimer;
extern uint8_t			timerWorkTimeTimer;

extern uint16_t			CPLD_X_RATIO;
extern uint16_t			CPLD_Y_RATIO;


PRINT_STATE				prtState;

LAYER_INFO				l_info;

uint8_t 				prtLBuff[LAYERBUFF_SIZE];
uint32_t				ldata_offset_begin = 0;
uint32_t				ldata_offset_current = 0;
uint32_t				ldata_length = 0;
uint32_t				resX = 0, resY = 0;

uint16_t			PRT_PREV_SCALE_X = 8;
uint16_t			PRT_PREV_SCALE_Y = 8;

TG_RECT				prev_position;
uint16_t			prevcolors[2] = {LCDUI_RGB(0x000000), LCDUI_RGB(0xEFEFEF)};
uint32_t			prevheight;
uint32_t			prevwidth;
uint32_t			prevheightbytes;
uint8_t				previmage[480*320/8] PLACE_TO_CCMRAM;


extern uint8_t			Line_Pixel[3000];
#ifdef __MKSDLP_BOARD__
extern DLP_BMP			cpld_bmp;
#endif  // __MKSDLP_BOARD__



uint8_t		PRINT_Init()
{
#ifdef __MKSDLP_BOARD__
	PRT_PREV_SCALE_X = 8;
	PRT_PREV_SCALE_Y = 8;
#endif
#ifdef __CHITU_BOARD__
	PRT_PREV_SCALE_X = 8;
	PRT_PREV_SCALE_Y = 9;
#endif
	
	prevheight = CPLD_Y_RATIO / PRT_PREV_SCALE_Y;
	prevwidth = CPLD_X_RATIO / PRT_PREV_SCALE_X;
	prevheightbytes = prevheight / 8 + 1;

	systemInfo.print_time_decrement = 0;
	systemInfo.print_current_layer = 0;
	systemInfo.print_current_sublayer = 0;
	systemInfo.print_time_begin = DTIME_GetCurrentUnixtime();
	systemInfo.print_light_time_total = 0;
	systemInfo.print_is_paused = 0;
	systemInfo.print_is_printing = 0;
	systemInfo.print_is_canceled = 0;
	systemInfo.print_pause_time = 0;
	systemInfo.print_lift_at_end = cfgConfig.end_lift;
	systemInfo.print_use_aa_grade = (PFILE_GetAntialiasing() > 1);
	systemInfo.print_use_ind_params = PFILE_GetIndLayerSettings();
	systemInfo.print_timer_secs = DTIME_GetCurrentUnixtime();

	memset(&l_info, 0, sizeof(LAYER_INFO));

	for (uint8_t i = 0; i < tguiScreenPrint.btns_count; i++)
	{
		if (tguiScreenPrint.buttons[i].button_id == TG_SCR_PRINT_PREVIEW_ID)
		{
			prev_position.left = tguiScreenPrint.buttons[i].position.left;
			prev_position.top = tguiScreenPrint.buttons[i].position.top;
			prev_position.right = tguiScreenPrint.buttons[i].position.right;
			prev_position.bottom = tguiScreenPrint.buttons[i].position.bottom;
			break;
		}
	}
	
	memset(Line_Pixel, 0, sizeof(Line_Pixel));
	memset(previmage, 0, sizeof(previmage));

	if (f_open(&ufile, fv_tfilename, FA_OPEN_EXISTING | FA_READ) != FR_OK)
	{
		return 0;
	}
	
	return 1;
}
//==============================================================================



uint8_t		PRINT_Complete()
{
	systemInfo.printer_state = PST_PRINT_LASTLAYERLIFT;
	if (systemInfo.position_known)
	{

		// prelifting
		systemInfo.target_position += PFILE_GetLiftHeight();
		if (systemInfo.target_position > cfgMotor.max_pos)
			systemInfo.target_position = cfgMotor.max_pos;
		ZMOTOR_MoveAbsolute(systemInfo.target_position, PFILE_GetLiftSpeed());

		// main lifting
		systemInfo.print_is_printing = 0;

		if (systemInfo.target_position < 30)
		{
			systemInfo.target_position = 30;
			ZMOTOR_MoveAbsolute(systemInfo.target_position, cfgMotor.travel_feedrate / 3);
		}
		systemInfo.target_position += systemInfo.print_lift_at_end;
		if (systemInfo.target_position > cfgMotor.max_pos)
			systemInfo.target_position = cfgMotor.max_pos;
		ZMOTOR_MoveAbsolute(systemInfo.target_position, cfgMotor.travel_feedrate);

	}

	UVLED_Off();
	UVD_Sleep();
#ifdef __MKSDLP_BOARD__
	_cpld_bank2disp_enable(CLEAN_USED_BANK,0,0);
#endif  // __MKSDLP_BOARD__
	UVFAN_TimerOn(10000);
	if (cfgConfig.mb_fan_mode == MBFAN_PRINTING_ON)
		MBFAN_Off();
	
	// save timers
	uint32_t	unix_time = DTIME_GetCurrentUnixtime();
	cfgTimers.led_time += (uint32_t)systemInfo.print_light_time_total;
	cfgTimers.disp_time += (uint32_t)systemInfo.print_light_time_total;
	cfgTimers.fan_time += unix_time - systemInfo.print_time_begin;
	cfgTimers.total_print_time += unix_time - systemInfo.print_time_begin;
	cfgTimers.total_on_time += unix_time - systemInfo.timerWorkTimeSecs;
	
	systemInfo.timerWorkTimeSecs = unix_time;
	SYSTIMER_SetCountDown(timerWorkTimeTimer, TIMERS_SAVE_PERIOD * 1000);
	CFG_SaveTimers();

	if (tguiActiveScreen == &tguiScreenSaver)
	{
		tguiActiveScreen = (TG_SCREEN*)tguiScreenSaver.prevscreen;
		TGUI_ForceRepaint();
		if (cfgConfig.screensaver_time != TIMER_DISABLE && cfgConfig.screensaver_time > 0)
		{
			if (cfgConfig.screensaver_time < 120000)
				SYSTIMER_SetCountDown(tguiScreenTimer, 120000);
			else
				SYSTIMER_SetCountDown(tguiScreenTimer, cfgConfig.screensaver_time);
		}
	}
	systemInfo.print_is_printing = 0;
	systemInfo.print_is_paused = 0;
	systemInfo.print_is_homing = 0;

	if (f_close(&ufile) != FR_OK)
	{
		return 0;
	}

	return 1;
}
//==============================================================================



uint8_t		PRINT_ReadLayerInfo()
{
	switch (fv_filetype)
	{
		case FTYPE_PWS:
		{
			memset(&l_info, 0, sizeof(LAYER_INFO));
			if (FPWS_GetLayerInfo(systemInfo.print_current_layer, &l_info) == 0)
				return 0;
		}
		break;

		case FTYPE_PHOTON:
		{
			memset(&l_info, 0, sizeof(LAYER_INFO));
			if (FPHOTON_GetLayerInfo(systemInfo.print_current_layer, &l_info) == 0)
				return 0;
		}
		break;
	}
	
	return 1;
}
//==============================================================================



uint8_t		PRINT_ReadRLEDecode(uint8_t preview)
{
#ifdef __MKSDLP_BOARD__
	_cpld_bank2disp_enable(WORK_USED_BANK, 0, 0);

	uint8_t		color;
	uint16_t	length;
	uint16_t	curpoint;
	uint8_t		*p;
	uint8_t		remaining;
	BYTE		sd_char;
	uint32_t	i = 0;
	uint32_t	prevcolumn = 0;
	uint8_t		pbyte = 0;
	uint8_t		poffset = 0;
	uint8_t		*pimage = previmage;
	UINT	rd = 0;

	cpld_bmp.current_line = 0;
	p = Line_Pixel;
	curpoint = 0;
	while(i++ < ldata_length && cpld_bmp.current_line < resY)
	{
		while (curpoint < CPLD_Y_RATIO / 2)
		{
			if (f_read(&ufile, &sd_char, 1, &rd) != FR_OK || rd != 1)
				return 0;
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
			if (f_read(&ufile, &sd_char, 1, &rd) != FR_OK || rd != 1)
				return 0;
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
		
		// preview image
		if (preview)
		{
			pimage = previmage + prevcolumn * prevheightbytes;
			pbyte = 0;
			poffset = 0;
			if ((cpld_bmp.current_line % PRT_PREV_SCALE_Y) == 0)
			{
				p = &Line_Pixel[0];
				curpoint = 0;
				while (curpoint < prevheight / 2)
				{
					pbyte |= *p & 0x01;
					if (poffset == 8)
					{
						*pimage = pbyte;
						pbyte = 0;
						poffset = 0;
						pimage++;
					}
					pbyte <<= 1;
					poffset++;
					curpoint++;
					p += PRT_PREV_SCALE_X;
				}
				p += CPLD_FILLCODE;
				while (curpoint < prevheight)
				{
					pbyte |= *p & 0x01;
					if (poffset == 8)
					{
						*pimage = pbyte;
						pbyte = 0;
						poffset = 0;
						pimage++;
					}
					pbyte <<= 1;
					poffset++;
					curpoint++;
					p += PRT_PREV_SCALE_X;
				}
				if (poffset)
					*pimage = pbyte;
				prevcolumn++;
			}
		}
		cpld_bmp.current_line++;
		p = &Line_Pixel[0];
		memset(p, color, remaining);
		p += remaining;
		curpoint = remaining;
		remaining = 0;
	}

	_cpld_bank2disp_enable(WORK_USED_BANK, 1, 1);
#endif  // __MKSDLP_BOARD__

	return 1;
}
//==============================================================================



uint8_t		PRINT_ReadLayerBegin()
{
	uint8_t		res = 0;
	ldata_offset_begin = l_info.data_offset;
	ldata_length = l_info.data_length;
	ldata_offset_current = 0;
	switch (fv_filetype)
	{
		case FTYPE_PWS:
		case FTYPE_PHOTON:
		{
			resX = PFILE_GetResolutionX();
			resY = PFILE_GetResolutionY();

			if (f_lseek(&ufile, ldata_offset_begin) != FR_OK)
				return 0;

			res = PRINT_ReadRLEDecode(1);
		}
		break;

	}


	return res;
}
//==============================================================================



uint8_t		PRINT_ReadSublayerContinue()
{
	uint8_t		res = 0;
	switch (fv_filetype)
	{
		case FTYPE_PWS:
		case FTYPE_PHOTON:
		{
			if (FPHOTON_GetVersion() > 2)
			{
				UINT	rd = 0;
				FPHOTON_LAYERSINFOEXT	i_infoext;
				if (f_read(&ufile, &i_infoext, sizeof(FPHOTON_LAYERSINFOEXT), &rd) != FR_OK || rd != sizeof(FPHOTON_LAYERSINFOEXT))
					return 0;
			}
			res = PRINT_ReadRLEDecode(0);
		}
		break;
	}


	return res;
}
//==============================================================================



void		PRINT_ClearLayerPreview()
{
	if (tguiActiveScreen == &tguiScreenPrint)
	{
		uint32_t	curpoint = 0;
		LCD_SetWindows(prev_position.left, prev_position.top, prevwidth, prevheight);
		LCD_WriteRAM_Prepare();
		while (curpoint < prevheight * prevwidth)
		{
			LCD_WriteRAM(prevcolors[0]);
			curpoint++;
		}
	}
}
//==============================================================================



void		PRINT_DrawLayerPreview()
{
	if (tguiActiveScreen == &tguiScreenPrint)
	{
		uint8_t		*pimage = previmage;
		uint32_t	curpoint = 0;
		uint8_t		curoffset = 0;
		uint8_t		curval = *pimage;
		uint32_t	prevcolumn = 0;
		
		LCD_WriteCmd(0x0036);
		if (cfgConfig.display_rotate == 0)
			LCD_WriteRAM(0x0098);
		else
			LCD_WriteRAM(0x0058);
		LCD_SetWindows(prev_position.top, prev_position.left, prevheight, prevwidth);
		LCD_WriteRAM_Prepare();

		while (prevcolumn < prevwidth)
		{
			pimage = previmage + prevcolumn * prevheightbytes;
			curval = *pimage;
			curpoint = 0;
			curoffset = 0;
			while (curpoint < prevheight)
			{
				LCD_WriteRAM(prevcolors[curval >> 7]);
				curval <<= 1;
				curpoint++;
				curoffset++;
				if (curoffset == 8)
				{
					pimage++;
					curval = *pimage;
					curoffset = 0;
				}
			}
			prevcolumn++;
		}
		LCD_WriteCmd(0x0036);
		if (cfgConfig.display_rotate == 0)
			LCD_WriteRAM(0x00B8);
		else
			LCD_WriteRAM(0x0078);
	}
}
//==============================================================================



