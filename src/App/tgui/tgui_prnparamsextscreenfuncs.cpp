/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "tgui_prnparamsextscreenfuncs.h"
#include "tgui_prnparamsscreenfuncs.h"
#include "tgui_defaultfuncs.h"
#include "tgui_numenterscreenfuncs.h"
#include "config.h"
#include "prtfiles.h"
#include "z_planner.h"


#define PPEXT_SCREENS_COUNT		2


extern TG_SCREEN				*tguiActiveScreen;
extern char						msg[512];

PRNPARAMSEXT_VALUES				prnparamsext;

uint8_t							ppext_current_screen = 0;


void		_tgui_PrnparamsExtScreenShow(void *tguiobj, void *param)
{
	tguiScreenPrint.prevscreen = &tguiScreenPrnparams;
	tguiActiveScreen = &tguiScreenPrnparamsExt;

	prnparamsext.pause_lift = cfgConfig.pause_lift;
	prnparamsext.end_lift = cfgConfig.end_lift;
	prnparamsext.accelerate = cfgMotor.acceleration;

	TGUI_ForceRepaint();
}
//==============================================================================



void		_tgui_PrnparamsExtScreenPaint(void *tguiobj, void *param)
{
	_tgui_DefaultScreenPaint(tguiobj, param);
}
//==============================================================================



void		_tgui_PrnparamsExtItemButtonPaint(void *tguiobj, void *param)
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

	
	// Value paint
	msg[0] = 0;
	char	*meas = NULL;
	
	if (ppext_current_screen == 0)
	{
		switch (thisbtn->button_id)
		{
			case TG_SCR_PRNPARAMSEXT_PAUSELIFT_ID:
				sprintf(msg, (char*)"%0.1f", prnparamsext.pause_lift);
				meas = LANG_GetString(LSTR_SHORTMILLIMETERS);
				break;

			case TG_SCR_PRNPARAMSEXT_ENDLIFT_ID:
				sprintf(msg, (char*)"%0.1f", prnparamsext.end_lift);
				meas = LANG_GetString(LSTR_SHORTMILLIMETERS);
				break;

			case TG_SCR_PRNPARAMSEXT_ACCELERATE_ID:
				sprintf(msg, (char*)"%0.1f", prnparamsext.accelerate);
				meas = LANG_GetString(LSTR_MM_SEC);
				break;

		}
	}

	if (msg[0] != 0)
	{
		uint16_t	meas_w = 0;
		uint16_t	ty = LCDUI_GetCurrentFontHeight();
		uint16_t	meas_h1 = LCDUI_GetCurrentFontHeight();

		if (thisbtn->options.bgpaint == BGP_FILL)
		{
			LCDUI_SetColor(newbackcolor);
			LCDUI_FillRoundRect(thisbtn->position.left, thisbtn->position.top, thisbtn->position.right - thisbtn->position.left, thisbtn->position.bottom - thisbtn->position.top, 7);
			LCDUI_SetColor(newcolor);
		}
		else
		{
			if (thisbtn->options.bgpaint == BGP_IMAGE && img != NULL)
			{
				_tgui_DrawFileCimg(img, thisbtn->position.left, thisbtn->position.top);
			}
		}

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

		if (meas != NULL)
		{
			LCDUI_SetFont(LCDUI_FONT_H18);
			meas_w = LCDUI_GetTextWidth(meas) + 6;
			uint16_t	meas_h2 = meas_h1 - LCDUI_GetCurrentFontHeight();
			LCDUI_DrawText(meas, LCDUI_TEXT_ALIGN_RIGHT | LCDUI_TEXT_TRANSBACK, thisbtn->textposition.left, ty + meas_h2, thisbtn->textposition.right);
			LCDUI_SetFont(thisbtn->font);
		}
		LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT | LCDUI_TEXT_TRANSBACK, thisbtn->textposition.left, ty, thisbtn->textposition.right - meas_w);
	}

	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================



void		_tgui_PrnparamsExtItemSolidButtonPaint(void *tguiobj, void *param)
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
		if (thisbtn->options.bgpaint == BGP_IMAGE && img != NULL)
		{
			_tgui_DrawFileCimg(img, thisbtn->position.left, thisbtn->position.top);
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

	LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT | LCDUI_TEXT_TRANSBACK, thisbtn->textposition.left, ty, thisbtn->textposition.right);

	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================



