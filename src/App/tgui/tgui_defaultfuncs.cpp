/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "tgui_defaultfuncs.h"




void		TGUI_DefaultButtonPaint(void *tguiobj, void* param)
{
	TGUI_BUTTON		*btn = (TGUI_BUTTON*)tguiobj;
//	if (btn->options.repaintonpress == BGP_NONE)
//		return;

	
	uint16_t oldcolor = LCDUI_SetColor(btn->backcolor_en);
	uint16_t oldbackcolor = LCDUI_SetBackColor(btn->backcolor_en);
	LCDUI_FONT_TYPE oldfont = LCDUI_SetFont(btn->font);

	LCDUI_FillRoundRect(btn->position.left, btn->position.top, btn->position.right - btn->position.left, btn->position.bottom - btn->position.top, 10);
	LCDUI_FillRect(50, 50, 1, 2);
	
	if (btn->text != NULL)
	{
		LCDUI_SetColor(btn->textcolor_en);
		int16_t tx = btn->textposition.left;
		int16_t ty = btn->textposition.top;
		int16_t tw = 0;
		int16_t th = 0;
		if (btn->options.textalign_h == HTA_CENTER)
		{
			tw = LCDUI_GetTextWidthUTF(btn->text);
			tx += ((btn->textposition.right - tx) - tw) / 2;
		}
		else
		{
			if (btn->options.textalign_h == HTA_RIGHT)
			{
				tw = LCDUI_GetTextWidthUTF(btn->text);
				tx = (btn->textposition.right - tw);
			}
		}
		if (btn->options.textalign_v == VTA_CENTER)
		{
			th = LCDUI_GetCurrentFontHeight();
			ty += ((btn->textposition.bottom - ty) - th) / 2;
		}
		else
		{
			if (btn->options.textalign_h == VTA_BOTTOM)
			{
				th = LCDUI_GetTextWidthUTF(btn->text);
				ty = (btn->textposition.bottom - th) + th / 6;
			}
		}
		LCDUI_DrawTextUTF(btn->text, 0, tx, ty);
	}
	

	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================




