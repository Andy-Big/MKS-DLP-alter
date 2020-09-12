/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "tgui_settingsscreenfuncs.h"
#include "tgui_defaultfuncs.h"
#include "tgui_numenterscreenfuncs.h"
#include "config.h"


extern TG_SCREEN				*tguiActiveScreen;
extern char						msg[512];




void		_tgui_SettingsItemButtonPaint(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	
	uint16_t oldcolor, newcolor = thisbtn->textcolor_en;
	uint16_t oldbackcolor, newbackcolor = thisbtn->backcolor_en;
	LCDUI_FONT_TYPE oldfont = LCDUI_SetFont(thisbtn->font);
	char *img = thisbtn->bgimagename_en;

	// colors
	if (thisbtn->options.disabled == 1)
	{
		newcolor = thisbtn->textcolor_dis;
		newbackcolor = thisbtn->backcolor_dis;
		img = thisbtn->bgimagename_dis;
	}
	else
	{
		if (thisbtn->options.pressed == 1 && thisbtn->options.repaintonpress == 1)
		{
			newcolor = thisbtn->textcolor_press;
			newbackcolor = thisbtn->backcolor_press;
			img = thisbtn->bgimagename_press;
		}
		else
		{
			if (thisbtn->options.active == 1)
			{
				newcolor = thisbtn->textcolor_act;
				newbackcolor = thisbtn->backcolor_act;
				img = thisbtn->bgimagename_act;
			}
		}
	}

	oldcolor = LCDUI_SetColor(newcolor);
	oldbackcolor = LCDUI_SetBackColor(newbackcolor);

	if (thisbtn->options.bgpaint == BGP_FILL)
	{
		LCDUI_SetColor(newbackcolor);
		LCDUI_FillRoundRect(thisbtn->position.left, thisbtn->position.top, thisbtn->position.right - thisbtn->position.left, thisbtn->position.bottom - thisbtn->position.top, 7);
		LCDUI_SetColor(newcolor);
	}
	else
	{
		if (thisbtn->options.bgpaint == BGP_SCREEN)
		{
		}
		else
		{
			if (thisbtn->options.bgpaint == BGP_IMAGE && img != NULL)
			{
				_tgui_DrawFileCimg(img, thisbtn->position.left, thisbtn->position.top);
			}
		}
	}

	uint16_t	ty = LCDUI_GetCurrentFontHeight();
	ty = thisbtn->textposition.top + ((thisbtn->textposition.bottom - thisbtn->textposition.top) - ty) / 2;
	// Name paint
	if (thisbtn->text != NULL)
	{
		char		*txt;
		// if "text" field is a pointer to memory
		if (thisbtn->text & 0xFF000000)
			txt = (char*)thisbtn->text;
		else
			txt = LANG_GetString(thisbtn->text);
		LCDUI_DrawText(txt, 0, thisbtn->textposition.left, ty);
	}
	
	// Value paint
	msg[0] = 0;
	switch (thisbtn->button_id)
	{
		case TG_SCR_SETTINGS_LIFTPAUSE_ID:
			sprintf(msg, (char*)"%0.1f", cfgConfig.pause_lift);
			break;

		case TG_SCR_SETTINGS_BUZZER_ID:
			if (cfgConfig.buzzer == 0)
				strcpy(msg, LANG_GetString(LSTR_OFF));
			else
				strcpy(msg, LANG_GetString(LSTR_ON));
			break;
	}

	if (msg[0] != 0)
	{
		LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT | LCDUI_TEXT_TRANSBACK, thisbtn->textposition.left, ty, thisbtn->textposition.right);
	}

	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================



void		_tgui_SettingsSaveButtonPress(void *tguiobj, void *param)
{
	TGUI_MessageBoxWait(LANG_GetString(LSTR_WAIT), LANG_GetString(LSTR_MSG_SAVING_SETTINGS));
	CFG_SaveConfig();
	if (tguiActiveScreen == (TG_SCREEN*)&tguiMsgBox)
	{
		tguiActiveScreen = (TG_SCREEN*)((TG_MSGBOX*)tguiActiveScreen)->prevscreen;
		TGUI_ForceRepaint();
	}
}
//==============================================================================



void		_tgui_SettingsPauseliftButtonPress(void *tguiobj, void *param)
{
	TGUI_NumenterScreenShow(LANG_GetString(LSTR_LIFT_ON_PAUSE), NT_UFLOAT, (void*)&(cfgConfig.pause_lift));
}
//==============================================================================



void		_tgui_SettingsBuzzerButtonPress(void *tguiobj, void *param)
{
	if (cfgConfig.buzzer == 0)
		cfgConfig.buzzer = 1;
	else
		cfgConfig.buzzer = 0;
	
	_tgui_SettingsItemButtonPaint(tguiobj, NULL);
}
//==============================================================================



