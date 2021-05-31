/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "tgui_clocksetscreenfuncs.h"
#include "tgui_defaultfuncs.h"
#include "tgui_numenterscreenfuncs.h"
#include "config.h"
#include "datetime.h"
#include "eeprom.h"
#include "ds3231.h"


extern TG_SCREEN				*tguiActiveScreen;
extern char						msg[512];

const uint8_t					days_in_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

DATETIME_STRUCT					datetime;



void		TGUI_ClockSetScreenShow()
{
	tguiScreenClockSet.prevscreen = tguiActiveScreen;
	DTIME_GetCurrentDatetime(&datetime);
	tguiActiveScreen = &tguiScreenClockSet;
	TGUI_ForceRepaint();
}
//==============================================================================




void		_tgui_ClockSetUpdateItem(uint32_t item_id)
{
	for (uint8_t i = 0; i < tguiScreenClockSet.btns_count; i++)
	{
		if (tguiScreenClockSet.buttons[i].button_id == item_id)
			_tgui_ClockSetItemPaint((void*)&tguiScreenClockSet.buttons[i], NULL);
	}
}
//==============================================================================




void		_tgui_ClockSetScreenPaint(void *tguiobj, void *param)
{
	_tgui_DefaultScreenPaint((void*)&tguiScreenClockSet, NULL);
	
	uint16_t	oldcolor = LCDUI_SetColor(tguiScreenClockSet.textcolor);
	LCDUI_FONT_TYPE oldfont = LCDUI_SetFont(tguiScreenClockSet.font);

	uint16_t	ty = LCDUI_GetCurrentFontHeight() + 1;
	for (uint8_t i = 0; i < tguiScreenClockSet.btns_count; i++)
	{
		if (tguiScreenClockSet.buttons[i].button_id == TG_SCR_CLOCKSET_HOURS_BTN_ID)
			LCDUI_DrawText(LANG_GetString(LSTR_CLOCK_HOURS_NAME), LCDUI_TEXT_ALIGN_CENTER | LCDUI_TEXT_TRANSBACK, tguiScreenClockSet.buttons[i].position.left, tguiScreenClockSet.buttons[i].textposition.top - ty, tguiScreenClockSet.buttons[i].position.right);
		else if (tguiScreenClockSet.buttons[i].button_id == TG_SCR_CLOCKSET_MINUTES_BTN_ID)
			LCDUI_DrawText(LANG_GetString(LSTR_CLOCK_MINUTES_NAME), LCDUI_TEXT_ALIGN_CENTER | LCDUI_TEXT_TRANSBACK, tguiScreenClockSet.buttons[i].position.left, tguiScreenClockSet.buttons[i].textposition.top - ty, tguiScreenClockSet.buttons[i].position.right);
		else if (tguiScreenClockSet.buttons[i].button_id == TG_SCR_CLOCKSET_DAY_BTN_ID)
			LCDUI_DrawText(LANG_GetString(LSTR_CLOCK_DAY_NAME), LCDUI_TEXT_ALIGN_CENTER | LCDUI_TEXT_TRANSBACK, tguiScreenClockSet.buttons[i].position.left, tguiScreenClockSet.buttons[i].textposition.top - ty, tguiScreenClockSet.buttons[i].position.right);
		else if (tguiScreenClockSet.buttons[i].button_id == TG_SCR_CLOCKSET_MONTH_BTN_ID)
			LCDUI_DrawText(LANG_GetString(LSTR_CLOCK_MONTH_NAME), LCDUI_TEXT_ALIGN_CENTER | LCDUI_TEXT_TRANSBACK, tguiScreenClockSet.buttons[i].position.left, tguiScreenClockSet.buttons[i].textposition.top - ty, tguiScreenClockSet.buttons[i].position.right);
		else if (tguiScreenClockSet.buttons[i].button_id == TG_SCR_CLOCKSET_YEAR_BTN_ID)
			LCDUI_DrawText(LANG_GetString(LSTR_CLOCK_YEAR_NAME), LCDUI_TEXT_ALIGN_CENTER | LCDUI_TEXT_TRANSBACK, tguiScreenClockSet.buttons[i].position.left, tguiScreenClockSet.buttons[i].textposition.top - ty, tguiScreenClockSet.buttons[i].position.right);
	}

	LCDUI_SetColor(oldcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================




void		_tgui_ClockSetItemPaint(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	
	uint16_t oldcolor = LCDUI_SetColor(thisbtn->textcolor_en);
	uint16_t oldbackcolor = LCDUI_SetBackColor(thisbtn->backcolor_en);
	LCDUI_FONT_TYPE oldfont = LCDUI_SetFont(thisbtn->font);

	LCDUI_SetColor(thisbtn->backcolor_en);
	LCDUI_FillRect(thisbtn->textposition.left, thisbtn->textposition.top, thisbtn->textposition.right - thisbtn->textposition.left, thisbtn->textposition.bottom - thisbtn->textposition.top);
	LCDUI_SetColor(thisbtn->textcolor_en);

	uint16_t	ty = LCDUI_GetCurrentFontHeight();
	ty = thisbtn->textposition.top + ((thisbtn->textposition.bottom - thisbtn->textposition.top) - ty) / 2;

	switch (thisbtn->button_id)
	{
		case TG_SCR_CLOCKSET_HOURS_BTN_ID:
			sprintf(msg, (char*)"%02u", datetime.hours);
			break;

		case TG_SCR_CLOCKSET_MINUTES_BTN_ID:
			sprintf(msg, (char*)"%02u", datetime.minutes);
			break;

		case TG_SCR_CLOCKSET_DAY_BTN_ID:
			sprintf(msg, (char*)"%u", datetime.date);
			break;

		case TG_SCR_CLOCKSET_MONTH_BTN_ID:
			sprintf(msg, (char*)"%02u", datetime.month);
			break;

		case TG_SCR_CLOCKSET_YEAR_BTN_ID:
			sprintf(msg, (char*)"%02u", datetime.year);
			break;

	}
	LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->textposition.left, ty, thisbtn->textposition.right);
	
	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================




void		_tgui_ClockSetUpButtonPress(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	uint32_t		item_id = 0;

	switch (thisbtn->button_id)
	{
		case TG_SCR_CLOCKSET_HOURS_UP_BTN_ID:
			datetime.hours++;
			if (datetime.hours > 23)
				datetime.hours = 0;
			item_id = TG_SCR_CLOCKSET_HOURS_BTN_ID;
			break;

		case TG_SCR_CLOCKSET_MINUTES_UP_BTN_ID:
			datetime.minutes++;
			if (datetime.minutes > 59)
				datetime.minutes = 0;
			item_id = TG_SCR_CLOCKSET_MINUTES_BTN_ID;
			break;

		case TG_SCR_CLOCKSET_DAY_UP_BTN_ID:
			datetime.date++;
			if (datetime.month == 2 && (datetime.year % 4) == 0)
			{
				if (datetime.date > 29)
					datetime.date = 1;
			}
			else
			{
				if (datetime.date > days_in_month[datetime.month - 1])
					datetime.date = 1;
			}
			item_id = TG_SCR_CLOCKSET_DAY_BTN_ID;
			break;

		case TG_SCR_CLOCKSET_MONTH_UP_BTN_ID:
			datetime.month++;
			if (datetime.month > 12)
				datetime.month = 1;
			if (datetime.month == 2 && (datetime.year % 4) == 0)
			{
				if (datetime.date > 29)
				{
					datetime.date = 29;
					_tgui_ClockSetUpdateItem(TG_SCR_CLOCKSET_DAY_BTN_ID);
				}
			}
			else
			{
				if (datetime.date > days_in_month[datetime.month - 1])
				{
					datetime.date = days_in_month[datetime.month - 1];
					_tgui_ClockSetUpdateItem(TG_SCR_CLOCKSET_DAY_BTN_ID);
				}
			}
			item_id = TG_SCR_CLOCKSET_MONTH_BTN_ID;
			break;

		case TG_SCR_CLOCKSET_YEAR_UP_BTN_ID:
			datetime.year++;
			if (datetime.year > 99)
				datetime.year = 0;
			if (datetime.month == 2 && (datetime.year % 4) == 0)
			{
				if (datetime.date > 29)
				{
					datetime.date = 29;
					_tgui_ClockSetUpdateItem(TG_SCR_CLOCKSET_DAY_BTN_ID);
				}
			}
			else
			{
				if (datetime.date > days_in_month[datetime.month - 1])
				{
					datetime.date = days_in_month[datetime.month - 1];
					_tgui_ClockSetUpdateItem(TG_SCR_CLOCKSET_DAY_BTN_ID);
				}
			}
			item_id = TG_SCR_CLOCKSET_YEAR_BTN_ID;
			break;

	}
	_tgui_ClockSetUpdateItem(item_id);
}
//==============================================================================




void		_tgui_ClockSetDownButtonPress(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	uint32_t		item_id = 0;

	switch (thisbtn->button_id)
	{
		case TG_SCR_CLOCKSET_HOURS_DOWN_BTN_ID:
			if (datetime.hours > 0)
				datetime.hours--;
			else
				datetime.hours = 23;
			item_id = TG_SCR_CLOCKSET_HOURS_BTN_ID;
			break;

		case TG_SCR_CLOCKSET_MINUTES_DOWN_BTN_ID:
			if (datetime.minutes > 0)
				datetime.minutes--;
			else
				datetime.minutes = 59;
			item_id = TG_SCR_CLOCKSET_MINUTES_BTN_ID;
			break;

		case TG_SCR_CLOCKSET_DAY_DOWN_BTN_ID:
			if (datetime.date > 1)
				datetime.date--;
			else
			if (datetime.month == 2 && (datetime.year % 4) == 0)
			{
					datetime.date = 29;
			}
			else
			{
				datetime.date = days_in_month[datetime.month - 1];
			}
			item_id = TG_SCR_CLOCKSET_DAY_BTN_ID;
			break;

		case TG_SCR_CLOCKSET_MONTH_DOWN_BTN_ID:
			if (datetime.month > 1)
				datetime.month--;
			else
				datetime.month = 12;
			if (datetime.month == 2 && (datetime.year % 4) == 0)
			{
				if (datetime.date > 29)
				{
					datetime.date = 29;
					_tgui_ClockSetUpdateItem(TG_SCR_CLOCKSET_DAY_BTN_ID);
				}
			}
			else
			{
				if (datetime.date > days_in_month[datetime.month - 1])
				{
					datetime.date = days_in_month[datetime.month - 1];
					_tgui_ClockSetUpdateItem(TG_SCR_CLOCKSET_DAY_BTN_ID);
				}
			}
			item_id = TG_SCR_CLOCKSET_MONTH_BTN_ID;
			break;

		case TG_SCR_CLOCKSET_YEAR_DOWN_BTN_ID:
			if (datetime.year > 0)
				datetime.year--;
			else
				datetime.year = 99;
			if (datetime.month == 2 && (datetime.year % 4) == 0)
			{
				if (datetime.date > 29)
				{
					datetime.date = 29;
					_tgui_ClockSetUpdateItem(TG_SCR_CLOCKSET_DAY_BTN_ID);
				}
			}
			else
			{
				if (datetime.date > days_in_month[datetime.month - 1])
				{
					datetime.date = days_in_month[datetime.month - 1];
					_tgui_ClockSetUpdateItem(TG_SCR_CLOCKSET_DAY_BTN_ID);
				}
			}
			item_id = TG_SCR_CLOCKSET_YEAR_BTN_ID;
			break;

	}
	_tgui_ClockSetUpdateItem(item_id);
}
//==============================================================================




void		_tgui_ClockSetOkButtonPress(void *tguiobj, void *param)
{
	datetime.seconds = 0;
	DTIME_SetWeekDay(&datetime);
	DTIME_SetCurrentDatetime(&datetime);
#ifdef __MKSDLP_BOARD__
	if (cfgConfig.use_ext_clock)
	{
		EEPROM_Deinit();
		if ((ds_state = DS3231_Init()) == 0)
		{
			TGUI_MessageBoxOk(LANG_GetString(LSTR_WARNING), LANG_GetString(LSTR_MSG_EXT_CLOCK_ERROR), _tgui_ClockSetClose);
		} else
		if (ds_state == 2)
		{
			if (DS3231_StartOsc() == 0)
			{
				TGUI_MessageBoxOk(LANG_GetString(LSTR_WARNING), LANG_GetString(LSTR_MSG_EXT_CLOCK_ERROR), _tgui_ClockSetClose);
				ds_state = 0;
			}
		} else
		if (DS3231_SetTime(&datetime, 0) == 0)
		{
			TGUI_MessageBoxOk(LANG_GetString(LSTR_WARNING), LANG_GetString(LSTR_MSG_EXT_CLOCK_ERROR), _tgui_ClockSetClose);
			ds_state = 0;
		}
		DS3231_Deinit();
		EEPROM_Init();
		if (ds_state == 0)
			return;
	}
#endif  // __MKSDLP_BOARD__

	_tgui_ClockSetClose(tguiobj, param);
}
//==============================================================================



void		_tgui_ClockSetClose(void *tguiobj, void *param)
{
	tguiActiveScreen = (TG_SCREEN*)tguiScreenClockSet.prevscreen;
	TGUI_ForceRepaint();
}
//==============================================================================



