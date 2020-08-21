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
	TGUI_MessageBoxWait(LANG_GetString(LSTR_WAIT), LANG_GetString(LSTR_SAVING_SETTINGS));
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
	TGUI_NumenterScreenShow(LANG_GetString(LSTR_LIFT_ON_PAUSE), NT_FLOAT, (void*)&(cfgConfig.pause_lift));
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



/*
void		_tgui_FilesUpButtonPress(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	TG_SCREEN		*thisscr = (TG_SCREEN*)(thisbtn->parentscreen);

	uint8_t		filesreq = 4;
	if (activefilelist == 1)
		filesreq = 8;

	// if no more files
	if (lastfilenum < filesreq+1)
		return;
	
	uint8_t		cpage = lastfilenum % filesreq;
	lastfilenum -= (filesreq + cpage);
	if (cpage == 0)
		lastfilenum -= filesreq;
	
	if (_tgui_FilesReadDir() > 0)
	{
		for (uint8_t i = 0; i < thisscr->btns_count; i++)
		{
			if (thisscr->buttons[i].button_id != 0 && thisscr->buttons[i].funcs._call_paint != NULL)
				thisscr->buttons[i].funcs._call_paint(&(thisscr->buttons[i]), 0);
			thisscr->buttons[i].options.needrepaint = 0;
		}
	}
}
//==============================================================================




void		_tgui_FilesBackButtonPress(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	lastfilenum = 0;
	_tgui_FilesReadDir();
	thisbtn->options.pressed = 0;
	tguiActiveScreen = (TG_SCREEN*)tguiActiveScreen->prevscreen;
	TGUI_ForceRepaint();
}
//==============================================================================




void		_tgui_FilesPrevButtonPress(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	TG_SCREEN		*thisscr = (TG_SCREEN*)(thisbtn->parentscreen);

	if (subdir == 0)
		return;
	
	uint8_t		pos = strlen(currdir);
	while (currdir[pos] != '/' && pos > 1)
	{
		pos--;
		if (currdir[pos] >= 0x80)
			pos--;
	}
	currdir[pos] = 0;
	
	memset(&files, 0, sizeof(files));
	lastfilenum = 0;
	subdir--;
	_tgui_FilesReadDir();
	for (uint8_t i = 0; i < thisscr->btns_count; i++)
	{
		if (thisscr->buttons[i].button_id == TG_SCR_FILES_PREV_ID && subdir == 0)
			thisscr->buttons[i].options.disabled = 1;
		if (thisscr->buttons[i].button_id != 0 && thisscr->buttons[i].funcs._call_paint != NULL)
			thisscr->buttons[i].funcs._call_paint(&(thisscr->buttons[i]), 0);
		thisscr->buttons[i].options.needrepaint = 0;
	}
	
}
//==============================================================================




void		_tgui_FilesListButtonPress(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;

	if (activefilelist == 0)
	{
		tguiActiveScreen = &tguiScreenFilesList;
		activefilelist = 1;
	}
	else
	{
		tguiActiveScreen = &tguiScreenFiles;
		activefilelist = 0;
	}
	
	lastfilenum = 0;
	_tgui_FilesReadDir();
	thisbtn->options.pressed = 0;
	TGUI_ForceRepaint();
	
}
//==============================================================================




void		_tgui_FilesFileButtonPress(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	TG_SCREEN		*thisscr = (TG_SCREEN*)(thisbtn->parentscreen);
	
	switch (files[thisbtn->button_id-1].type)
	{
		case FTYPE_DIR:
			if (subdir > 0)
			{
				strcat(currdir, (char*)"/");
			}
			strcat(currdir, files[thisbtn->button_id-1].fname);
			memset(&files, 0, sizeof(files));
			lastfilenum = 0;
			subdir++;
			_tgui_FilesReadDir();
			for (uint8_t i = 0; i < thisscr->btns_count; i++)
			{
				if (thisscr->buttons[i].button_id == TG_SCR_FILES_PREV_ID)
					thisscr->buttons[i].options.disabled = 0;
				if (thisscr->buttons[i].button_id != 0 && thisscr->buttons[i].funcs._call_paint != NULL)
					thisscr->buttons[i].funcs._call_paint(&(thisscr->buttons[i]), 0);
				thisscr->buttons[i].options.needrepaint = 0;
			}
			break;


		case FTYPE_CONFIG:
			UnicodeToUTF8_Str(cfgCFileName, UsbPath, sizeof(cfgCFileName));
			if (subdir > 0)
			{
				strcat(cfgCFileName, currdir);
				strcat(cfgCFileName, (char*)"/");
			}
			strcat(cfgCFileName, files[thisbtn->button_id-1].fname);
			TGUI_MessageBoxYesNo(LANG_GetString(LSTR_CONFIRM_ACT), LANG_GetString(LSTR_CFGFILE_LOAD_QUEST), CFG_LoadFromFile);
			break;
	}
	
}
//==============================================================================
*/




