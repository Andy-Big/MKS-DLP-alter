/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "tgui_uvtestscreenfuncs.h"
#include "tgui_defaultfuncs.h"
#include "config.h"
#include "sys_timer.h"
#include "uvdisplay.h"


extern char				msg[512];
extern FIL				sfile PLACE_TO_CCMRAM;
extern TCHAR			s_tfname[512];

extern uint8_t			Line_Pixel[CPLD_Y_RATIO + CPLD_FILLCODE * 2];
#ifdef __MKSDLP_BOARD__
extern DLP_BMP			cpld_bmp;
#endif  // __MKSDLP_BOARD__


uint8_t		uvlight_on = 0;
uint8_t		uvdisp_on = 0;
uint8_t		uv_image = 0;



void		_tgui_UVTestEnableLight()
{
	UVFAN_TimerOn(130000);
	UVLED_TimerOn(120000);
	uvlight_on = 1;
}
//==============================================================================




void		_tgui_UVTestDisableLight()
{
	UVFAN_TimerOn(10000);
	UVLED_Off();
	uvlight_on = 0;
}
//==============================================================================




void		_tgui_UVTestEnableDisp()
{
	if (uvdisp_on != 0)
		return;
#ifdef __MKSDLP_BOARD__
	_cpld_bank2disp_enable(TEST_USED_BANK, 1, 1);
#endif  // __MKSDLP_BOARD__
	HAL_Delay(100);
	UVD_Wakeup();
	uvdisp_on = 1;
}
//==============================================================================




void		_tgui_UVTestDisableDisp()
{
	if (uvdisp_on == 0)
		return;
	UVD_Sleep();
	HAL_Delay(100);
#ifdef __MKSDLP_BOARD__
	_cpld_bank2disp_enable(TEST_USED_BANK, 0, 0);
#endif  // __MKSDLP_BOARD__
	uvdisp_on = 0;
}
//==============================================================================




void		_tgui_UVTestReadImage(uint8_t imgnum)
{
	uint8_t		disp_has_en = uvdisp_on;
	if (disp_has_en != 0)
		_tgui_UVTestDisableDisp();

#ifdef __MKSDLP_BOARD__

	uint16_t	color_f;
	uint8_t		color;
	uint8_t		length;
	uint16_t	curpoint;
	uint8_t		*p;
	uint8_t		remaining;
	UINT	rd = 0;

	tstrcpy(tfname, SpiflPath);
	switch (imgnum)
	{
		case 1:
			tstrcat_utf(tfname, (char*)"icn/uvtest_image2.cimg");
			break;
		case 2:
			tstrcat_utf(tfname, (char*)"icn/uvtest_image3.cimg");
			break;
		default:
			tstrcat_utf(tfname, (char*)"icn/uvtest_image1.cimg");
			break;
	}
	
	if (f_open(&sfile, tfname, FA_OPEN_EXISTING | FA_READ) != FR_OK)
		return;
	if (f_lseek(&sfile, 4) != FR_OK)
		return;

	cpld_bmp.current_line = 0;
	p = Line_Pixel;
	curpoint = 0;
	while(cpld_bmp.current_line < 2560)
	{
		while (curpoint < CPLD_Y_RATIO / 2)
		{
			if (f_read(&sfile, &length, 1, &rd) != FR_OK || rd != 1)
				return;
			if (f_read(&sfile, &color_f, 2, &rd) != FR_OK || rd != 2)
				return;
			color = color_f >> 15;

			if (length & 0x80)
				length = (length & 0x7F) + 1;
			else
				length++;

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
			if (f_read(&sfile, &length, 1, &rd) != FR_OK || rd != 1)
				return;
			if (f_read(&sfile, &color_f, 2, &rd) != FR_OK || rd != 2)
				return;
			color = color_f >> 15;

			if (length & 0x80)
				length = (length & 0x7F) + 1;
			else
				length++;

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
		_cpld_line_gen_data(cpld_bmp.current_line, TEST_USED_BANK);
		while (FLASH_IsDMAReady() == 0);

		cpld_bmp.current_line++;
		p = &Line_Pixel[0];
		memset(p, color, remaining);
		p += remaining;
		curpoint = remaining;
		remaining = 0;
	}

#endif  // __MKSDLP_BOARD__

	if (disp_has_en != 0)
		_tgui_UVTestEnableDisp();
}
//==============================================================================




void		_tgui_UVTestBackButtonPress(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;

	if (uvdisp_on != 0)
	{
		_tgui_UVTestDisableDisp();
	}
	if (uvlight_on != 0)
	{
		_tgui_UVTestDisableLight();
	}

	thisbtn->options.pressed = 0;
	tguiActiveScreen = (TG_SCREEN*)tguiActiveScreen->prevscreen;
	TGUI_ForceRepaint();
}
//==============================================================================




void		_tgui_UVTestImgSelectButtonPress(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;

	uint8_t			currid = thisbtn->button_id;
	uint8_t			new_uv_image = 0;
	switch (currid)
	{
		case TG_SCR_UVTEST_IMG2_BTN_ID:
			new_uv_image = 1;
			break;
		case TG_SCR_UVTEST_IMG3_BTN_ID:
			new_uv_image = 2;
			break;
	}

	if (new_uv_image != uv_image)
	{
		uv_image = new_uv_image;
		TGUI_MessageBoxWait(LANG_GetString(LSTR_WAIT), LANG_GetString(LSTR_IMAGE_LOADING));
		_tgui_UVTestReadImage(new_uv_image);
		tguiActiveScreen = (TG_SCREEN*)((TG_MSGBOX*)tguiActiveScreen)->prevscreen;
		TGUI_ForceRepaint();
	}
}
//==============================================================================




void		_tgui_UVTestLightButtonPress(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	
	if (uvlight_on != 0)
		_tgui_UVTestDisableLight();
	else
		_tgui_UVTestEnableLight();
	
	thisbtn->options.needrepaint = 1;
}
//==============================================================================




void		_tgui_UVTestDispButtonPress(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	
	if (uvdisp_on != 0)
		_tgui_UVTestDisableDisp();
	else
		_tgui_UVTestEnableDisp();
	
	thisbtn->options.needrepaint = 1;
}
//==============================================================================




void		_tgui_UVTestLightButtonPaint(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;

	strcpy(msg, LANG_GetString(LSTR_UVTEST_LIGHT));
	if (uvlight_on != 0)
		strcat(msg, LANG_GetString(LSTR_ON_CAPS));
	else
		strcat(msg, LANG_GetString(LSTR_OFF_CAPS));
	
	thisbtn->text = (LNG_STRING_ID)((DWORD)msg);
	_tgui_DefaultButtonPaint(tguiobj, param);
}
//==============================================================================




void		_tgui_UVTestDispButtonPaint(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;

	strcpy(msg, LANG_GetString(LSTR_UVTEST_DISPLAY));
	if (uvdisp_on != 0)
		strcat(msg, LANG_GetString(LSTR_ON_CAPS));
	else
		strcat(msg, LANG_GetString(LSTR_OFF_CAPS));
	
	thisbtn->text = (LNG_STRING_ID)((DWORD)msg);
	_tgui_DefaultButtonPaint(tguiobj, param);
}
//==============================================================================



