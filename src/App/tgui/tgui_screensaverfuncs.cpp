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


extern TG_SCREEN				*tguiActiveScreen;
extern char						msg[512];

extern DATETIME_STRUCT			datetime;
DATETIME_STRUCT					old_datetime;



void		TGUI_ScreenSaverShow()
{
	tguiScreenSaver.prevscreen = tguiActiveScreen;
	DTIME_GetCurrentDatetime(&datetime);
	memcpy(&old_datetime, &datetime, sizeof(DATETIME_STRUCT));
	tguiActiveScreen = &tguiScreenSaver;
	TGUI_ForceRepaint();
}
//==============================================================================




void		_tgui_ScreenSaverPaint(void *tguiobj, void *param)
{
	uint16_t	oldbackcolor = LCDUI_SetBackColor(tguiScreenSaver.backcolor);
	LCDUI_Clear();
	LCDUI_SetBackColor(oldbackcolor);
	
	_tgui_ScreenSaverTimePaint();
	_tgui_ScreenSaverDatePaint();
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

	LCDUI_FillRect(46, 240, 370, 54);

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
	LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, 0, 252, 300);

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
	LCDUI_DrawText(LANG_GetString(str_id), 0, 320, 252);
	
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
		TGUI_ForceRepaint();
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
}
//==============================================================================




