/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "tgui_infoscreenfuncs.h"
#include "tgui_defaultfuncs.h"
#include "config.h"
#include "cpld_utils.h"
#include "cpld_utils.h"


extern char				msg[512];


	
void		_tgui_InfoScreenInfoPaint(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;

	LCDUI_FONT_TYPE oldfont = LCDUI_SetFont(thisbtn->font);
	uint16_t 		oldcolor = LCDUI_SetColor(thisbtn->textcolor_en);
	uint16_t 		oldbackcolor = LCDUI_SetBackColor(LCDUI_RGB(0xE0E0E0));

	uint16_t	fntheight = LCDUI_GetCurrentFontHeight();
	uint16_t	yinc = (thisbtn->position.bottom - thisbtn->position.top - fntheight * 7) / 8;	// 8 text lines and 9 intervals
	uint16_t	ytop = thisbtn->position.top + yinc;
	// version
#ifdef __MKSDLP_BOARD__
	uint8_t		fpgav = _cpld_get_version();
#endif
#ifdef __CHITU_BOARD__
	uint8_t		fpgav = 0x00;
#endif
	LCDUI_SetColor(LCDUI_RGB(0x000000));
	LCDUI_DrawText(LANG_GetString(LSTR_VERSION), 0, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	LCDUI_SetColor(LCDUI_RGB(0x00496C));
	sprintf(msg, "mcu: v%0u.%02u / fpga: v%u", FW_VERSION >> 8, FW_VERSION & 0xFF, fpgav);
	LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	// light time
	ytop += fntheight + yinc;
	LCDUI_SetColor(LCDUI_RGB(0x000000));
	LCDUI_DrawFastHLine(thisbtn->position.left + 5, ytop - (yinc / 2) - 1, thisbtn->position.right - thisbtn->position.left - 10);
	LCDUI_DrawText(LANG_GetString(LSTR_LIGHT_TIME), 0, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	LCDUI_SetColor(LCDUI_RGB(0x00496C));
	sprintf(msg, "%u %s", cfgTimers.led_time / 3600, LANG_GetString(LSTR_SHORTHOUR));
	LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	// diaplay time
	ytop += fntheight + yinc;
	LCDUI_SetColor(LCDUI_RGB(0x000000));
	LCDUI_DrawFastHLine(thisbtn->position.left + 5, ytop - (yinc / 2) - 1, thisbtn->position.right - thisbtn->position.left - 10);
	LCDUI_DrawText(LANG_GetString(LSTR_DISPLAY_TIME), 0, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	LCDUI_SetColor(LCDUI_RGB(0x00496C));
	sprintf(msg, "%u %s", cfgTimers.disp_time / 3600, LANG_GetString(LSTR_SHORTHOUR));
	LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	// fan time
	ytop += fntheight + yinc;
	LCDUI_SetColor(LCDUI_RGB(0x000000));
	LCDUI_DrawFastHLine(thisbtn->position.left + 5, ytop - (yinc / 2) - 1, thisbtn->position.right - thisbtn->position.left - 10);
	LCDUI_DrawText(LANG_GetString(LSTR_FAN_TIME), 0, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	LCDUI_SetColor(LCDUI_RGB(0x00496C));
	sprintf(msg, "%u %s", cfgTimers.fan_time / 3600, LANG_GetString(LSTR_SHORTHOUR));
	LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	// total print time
	ytop += fntheight + yinc;
	LCDUI_SetColor(LCDUI_RGB(0x000000));
	LCDUI_DrawFastHLine(thisbtn->position.left + 5, ytop - (yinc / 2) - 1, thisbtn->position.right - thisbtn->position.left - 10);
	LCDUI_DrawText(LANG_GetString(LSTR_TOTALPRINT_TIME), 0, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	LCDUI_SetColor(LCDUI_RGB(0x00496C));
	sprintf(msg, "%u %s", cfgTimers.total_print_time / 3600, LANG_GetString(LSTR_SHORTHOUR));
	LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	// URL 
	ytop += fntheight + yinc;
	LCDUI_SetColor(LCDUI_RGB(0x000000));
	LCDUI_DrawFastHLine(thisbtn->position.left + 5, ytop - (yinc / 2) - 1, thisbtn->position.right - thisbtn->position.left - 10);
	LCDUI_DrawText(LANG_GetString(LSTR_URL), 0, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	LCDUI_SetColor(LCDUI_RGB(0x00496C));
	if (LCDUI_GetScreenWidth() == 320)
	{
		LCDUI_SetFont(LCDUI_FONT_H14);
		ytop += 2;
	}
	LCDUI_DrawText((char*)"github.com/Andy-Big/MKS-DLP-alter", LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	if (LCDUI_GetScreenWidth() == 320)
	{
		LCDUI_SetFont(LCDUI_FONT_H18);
		ytop -= 2;
	}
	// UI author 
	ytop += fntheight + yinc;
	LCDUI_SetColor(LCDUI_RGB(0x000000));
	LCDUI_DrawFastHLine(thisbtn->position.left + 5, ytop - (yinc / 2) - 1, thisbtn->position.right - thisbtn->position.left - 10);
	LCDUI_DrawText(LANG_GetString(LSTR_UIAUTHOR), 0, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	LCDUI_SetColor(LCDUI_RGB(0x00496C));
	LCDUI_DrawText((char*)"andybig@gmail.com", LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
}
//==============================================================================




