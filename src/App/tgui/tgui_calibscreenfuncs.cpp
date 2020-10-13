/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "tgui_calibscreenfuncs.h"
#include "tgui_defaultfuncs.h"
#include "config.h"


#define CALIB_TARGET_SIZE_X			(uint16_t)36
#define CALIB_TARGET_SIZE_Y			(uint16_t)36
#define CALIB_TARGET_IMAGE_FILE		(char*)"icn/icn_calib_target.cimg"

#define CALIB_XD1					(LCD_WIDTH * 90) / 100
#define CALIB_YD1					(LCD_HEIGHT * 50) / 100
#define CALIB_XD2					(LCD_WIDTH * 50) / 100
#define CALIB_YD2					(LCD_HEIGHT * 90) / 100
#define CALIB_XD3					(LCD_WIDTH * 10) / 100
#define CALIB_YD3					(LCD_HEIGHT * 10) / 100


uint8_t							calib_screen_state = 0;
uint16_t						calib_x[3], calib_y[3];


void		_tgui_CalibButtonProcess(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	TOUCH_STATES	*ts = (TOUCH_STATES*)param;;
	TOUCH_POINT		tp;
	
	if (*ts > TS_PREPRESSED)
	{
		Touch_GetLastCoordsRaw(&tp);
		switch (*ts)
		{
			case TS_SPRESSED:
				if (thisbtn->options.disabled == 0 && thisbtn->options.pressed == 0)
				{
					if (TGUI_PointInRect(&tp, &thisbtn->position) == 1)
					{
						BUZZ_TimerOn(cfgConfig.buzzer_touch);
						thisbtn->options.pressed = 1;
						Touch_SetWorked(TS_SPRESSED);
					}
				}
				break;

			case TS_LPRESSED:
				if (thisbtn->options.disabled == 0 && thisbtn->options.pressed == 1)
				{
					// call linked function
					if (thisbtn->funcs._call_longpress != NULL)
					{
						// call linked function
						BUZZ_TimerOn(cfgConfig.buzzer_touch);
						thisbtn->funcs._call_longpress((void*)thisbtn, NULL);
					}
					
					Touch_SetWorked(TS_LPRESSED);
				}
				break;

			case TS_SRELEASED:
				if (thisbtn->options.disabled == 0 && thisbtn->options.pressed == 1)
				{
					thisbtn->options.pressed = 0;
					// call linked function or predefined action
					if (thisbtn->funcs._call_press != NULL)
					{
						// call linked function or predefined action
						thisbtn->funcs._call_press((void*)thisbtn, NULL);
					}
					
					Touch_SetWorked(TS_SRELEASED);
				}
				break;

			case TS_LRELEASED:
				if (thisbtn->options.disabled == 0 && thisbtn->options.pressed == 1)
				{
					thisbtn->options.pressed = 0;
					Touch_SetWorked(TS_LRELEASED);
				}
				break;
		}
	}
}
//==============================================================================




