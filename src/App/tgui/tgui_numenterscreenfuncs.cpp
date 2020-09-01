/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "tgui_numenterscreenfuncs.h"
#include "tgui_defaultfuncs.h"
#include "config.h"


#define	NUMVALUE_MAXLEN		10



extern TG_SCREEN		*tguiActiveScreen;
extern char				msg[512];


char					numstr[32];
uint8_t					numstr_len = 0;
uint8_t					enter_begin = 0;
uint8_t					dot_entered = 0;



void		TGUI_NumenterScreenShow(char *name, NUMENTER_TYPEVAL type, void *value)
{
	enter_begin = 0;
	dot_entered = 0;
	tguiScreenNumenter.prevscreen = tguiActiveScreen;
	tguiScreenNumenter.name = (LNG_STRING_ID)((DWORD)name);
	switch(type)
	{
		case NT_UINT:
			sprintf(numstr, "%u", *((uint32_t*)(value)));
			for (uint8_t i = 0; i < tguiScreenNumenter.btns_count; i++)
			{
				if (tguiScreenNumenter.buttons[i].button_id == TG_SCR_NUMENTER_MINUS_ID || tguiScreenNumenter.buttons[i].button_id == TG_SCR_NUMENTER_DOT_ID)
					tguiScreenNumenter.buttons[i].options.disabled = 1;
				else
					tguiScreenNumenter.buttons[i].options.disabled = 0;
				if (tguiScreenNumenter.buttons[i].button_id == TG_SCR_NUMENTER_DIGISCREEN)
					tguiScreenNumenter.buttons[i].text = (LNG_STRING_ID)((DWORD)numstr);
			}
			break;
		case NT_INT:
			sprintf(numstr, "%d", *((int32_t*)(value)));
			for (uint8_t i = 0; i < tguiScreenNumenter.btns_count; i++)
			{
				if (tguiScreenNumenter.buttons[i].button_id == TG_SCR_NUMENTER_DOT_ID)
					tguiScreenNumenter.buttons[i].options.disabled = 1;
				else
					tguiScreenNumenter.buttons[i].options.disabled = 0;
				if (tguiScreenNumenter.buttons[i].button_id == TG_SCR_NUMENTER_DIGISCREEN)
					tguiScreenNumenter.buttons[i].text = (LNG_STRING_ID)((DWORD)numstr);
			}
			break;
		case NT_UFLOAT:
			sprintf(numstr, "%0.3f", *((float*)(value)));
			for (uint8_t i = 0; i < tguiScreenNumenter.btns_count; i++)
			{
				if (tguiScreenNumenter.buttons[i].button_id == TG_SCR_NUMENTER_MINUS_ID)
					tguiScreenNumenter.buttons[i].options.disabled = 1;
				else
					tguiScreenNumenter.buttons[i].options.disabled = 0;
				if (tguiScreenNumenter.buttons[i].button_id == TG_SCR_NUMENTER_DIGISCREEN)
					tguiScreenNumenter.buttons[i].text = (LNG_STRING_ID)((DWORD)numstr);
			}
			break;
		case NT_FLOAT:
			sprintf(numstr, "%0.3f", *((float*)(value)));
			dot_entered = 1;
			for (uint8_t i = 0; i < tguiScreenNumenter.btns_count; i++)
			{
				tguiScreenNumenter.buttons[i].options.disabled = 0;
				if (tguiScreenNumenter.buttons[i].button_id == TG_SCR_NUMENTER_DIGISCREEN)
					tguiScreenNumenter.buttons[i].text = (LNG_STRING_ID)((DWORD)numstr);
			}
			break;
		default:
			strcpy(numstr, (char*)"0");
	}
	
	numstr_len = strlen(numstr);
	tguiActiveScreen = &tguiScreenNumenter;
	TGUI_ForceRepaint();
}
//==============================================================================



void		_tgui_NumenterButtonPaint(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	
	if (thisbtn->button_id == 0 || thisbtn->button_id > 10)
		_tgui_DefaultButtonPaint(tguiobj, param);

	return;
}
//==============================================================================



