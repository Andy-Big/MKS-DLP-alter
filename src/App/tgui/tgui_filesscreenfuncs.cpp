/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "tgui_filesscreenfuncs.h"
#include "tgui_defaultfuncs.h"
#include "usb_host.h"


extern uint8_t					UsbMounted;
uint8_t							oldUsbMounted;

uint8_t		beginfilenum = 255;
char		currdir[512] = {'/'};
FILES_ITEM	files[4];


uint8_t		_tgui_FilesReadDir(char *filename)
{
	TCHAR		dirname[512];
	DIR			dir;
	FILINFO		finfo;
	char		fname[256];
	uint8_t		fcount = 0;

	tstrcpy(dirname, UsbPath);
	tstrcat_utf(dirname, currdir);
	tstrcpy_utf(finfo.fname, filename);
	if (f_opendir(&dir, dirname) == FR_OK)
	{
		beginfilenum = 0;
		while(fcount < 4)
		{
			if (f_readdir(&dir, &finfo) == FR_OK)
			{
				UnicodeToANSI_Str(fname, finfo.fname, sizeof(fname));
				char *fext = FATFS_GetFileExtension(fname);
				if (finfo.fattrib & AM_DIR)
				{
					files[fcount].type = FTYPE_DIR;
				}
				else if (strcmp(fext, (char*)"pws") == 0)
				{
					files[fcount].type = FTYPE_PWS;
				}

				if (files[fcount].type != FTYPE_NONE)
				{
					strcpy(files[fcount].fname, fname);
					fcount++;
				}
			}
			else
			{
				break;
			}
		}
		f_closedir(&dir);
	}
	
	return fcount;
}
//==============================================================================




void		_tgui_FilesScreenProcess(void *tguiobj, void *param)
{
	TG_SCREEN		*thisscr = (TG_SCREEN*)tguiobj;
	// USB disk state changed
	if (oldUsbMounted != UsbMounted)
	{
		// USB disk removed
		if (UsbMounted != 1)
		{
			beginfilenum = 255;
			strcpy(currdir, (char*)"/");
			uint32_t gg = sizeof(files);
			memset(&files, 0, gg);
		}
		// USB disk inserted
		else
		{
			_tgui_FilesReadDir((char*)"");
		}
		// repaint buttons if this screen is active
		if (tguiActiveScreen == thisscr)
		{
			for (uint8_t i = 0; i < thisscr->btns_count; i++)
			{
				if (thisscr->buttons[i].button_id != 0 && thisscr->buttons[i].funcs._call_paint != NULL)
					thisscr->buttons[i].funcs._call_paint(&(thisscr->buttons[i]), 0);
				thisscr->buttons[i].options.needrepaint = 0;
			}
		}
		oldUsbMounted = UsbMounted;
	}
	if (tguiActiveScreen == thisscr)
		_tgui_DefaultScreenProcess((void*)thisscr, NULL);
}
//==============================================================================




void		_tgui_FilesScreenPaint(void *tguiobj, void *param)
{
	TG_SCREEN		*thisscr = (TG_SCREEN*)tguiobj;
	// USB disk removed/inserted after last read files
	if (UsbMounted == 1 && beginfilenum == 255)
	{
	}
	_tgui_DefaultScreenPaint((void*)thisscr, NULL);
}
//==============================================================================




void		_tgui_FilesFileButtonPaint(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	
	uint16_t			oldcolor = LCDUI_SetColor(thisbtn->textcolor_en);

	if (files[thisbtn->button_id-1].type == FTYPE_NONE)
	{
		LCDUI_SetColor(thisbtn->backcolor_dis);
		LCDUI_FillRect(thisbtn->position.left, thisbtn->position.top, thisbtn->position.right - thisbtn->position.left, thisbtn->position.bottom - thisbtn->position.top);
		LCDUI_SetColor(oldcolor);
		return;
	}

	uint16_t			oldbackcolor = LCDUI_SetBackColor(thisbtn->backcolor_en);
	LCDUI_FONT_TYPE 	oldfont = LCDUI_SetFont(thisbtn->font);

	LCDUI_SetColor(thisbtn->backcolor_en);
	LCDUI_FillRect(thisbtn->position.left, thisbtn->position.top, thisbtn->position.right - thisbtn->position.left, thisbtn->position.bottom - thisbtn->position.top);
	LCDUI_SetColor(thisbtn->textcolor_en);

	// directory icon
	if (files[thisbtn->button_id-1].type == FTYPE_DIR)
	{
		_tgui_DrawFileCimg(FNAME_ICN_FILES_DIRECTORY, thisbtn->position.left, thisbtn->position.top);
	}
	else
	{
		// preview paint
	}	
	
	
	// filename paint
	LCDUI_DrawText(files[thisbtn->button_id-1].fname, LCDUI_TEXT_ALIGN_CENTER, thisbtn->textposition.left, thisbtn->textposition.top, thisbtn->textposition.right, thisbtn->textposition.bottom);


	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================




void		_tgui_FilesDirNamePaint(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	
	uint16_t			oldcolor = LCDUI_SetColor(thisbtn->textcolor_en);
	uint16_t			oldbackcolor = LCDUI_SetBackColor(thisbtn->backcolor_en);
	LCDUI_FONT_TYPE 	oldfont = LCDUI_SetFont(thisbtn->font);

	LCDUI_SetColor(thisbtn->backcolor_en);
	LCDUI_FillRect(thisbtn->position.left, thisbtn->position.top, thisbtn->position.right - thisbtn->position.left, thisbtn->position.bottom - thisbtn->position.top);
	LCDUI_SetColor(thisbtn->textcolor_en);

	// directory name paint
	LCDUI_DrawText(currdir, 0, thisbtn->textposition.left, thisbtn->textposition.top, thisbtn->textposition.right, thisbtn->textposition.bottom);


	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================




void		_tgui_FilesDownPress(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	TG_SCREEN		*thisscr = (TG_SCREEN*)(thisbtn->parentscreen);
	// if no more files
	if (files[3].fname[0] == 0)
		return;
	
	if (_tgui_FilesReadDir(files[3].fname) > 0)
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