void		_tgui_CalibScreenPaint(void *tguiobj, void *param)
{
	TG_SCREEN		*thisscr = (TG_SCREEN*)tguiobj;

	uint16_t		bwidth  = 0, bheight = 0, t_x = 0, t_y = 0;
	uint8_t			is_img = 0;
	uint32_t		readed = 0;
	
	uint16_t		oldcolor = LCDUI_SetColor(thisscr->textcolor);
	uint16_t		oldbackcolor = LCDUI_SetBackColor(thisscr->backcolor);
	LCDUI_FONT_TYPE oldfont = LCDUI_SetFont(thisscr->font);

	LCDUI_Clear();
	LCDUI_DrawText(LANG_GetString(LSTR_PRESS_CENTER_OF_POINT), LCDUI_TEXT_ALIGN_CENTER, 10, 75, 470);
	LCDUI_SetColor(LCDUI_RGB(0x94FF9C));

	switch(calib_screen_state)
	{
		case 0:
			t_x = CALIB_XD1;
			t_y = CALIB_YD1;
			break;

		case 1:
			t_x = CALIB_XD2;
			t_y = CALIB_YD2;
			break;

		case 2:
			t_x = CALIB_XD3;
			t_y = CALIB_YD3;
			break;

	}
	
	tstrcpy(tfname, SpiflPath);
	tstrcat_utf(tfname, CALIB_TARGET_IMAGE_FILE);

	if (f_open(&tguiFile, tfname, FA_OPEN_EXISTING | FA_READ) == FR_OK)
	{
		// Reading CIMG header
		if (f_read(&tguiFile, tguiDBuff, 4, &readed) == FR_OK && readed == 4)
		{
			// Image width
			bwidth = *(uint16_t*)(tguiDBuff);
			// Image height
			bheight = *(uint16_t*)(tguiDBuff+2);
			if (bheight & 0x8000)
			{
				// If standart row order
				bheight &= 0x7FFF;
			}
			is_img = 1;
		}
		f_close(&tguiFile);
	}

	if (is_img == 0)
	{
		LCDUI_FillCircle(t_x, t_y, 10);
	}
	else
	{
		_tgui_DrawFileCimg(CALIB_TARGET_IMAGE_FILE, t_x - (bwidth / 2), t_y - (bheight / 2));
	}

	
	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================



void		_tgui_CalibButtonPress(void *tguiobj, void *param)
{
	float			temp1, temp2;
	TOUCH_POINT		pt;
	
	Touch_GetLastCoordsRaw(&pt);
	switch(calib_screen_state)
	{
		case 0:
			calib_x[0] = pt.xc;
			calib_y[0] = pt.yc;
			calib_screen_state++;
			TGUI_ForceRepaint();
			break;

		case 1:
			calib_x[1] = pt.xc;
			calib_y[1] = pt.yc;
			calib_screen_state++;
			TGUI_ForceRepaint();
			break;

		case 2:
			calib_x[2] = pt.xc;
			calib_y[2] = pt.yc;
			// calculate coeffs
			// A
			temp1 = ((float) CALIB_XD1 * ((float) calib_y[1] - (float) calib_y[2])) + ((float) CALIB_XD2 * ((float) calib_y[2] - (float) calib_y[0])) + ((float) CALIB_XD3 * ((float) calib_y[0] - (float) calib_y[1]));
			temp2 = ((float) calib_x[0] * ((float) calib_y[1] - (float) calib_y[2])) + ((float) calib_x[1] * ((float) calib_y[2] - (float) calib_y[0])) + ((float) calib_x[2] * ((float) calib_y[0] - (float) calib_y[1]));
			cfgConfig.touch_cal[0] = temp1 / temp2;
			// B
			temp1 = (cfgConfig.touch_cal[0] * ((float) calib_x[2] - (float) calib_x[1])) + (float) CALIB_XD2 - (float) CALIB_XD3;
			temp2 = (float) calib_y[1] - (float) calib_y[2];
			cfgConfig.touch_cal[1] = temp1 / temp2;
			//C
			cfgConfig.touch_cal[2] = (float) CALIB_XD3 - (cfgConfig.touch_cal[0] * (float) calib_x[2]) - (cfgConfig.touch_cal[1] * (float) calib_y[2]);
			//D
			temp1 = ((float) CALIB_YD1 * ((float) calib_y[1] - (float) calib_y[2])) + ((float) CALIB_YD2 * ((float) calib_y[2] - (float) calib_y[0])) + ((float) CALIB_YD3 * ((float) calib_y[0] - (float) calib_y[1]));
			temp2 = ((float) calib_x[0] * ((float) calib_y[1] - (float) calib_y[2])) + ((float) calib_x[1] * ((float) calib_y[2] - (float) calib_y[0])) + ((float) calib_x[2] * ((float) calib_y[0] - (float) calib_y[1]));
			cfgConfig.touch_cal[3] = (float)temp1 / (float)temp2;
			//E
			temp1 = (cfgConfig.touch_cal[3] * ((float) calib_x[2] - (float) calib_x[1])) + (float) CALIB_YD2 - (float) CALIB_YD3;
			temp2 = (float) calib_y[1] - (float) calib_y[2];
			cfgConfig.touch_cal[4] = (float)temp1 / (float)temp2;
			//F
			cfgConfig.touch_cal[5] = (float) CALIB_YD3 - cfgConfig.touch_cal[3] * (float) calib_x[2] - cfgConfig.touch_cal[4] * (float) calib_y[2];

			CFG_SaveConfig();
			calib_screen_state = 0;
			tguiActiveScreen = (TG_SCREEN*)tguiScreenCalib.prevscreen;
			TGUI_MessageBoxOk(LANG_GetString(LSTR_COMPLETED), LANG_GetString(LSTR_MSG_CALIBRATING_SUCCESS));
			break;

	}

}
//==============================================================================



void		_tgui_CalibButtonLongPress(void *tguiobj, void *param)
{
	calib_screen_state = 0;
	tguiActiveScreen = (TG_SCREEN*)tguiScreenCalib.prevscreen;
	TGUI_ForceRepaint();
}
//==============================================================================




