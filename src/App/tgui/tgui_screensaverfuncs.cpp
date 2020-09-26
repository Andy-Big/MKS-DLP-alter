/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "tgui_screensaverfuncs.h"
#include "tgui_defaultfuncs.h"
#include "tgui_numenterscreenfuncs.h"
#include "config.h"
#include "datetime.h"
#include "printing.h"
#include "fatfs.h"
#include "usbh_platform.h"
#include "uvdisplay.h"


extern TG_SCREEN				*tguiActiveScreen;
extern char						msg[512];

extern uint8_t					UsbMounted;
extern FATFS					UsbFS;    /* File system object for USBH logical drive */

extern uint32_t 				SystemCoreClock;

extern DATETIME_STRUCT			datetime;
DATETIME_STRUCT					old_datetime;
uint32_t						old_prtlayer;



void		TGUI_ScreenSaverShow()
{
	tguiScreenSaver.prevscreen = tguiActiveScreen;
	DTIME_GetCurrentDatetime(&datetime);
	memcpy(&old_datetime, &datetime, sizeof(DATETIME_STRUCT));
	if (systemInfo.print_is_printing > 0)
	{
		old_prtlayer = systemInfo.print_current_layer;
	}
	tguiActiveScreen = &tguiScreenSaver;
	TGUI_ForceRepaint();

	if (systemInfo.print_is_printing == 0)
	{
		// USB
		if (UsbMounted)
		{
			f_mount(NULL, UsbPath, 1);
			memset(&UsbFS, 0, sizeof(FATFS));
			UsbMounted = 0;
		}
		USB_HOST_VbusFS(1);
		USB_HOST_DeInit();
		
		// UV LCD
		UVD_Off();

		// System Ticks (500 Hz)
		HAL_SYSTICK_Config(32000);

		// System Clock
		__HAL_RCC_SYSCLK_CONFIG(RCC_SYSCLKSOURCE_HSI);
		while (__HAL_RCC_GET_SYSCLK_SOURCE() != (RCC_SYSCLKSOURCE_HSI << RCC_CFGR_SWS_Pos));
	}
}
//==============================================================================




void		_tgui_ScreenSaverPaint(void *tguiobj, void *param)
{
	uint16_t	oldbackcolor = LCDUI_SetBackColor(tguiScreenSaver.backcolor);
	LCDUI_Clear();
	LCDUI_SetBackColor(oldbackcolor);
	
	_tgui_ScreenSaverTimePaint();
	_tgui_ScreenSaverDatePaint();
	
	if (systemInfo.print_is_printing > 0)
	{
		_tgui_ScreenSaverProgressPaint();
	}
}
//==============================================================================




