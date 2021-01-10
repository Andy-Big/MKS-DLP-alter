/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "tgui_prnparamsscreenfuncs.h"
#include "tgui_prnparamsextscreenfuncs.h"
#include "tgui_defaultfuncs.h"
#include "tgui_numenterscreenfuncs.h"
#include "config.h"
#include "prtfiles.h"


#define PP_SCREENS_COUNT		2


extern TG_SCREEN				*tguiActiveScreen;
extern char						msg[512];

PRNPARAMS_VALUES				prnparams;

uint8_t							pp_current_screen = 0;


void		_tgui_PrnparamsScreenShow(void *tguiobj, void *param)
{
	tguiScreenPrint.prevscreen = &tguiScreenPrint;
	tguiActiveScreen = &tguiScreenPrnparams;

	prnparams.antialiasing = systemInfo.print_use_aa_grade;
	prnparams.bottom_light = PFILE_GetLightBottom();
	prnparams.drop_speed = PFILE_GetDropSpeed();
	prnparams.ind_params = systemInfo.print_use_ind_params;
	prnparams.lift_height = PFILE_GetLiftHeight();
	prnparams.lift_speed = PFILE_GetLiftSpeed();
	prnparams.light = PFILE_GetLightLayer();
	prnparams.light_pause = PFILE_GetLightPause();

	for (uint8_t i = 0; i < tguiScreenPrnparams.btns_count; i++)
	{
		switch (tguiScreenPrnparams.buttons[i].button_id)
		{
			case TG_SCR_PRNPARAMS_AA_ID:
				if (PFILE_GetAntialiasing() > 1)
					tguiScreenPrnparams.buttons[i].options.disabled = 0;
				else
					tguiScreenPrnparams.buttons[i].options.disabled = 1;
				break;

			case TG_SCR_PRNPARAMS_INDPARAMS_ID:
				if (PFILE_GetIndLayerSettings() > 0)
					tguiScreenPrnparams.buttons[i].options.disabled = 0;
				else
					tguiScreenPrnparams.buttons[i].options.disabled = 1;
				break;

		}
	}

	TGUI_ForceRepaint();
}
//==============================================================================



void		_tgui_PrnparamsScreenPaint(void *tguiobj, void *param)
{
	_tgui_DefaultScreenPaint(tguiobj, param);
}
//==============================================================================



