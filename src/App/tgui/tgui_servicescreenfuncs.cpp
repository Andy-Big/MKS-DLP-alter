/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "tgui_servicescreenfuncs.h"
#include "tgui_defaultfuncs.h"
#include "config.h"


#define SRV_SCREENS_COUNT		2



uint8_t						srv_current_screen = 0;


void		_tgui_ServiceItemButtonPaint(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	
	if (thisbtn->options.disabled == 1)
		return;

	uint16_t oldcolor, newcolor = thisbtn->textcolor_en;
	uint16_t oldbackcolor, newbackcolor = thisbtn->backcolor_en;
	LCDUI_FONT_TYPE oldfont = LCDUI_SetFont(thisbtn->font);
	char *img = thisbtn->bgimagename_en;

	// colors
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

	// Name paint
	uint16_t	ty = LCDUI_GetCurrentFontHeight();
	ty = thisbtn->textposition.top + ((thisbtn->textposition.bottom - thisbtn->textposition.top) - ty) / 2;
	if (thisbtn->text != NULL)
	{
		char		*txt;
		// if "text" field is a pointer to memory
		if (thisbtn->text & 0xFF000000)
			txt = (char*)thisbtn->text;
		else
			txt = LANG_GetString(thisbtn->text);
		LCDUI_DrawText(txt, LCDUI_TEXT_ALIGN_CENTER | LCDUI_TEXT_TRANSBACK, thisbtn->textposition.left, ty, thisbtn->textposition.right);
	}

	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================



void		_tgui_ServiceDownButtonPress(void *tguiobj, void *param)
{
	if (srv_current_screen < SRV_SCREENS_COUNT - 1)
	{
		srv_current_screen++;
		for (uint8_t i = 0; i < tguiScreenService.btns_count; i++)
		{
			switch (tguiScreenService.buttons[i].button_id)
			{
				case TG_SCR_SERVICE_UP_ID:
					tguiScreenService.buttons[i].options.disabled = 0;
					break;

				case TG_SCR_SERVICE_DOWN_ID:
					if (srv_current_screen < SRV_SCREENS_COUNT - 1)
						tguiScreenService.buttons[i].options.disabled = 0;
					else
						tguiScreenService.buttons[i].options.disabled = 1;
					break;

				case TG_SCR_SERVICE_LANG_ID:
				case TG_SCR_SERVICE_SET_ID:
				case TG_SCR_SERVICE_ZMOVE_ID:
				case TG_SCR_SERVICE_UVTEST_ID:
					if (srv_current_screen == 0)
						tguiScreenService.buttons[i].options.disabled = 0;
					else
						tguiScreenService.buttons[i].options.disabled = 1;
					break;

				case TG_SCR_SERVICE_CALIB_ID:
					if (srv_current_screen == 1)
						tguiScreenService.buttons[i].options.disabled = 0;
					else
						tguiScreenService.buttons[i].options.disabled = 1;
					break;

			}
		}
		TGUI_ForceRepaint();
	}
}
//==============================================================================



void		_tgui_ServiceUpButtonPress(void *tguiobj, void *param)
{
	if (srv_current_screen > 0)
	{
		srv_current_screen--;
		for (uint8_t i = 0; i < tguiScreenService.btns_count; i++)
		{
			switch (tguiScreenService.buttons[i].button_id)
			{
				case TG_SCR_SERVICE_UP_ID:
					if (srv_current_screen > 0)
						tguiScreenService.buttons[i].options.disabled = 0;
					else
						tguiScreenService.buttons[i].options.disabled = 1;
					break;

				case TG_SCR_SERVICE_DOWN_ID:
					tguiScreenService.buttons[i].options.disabled = 0;
					break;

				case TG_SCR_SERVICE_LANG_ID:
				case TG_SCR_SERVICE_SET_ID:
				case TG_SCR_SERVICE_ZMOVE_ID:
				case TG_SCR_SERVICE_UVTEST_ID:
					if (srv_current_screen == 0)
						tguiScreenService.buttons[i].options.disabled = 0;
					else
						tguiScreenService.buttons[i].options.disabled = 1;
					break;

				case TG_SCR_SERVICE_CALIB_ID:
					if (srv_current_screen == 1)
						tguiScreenService.buttons[i].options.disabled = 0;
					else
						tguiScreenService.buttons[i].options.disabled = 1;
					break;

			}
		}
		TGUI_ForceRepaint();
	}
}
//==============================================================================