void		_tgui_PrnparamsExtDownButtonPress(void *tguiobj, void *param)
{
	if (ppext_current_screen < PPEXT_SCREENS_COUNT - 1)
	{
		ppext_current_screen++;
		for (uint8_t i = 0; i < tguiScreenPrnparamsExt.btns_count; i++)
		{
			switch (tguiScreenPrnparamsExt.buttons[i].button_id)
			{
				case TG_SCR_PRNPARAMSEXT_UP_ID:
					tguiScreenPrnparamsExt.buttons[i].options.disabled = 0;
					break;

				case TG_SCR_PRNPARAMSEXT_DOWN_ID:
					if (ppext_current_screen < PPEXT_SCREENS_COUNT - 1)
						tguiScreenPrnparamsExt.buttons[i].options.disabled = 0;
					else
						tguiScreenPrnparamsExt.buttons[i].options.disabled = 1;
					break;

				case TG_SCR_PRNPARAMSEXT_PAUSELIFT_ID:
				case TG_SCR_PRNPARAMSEXT_ENDLIFT_ID:
				case TG_SCR_PRNPARAMSEXT_ACCELERATE_ID:
					if (ppext_current_screen == 0)
						tguiScreenPrnparamsExt.buttons[i].options.disabled = 0;
					else
						tguiScreenPrnparamsExt.buttons[i].options.disabled = 1;
					break;

			}
		}
		TGUI_ForceRepaint();
	}
}
//==============================================================================



void		_tgui_PrnparamsExtUpButtonPress(void *tguiobj, void *param)
{
	if (ppext_current_screen > 0)
	{
		ppext_current_screen--;
		for (uint8_t i = 0; i < tguiScreenPrnparamsExt.btns_count; i++)
		{
			switch (tguiScreenPrnparamsExt.buttons[i].button_id)
			{
				case TG_SCR_PRNPARAMSEXT_UP_ID:
					if (ppext_current_screen > 0)
						tguiScreenPrnparamsExt.buttons[i].options.disabled = 0;
					else
						tguiScreenPrnparamsExt.buttons[i].options.disabled = 1;
					break;

				case TG_SCR_PRNPARAMSEXT_DOWN_ID:
					tguiScreenPrnparamsExt.buttons[i].options.disabled = 0;
					break;

				case TG_SCR_PRNPARAMSEXT_PAUSELIFT_ID:
				case TG_SCR_PRNPARAMSEXT_ENDLIFT_ID:
				case TG_SCR_PRNPARAMSEXT_ACCELERATE_ID:
					if (ppext_current_screen == 0)
						tguiScreenPrnparamsExt.buttons[i].options.disabled = 0;
					else
						tguiScreenPrnparamsExt.buttons[i].options.disabled = 1;
					break;

			}
		}
		TGUI_ForceRepaint();
	}
}
//==============================================================================



void		_tgui_PrnparamsExtItemButtonPress(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	thisbtn->options.pressed = 0;
	switch (thisbtn->button_id)
	{
		case TG_SCR_PRNPARAMSEXT_PAUSELIFT_ID:
			TGUI_NumenterScreenShow(LANG_GetString(LSTR_PPEXT_LIFT_PAUSE), NT_UFLOAT, (void*)&(prnparamsext.pause_lift));
			break;

		case TG_SCR_PRNPARAMSEXT_ENDLIFT_ID:
			TGUI_NumenterScreenShow(LANG_GetString(LSTR_PPEXT_LIFT_END), NT_UFLOAT, (void*)&(prnparamsext.end_lift));
			break;

		case TG_SCR_PRNPARAMSEXT_ACCELERATE_ID:
			TGUI_NumenterScreenShow(LANG_GetString(LSTR_PPEXT_ACCELERATE), NT_UFLOAT, (void*)&(prnparamsext.accelerate));
			break;

	}
}
//==============================================================================



void		_tgui_PrnparamsExtOkButtonPress(void *tguiobj, void *param)
{
	cfgConfig.pause_lift = prnparamsext.pause_lift;
	cfgConfig.end_lift = prnparamsext.end_lift;
	cfgMotor.acceleration = prnparamsext.accelerate;
	zPlanner.settings.acceleration = prnparamsext.accelerate;

	tguiActiveScreen = (TG_SCREEN*)tguiActiveScreen->prevscreen;
	TGUI_ForceRepaint();
}
//==============================================================================



void		_tgui_PrnparamsExtSaveButtonPress(void *tguiobj, void *param)
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