void		_tgui_NumenterDigiScreenPaint(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	
	LCDUI_FONT_TYPE oldfont = LCDUI_SetFont(thisbtn->font);
	uint16_t 		oldcolor = LCDUI_SetColor(thisbtn->textcolor_en);
	uint16_t 		oldbackcolor = LCDUI_SetBackColor(thisbtn->backcolor_en);

	LCDUI_SetColor(thisbtn->backcolor_en);
	LCDUI_FillRect(thisbtn->textposition.left, thisbtn->textposition.top, thisbtn->textposition.right - thisbtn->textposition.left, thisbtn->textposition.bottom - thisbtn->textposition.top);
	LCDUI_SetColor(thisbtn->textcolor_en);

	uint16_t	ty = LCDUI_GetCurrentFontHeight();
	ty = thisbtn->textposition.top + ((thisbtn->textposition.bottom - thisbtn->textposition.top) - ty) / 2;
	// Name paint
	char *txt = (char*)thisbtn->text;
	LCDUI_DrawText(txt, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->textposition.left, ty, thisbtn->textposition.right);
	
	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================



void		_tgui_NumenterNumbersPress(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	if (enter_begin == 0)
	{
		numstr[0] = thisbtn->button_id + 47;
		numstr[1] = 0;
		numstr_len = 1;
		enter_begin = 1;
		dot_entered = 0;
	}
	else
	{
		if (numstr_len == 1 && numstr[0] == '0')
		{
			numstr[0] = thisbtn->button_id + 47;
		}
		else
		{
			if (numstr_len < NUMVALUE_MAXLEN)
			{
				numstr[numstr_len] = thisbtn->button_id + 47;
				numstr_len++;
				numstr[numstr_len] = 0;
			}
		}
	}
	for (uint8_t i = 0; i < tguiScreenNumenter.btns_count; i++)
	{
		if (tguiScreenNumenter.buttons[i].button_id == TG_SCR_NUMENTER_DIGISCREEN)
			tguiScreenNumenter.buttons[i].options.needrepaint = 1;
	}
}
//==============================================================================



void		_tgui_NumenterDelPress(void *tguiobj, void *param)
{
	if (numstr_len > 0)
	{
		enter_begin = 1;
		numstr_len--;
		if (numstr[numstr_len] == '.')
			dot_entered = 0;
		if (numstr_len == 0)
		{
			numstr[0] = '0';
			numstr_len = 1;
		}
		numstr[numstr_len] = 0;
		for (uint8_t i = 0; i < tguiScreenNumenter.btns_count; i++)
		{
			if (tguiScreenNumenter.buttons[i].button_id == TG_SCR_NUMENTER_DIGISCREEN)
				tguiScreenNumenter.buttons[i].options.needrepaint = 1;
		}
	}
}
//==============================================================================



void		_tgui_NumenterDotPress(void *tguiobj, void *param)
{
	if (enter_begin == 0)
	{
		strcpy(numstr, (char*)"0.");
		numstr_len = strlen(numstr);
		enter_begin = 1;
		dot_entered = 1;
	}
	else
	{
		if (numstr_len < NUMVALUE_MAXLEN && dot_entered == 0)
		{
			numstr[numstr_len] = '.';
			numstr_len++;
			numstr[numstr_len] = 0;
			dot_entered = 1;
		}
	}
	for (uint8_t i = 0; i < tguiScreenNumenter.btns_count; i++)
	{
		if (tguiScreenNumenter.buttons[i].button_id == TG_SCR_NUMENTER_DIGISCREEN)
			tguiScreenNumenter.buttons[i].options.needrepaint = 1;
	}
}
//==============================================================================



void		_tgui_NumenterACPress(void *tguiobj, void *param)
{
	strcpy(numstr, (char*)"0");
	numstr_len = strlen(numstr);
	enter_begin = 1;
	dot_entered = 0;
	for (uint8_t i = 0; i < tguiScreenNumenter.btns_count; i++)
	{
		if (tguiScreenNumenter.buttons[i].button_id == TG_SCR_NUMENTER_DIGISCREEN)
			tguiScreenNumenter.buttons[i].options.needrepaint = 1;
	}
}
//==============================================================================