void		_tgui_ScreenSaverTimePaint()
{
	uint16_t	oldcolor = LCDUI_SetColor(tguiScreenSaver.textcolor);
	uint16_t	oldbackcolor = LCDUI_SetBackColor(tguiScreenSaver.backcolor);
	LCDUI_FONT_TYPE oldfont = LCDUI_SetFont(LCDUI_FONT_H170NUM_LCD);

	if (datetime.seconds % 2)
		sprintf(msg, (char*)"%02u %02u", datetime.hours, datetime.minutes);
	else
		sprintf(msg, (char*)"%02u:%02u", datetime.hours, datetime.minutes);
	LCDUI_DrawText(msg, 0, 23, 23);

	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================




void		_tgui_ScreenSaverDatePaint()
{
	uint16_t	oldcolor = LCDUI_SetColor(tguiScreenSaver.backcolor);
	uint16_t	oldbackcolor = LCDUI_SetBackColor(tguiScreenSaver.backcolor);
	LCDUI_FONT_TYPE oldfont = LCDUI_SetFont(LCDUI_FONT_H36);

	LCDUI_FillRect(46, 210, 370, 54);

	LCDUI_SetColor(tguiScreenSaver.textcolor);
	LNG_STRING_ID	str_id = LSTR_SHORT_JANUARY;
	switch(datetime.month)
	{
		case 2:
			str_id = LSTR_SHORT_FEBRUARY;
			break;
		case 3:
			str_id = LSTR_SHORT_MARCH;
			break;
		case 4:
			str_id = LSTR_SHORT_APRIL;
			break;
		case 5:
			str_id = LSTR_SHORT_MAY;
			break;
		case 6:
			str_id = LSTR_SHORT_JUNE;
			break;
		case 7:
			str_id = LSTR_SHORT_JULY;
			break;
		case 8:
			str_id = LSTR_SHORT_AUGUST;
			break;
		case 9:
			str_id = LSTR_SHORT_SEPTEMBER;
			break;
		case 10:
			str_id = LSTR_SHORT_OCTOBER;
			break;
		case 11:
			str_id = LSTR_SHORT_NOVEMBER;
			break;
		case 12:
			str_id = LSTR_SHORT_DECEMBER;
			break;
	}
	sprintf(msg, (char*)"%u %s %u", datetime.date, LANG_GetString(str_id), datetime.year + 2000);
	LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, 0, 222, 310);

	LCDUI_SetColor(tguiScreenSaver.nametextcolor);
	str_id = LSTR_SHORT_SUNDAY;
	switch(datetime.weekday)
	{
		case 2:
			str_id = LSTR_SHORT_MONDAY;
			break;
		case 3:
			str_id = LSTR_SHORT_TUESDAY;
			break;
		case 4:
			str_id = LSTR_SHORT_WEDNESDAY;
			break;
		case 5:
			str_id = LSTR_SHORT_THURSDAY;
			break;
		case 6:
			str_id = LSTR_SHORT_FRIDAY;
			break;
		case 7:
			str_id = LSTR_SHORT_SATURDAY;
			break;
	}
	LCDUI_DrawText(LANG_GetString(str_id), 0, 330, 222);
	
	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================




void		_tgui_ScreenSaverProgressPaint()
{
	uint16_t	oldcolor = LCDUI_SetColor(tguiScreenSaver.textcolor);
	uint16_t	oldbackcolor = LCDUI_SetBackColor(tguiScreenSaver.backcolor);
	LCDUI_FONT_TYPE oldfont = LCDUI_SetFont(LCDUI_FONT_H36);

	LCDUI_SetColor(tguiScreenSaver.backcolor);
	LCDUI_FillRect(381, 277, 97, 36);
	LCDUI_SetColor(LCDUI_RGB(0x89C8D3));

	LCDUI_DrawRect(5, 275, 375, 40);
	
	float		f_proc = ((float)(systemInfo.print_current_layer + 1) / (float)PFILE_GetTotalLayers()) * 100;
	uint32_t	proc = (uint32_t)f_proc;
	f_proc = (369.0 / 100.0) * f_proc;
	uint32_t		pb_width = (uint32_t)f_proc;
	LCDUI_FillRect(8, 278, pb_width, 34);

	sprintf(msg, (char*)"%u%%", proc);
	LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, 380, 282, 479, -1);

	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================




void		_tgui_ScreenSaverProcess(void *tguiobj, void *param)
{
	TOUCH_STATES	ts = Touch_GetState();
	
	if (ts == TS_SPRESSED)
	{
		Touch_SetWorked(TS_SPRESSED);
		tguiActiveScreen = (TG_SCREEN*)tguiScreenSaver.prevscreen;

		// System Ticks (1000 Hz)
		HAL_SYSTICK_Config(SystemCoreClock / (1000U / HAL_TICK_FREQ_1KHZ));

		// System Clock
		__HAL_RCC_SYSCLK_CONFIG(RCC_SYSCLKSOURCE_PLLCLK);
		while (__HAL_RCC_GET_SYSCLK_SOURCE() != (RCC_SYSCLKSOURCE_PLLCLK << RCC_CFGR_SWS_Pos));

		TGUI_ForceRepaint();

		// UV LCD
		UVD_Init();

		// USB
		USB_HOST_Init();
		USB_HOST_VbusFS(0);
		
		return;
	}

	DTIME_GetCurrentDatetime(&datetime);
	if (old_datetime.seconds != datetime.seconds)
	{
		_tgui_ScreenSaverTimePaint();
		if (old_datetime.date != datetime.date)
		{
			_tgui_ScreenSaverDatePaint();
		}
		memcpy(&old_datetime, &datetime, sizeof(DATETIME_STRUCT));
	}
	if (systemInfo.print_is_printing > 0 && old_prtlayer != systemInfo.print_current_layer)
	{
		_tgui_ScreenSaverProgressPaint();
		old_prtlayer = systemInfo.print_current_layer;
	}
}
//==============================================================================