void		_tgui_PrnparamsItemButtonPaint(void *tguiobj, void *param)
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
	
	if (pp_current_screen == 0)
	{
		switch (thisbtn->button_id)
		{
			case TG_SCR_PRNPARAMS_LIGHTBOTTOM_ID:
				sprintf(msg, (char*)"%0.1f", prnparams.bottom_light);
				meas = LANG_GetString(LSTR_SHORTSECOND);
				break;

			case TG_SCR_PRNPARAMS_LIGHT_ID:
				sprintf(msg, (char*)"%0.1f", prnparams.light);
				meas = LANG_GetString(LSTR_SHORTSECOND);
				break;

			case TG_SCR_PRNPARAMS_LIFTHEIGHT_ID:
				sprintf(msg, (char*)"%0.1f", prnparams.lift_height);
				meas = LANG_GetString(LSTR_SHORTMILLIMETERS);
				break;

			case TG_SCR_PRNPARAMS_LIFTSPEED_ID:
				sprintf(msg, (char*)"%0.1f", prnparams.lift_speed);
				meas = LANG_GetString(LSTR_MM_SEC);
				break;

			case TG_SCR_PRNPARAMS_DROPSPEED_ID:
				sprintf(msg, (char*)"%0.1f", prnparams.drop_speed);
				meas = LANG_GetString(LSTR_MM_SEC);
				break;

		}
	} else
	if (pp_current_screen == 1)
	{
		switch (thisbtn->button_id)
		{
			case TG_SCR_PRNPARAMS_LIGHTPAUSE_ID:
				sprintf(msg, (char*)"%0.1f", prnparams.light_pause);
				meas = LANG_GetString(LSTR_SHORTSECOND);
				break;

			case TG_SCR_PRNPARAMS_AA_ID:
				{
					if (prnparams.antialiasing)
						sprintf(msg, (char*)"x%u", PFILE_GetAntialiasing());
					else
						strcpy(msg, LANG_GetString(LSTR_OFF));
				}
				break;

			case TG_SCR_PRNPARAMS_INDPARAMS_ID:
				if (prnparams.ind_params)
					strcpy(msg, LANG_GetString(LSTR_ON));
				else
					strcpy(msg, LANG_GetString(LSTR_OFF));
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



void		_tgui_PrnparamsItemSolidButtonPaint(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	uint8_t			draw_btn = 0;
	
	if (pp_current_screen == 1)
	{
		switch (thisbtn->button_id)
		{
			case TG_SCR_PRNPARAMS_EXTPARAMS_ID:
				draw_btn = 1;
				break;
		}
	}
	if (draw_btn)
	{
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
			LCDUI_DrawText(txt, LCDUI_TEXT_ALIGN_CENTER, thisbtn->textposition.left, ty, thisbtn->textposition.right);
		}

		LCDUI_SetColor(oldcolor);
		LCDUI_SetBackColor(oldbackcolor);
		LCDUI_SetFont(oldfont);
	}

}
//==============================================================================



void		_tgui_PrnparamsDownButtonPress(void *tguiobj, void *param)
{
	if (pp_current_screen < PP_SCREENS_COUNT - 1)
	{
		pp_current_screen++;
		for (uint8_t i = 0; i < tguiScreenPrnparams.btns_count; i++)
		{
			switch (tguiScreenPrnparams.buttons[i].button_id)
			{
				case TG_SCR_PRNPARAMS_UP_ID:
					tguiScreenPrnparams.buttons[i].options.disabled = 0;
					break;

				case TG_SCR_PRNPARAMS_DOWN_ID:
					if (pp_current_screen < PP_SCREENS_COUNT - 1)
						tguiScreenPrnparams.buttons[i].options.disabled = 0;
					else
						tguiScreenPrnparams.buttons[i].options.disabled = 1;
					break;

				case TG_SCR_PRNPARAMS_LIGHTBOTTOM_ID:
				case TG_SCR_PRNPARAMS_LIGHT_ID:
				case TG_SCR_PRNPARAMS_LIFTHEIGHT_ID:
				case TG_SCR_PRNPARAMS_LIFTSPEED_ID:
				case TG_SCR_PRNPARAMS_DROPSPEED_ID:
					if (pp_current_screen == 0)
						tguiScreenPrnparams.buttons[i].options.disabled = 0;
					else
						tguiScreenPrnparams.buttons[i].options.disabled = 1;
					break;

				case TG_SCR_PRNPARAMS_LIGHTPAUSE_ID:
					if (pp_current_screen == 1)
						tguiScreenPrnparams.buttons[i].options.disabled = 0;
					else
						tguiScreenPrnparams.buttons[i].options.disabled = 1;
					break;

				case TG_SCR_PRNPARAMS_AA_ID:
					if (pp_current_screen == 1 && PFILE_GetAntialiasing() > 1)
						tguiScreenPrnparams.buttons[i].options.disabled = 0;
					else
						tguiScreenPrnparams.buttons[i].options.disabled = 1;
					break;

				case TG_SCR_PRNPARAMS_INDPARAMS_ID:
					if (pp_current_screen == 1 && PFILE_GetIndLayerSettings() > 0)
						tguiScreenPrnparams.buttons[i].options.disabled = 0;
					else
						tguiScreenPrnparams.buttons[i].options.disabled = 1;
					break;
			}
		}
		TGUI_ForceRepaint();
	}
}
//==============================================================================



void		_tgui_PrnparamsUpButtonPress(void *tguiobj, void *param)
{
	if (pp_current_screen > 0)
	{
		pp_current_screen--;
		for (uint8_t i = 0; i < tguiScreenPrnparams.btns_count; i++)
		{
			switch (tguiScreenPrnparams.buttons[i].button_id)
			{
				case TG_SCR_PRNPARAMS_UP_ID:
					if (pp_current_screen > 0)
						tguiScreenPrnparams.buttons[i].options.disabled = 0;
					else
						tguiScreenPrnparams.buttons[i].options.disabled = 1;
					break;

				case TG_SCR_PRNPARAMS_DOWN_ID:
					tguiScreenPrnparams.buttons[i].options.disabled = 0;
					break;

				case TG_SCR_PRNPARAMS_LIGHTBOTTOM_ID:
				case TG_SCR_PRNPARAMS_LIGHT_ID:
				case TG_SCR_PRNPARAMS_LIFTHEIGHT_ID:
				case TG_SCR_PRNPARAMS_LIFTSPEED_ID:
				case TG_SCR_PRNPARAMS_DROPSPEED_ID:
					if (pp_current_screen == 0)
						tguiScreenPrnparams.buttons[i].options.disabled = 0;
					else
						tguiScreenPrnparams.buttons[i].options.disabled = 1;
					break;

				case TG_SCR_PRNPARAMS_LIGHTPAUSE_ID:
					if (pp_current_screen == 1)
						tguiScreenPrnparams.buttons[i].options.disabled = 0;
					else
						tguiScreenPrnparams.buttons[i].options.disabled = 1;
					break;

				case TG_SCR_PRNPARAMS_AA_ID:
					if (pp_current_screen == 1 && PFILE_GetAntialiasing() > 1)
						tguiScreenPrnparams.buttons[i].options.disabled = 0;
					else
						tguiScreenPrnparams.buttons[i].options.disabled = 1;
					break;

				case TG_SCR_PRNPARAMS_INDPARAMS_ID:
					if (pp_current_screen == 1 && PFILE_GetIndLayerSettings() > 0)
						tguiScreenPrnparams.buttons[i].options.disabled = 0;
					else
						tguiScreenPrnparams.buttons[i].options.disabled = 1;
					break;
			}
		}
		TGUI_ForceRepaint();
	}
}
//==============================================================================



void		_tgui_PrnparamsItemButtonPress(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	thisbtn->options.pressed = 0;
	switch (thisbtn->button_id)
	{
		case TG_SCR_PRNPARAMS_LIGHTBOTTOM_ID:
			TGUI_NumenterScreenShow(LANG_GetString(LSTR_PP_LIGHT_BOTTOM), NT_UFLOAT, (void*)&(prnparams.bottom_light));
			break;

		case TG_SCR_PRNPARAMS_LIGHT_ID:
			TGUI_NumenterScreenShow(LANG_GetString(LSTR_PP_LIGHT), NT_UFLOAT, (void*)&(prnparams.light));
			break;

		case TG_SCR_PRNPARAMS_LIFTHEIGHT_ID:
			TGUI_NumenterScreenShow(LANG_GetString(LSTR_PP_LIFT_HEIGHT), NT_UFLOAT, (void*)&(prnparams.lift_height));
			break;

		case TG_SCR_PRNPARAMS_LIFTSPEED_ID:
			TGUI_NumenterScreenShow(LANG_GetString(LSTR_PP_LIFT_SPEED), NT_UFLOAT, (void*)&(prnparams.lift_speed));
			break;

		case TG_SCR_PRNPARAMS_DROPSPEED_ID:
			TGUI_NumenterScreenShow(LANG_GetString(LSTR_PP_DROP_SPEED), NT_UFLOAT, (void*)&(prnparams.drop_speed));
			break;

		case TG_SCR_PRNPARAMS_LIGHTPAUSE_ID:
			TGUI_NumenterScreenShow(LANG_GetString(LSTR_PP_LIGHT_PAUSE), NT_UFLOAT, (void*)&(prnparams.light_pause));
			break;

		case TG_SCR_PRNPARAMS_AA_ID:
			break;

		case TG_SCR_PRNPARAMS_INDPARAMS_ID:
			if (prnparams.ind_params == 1)
				prnparams.ind_params = 0;
			else
				prnparams.ind_params = 1;
			thisbtn->options.needrepaint = 1;
			break;

	}
}
//==============================================================================



void		_tgui_PrnparamsOkButtonPress(void *tguiobj, void *param)
{
	systemInfo.print_use_aa_grade = prnparams.antialiasing;
	systemInfo.print_use_ind_params = prnparams.ind_params;
	PFILE_SetLightBottom(prnparams.bottom_light);
	PFILE_SetDropSpeed(prnparams.drop_speed);
	PFILE_SetLiftHeight(prnparams.lift_height);
	PFILE_SetLiftSpeed(prnparams.lift_speed);
	PFILE_SetLightLayer(prnparams.light);
	PFILE_SetLightPause(prnparams.light_pause);
	
	tguiActiveScreen = (TG_SCREEN*)tguiActiveScreen->prevscreen;
	TGUI_ForceRepaint();
}
//==============================================================================



void		_tgui_PrnparamsSaveButtonPress(void *tguiobj, void *param)
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



