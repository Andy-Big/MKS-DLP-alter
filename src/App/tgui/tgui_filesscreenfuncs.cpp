/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "tgui_filesscreenfuncs.h"
#include "tgui_defaultfuncs.h"
#include "usb_host.h"


extern uint8_t					UsbMounted;

extern __no_init uint8_t 		fbuff[8192] @ "CCMRAM";
extern __no_init FIL			ufile @ "CCMRAM";
extern __no_init FIL			sfile @ "CCMRAM";


uint8_t							oldUsbMounted;

uint8_t		lastfilenum = 255;
char		currdir[512] = {'/'};
FILES_ITEM	files[4];
uint8_t		subdir = 0;
uint8_t		nomorefiles = 0;


void		_tgui_FilesDrawPreview(TG_RECT *rect, FILES_ITEM *fitem)
{
	TG_SCREEN		*thisscr = &tguiScreenFiles;
	TCHAR			fname[512];
	uint32_t		rd = 0;
	uint16_t		pw = 0, ph = 0, iw = 0, ih = 0;
	uint8_t			pbpp = 16;			// preview bits per pixel
	float			pscale = 0;

	tstrcpy(fname, UsbPath);
	if (subdir > 0)
	{
		tstrcat_utf(fname, currdir);
		tstrcat_utf(fname, (char*)"/");
	}
	tstrcat_utf(fname, fitem->fname);

	if (f_open(&ufile, fname, FA_OPEN_EXISTING | FA_READ) != FR_OK)
		return;
	
	if (PFILE_Init(&ufile, fitem->type) == 0)
		goto closeexit;

	PFILE_DrawPreview(&ufile, rect);
	
closeexit:
	f_close(&ufile);
	return;
}
//==============================================================================




uint8_t		_tgui_FilesReadDir()
{
	TG_SCREEN		*thisscr = &tguiScreenFiles;
	TCHAR		dirname[512];
	DIR			dir;
	FILINFO		finfo;
	char		fname[256];
	uint8_t		fcount = 0;
	uint8_t		currfilenum = 0;
	FILES_TYPE	ftype;

	tstrcpy(dirname, UsbPath);
	tstrcat_utf(dirname, currdir);

	nomorefiles = 0;
	if (f_opendir(&dir, dirname) == FR_OK)
	{
		// read from begin
		if (lastfilenum == 255)
		{
			lastfilenum = 0;
		}
		
		fcount = 0;
		while(fcount < 4)
		{
			ftype = FTYPE_NONE;
			if (f_readdir(&dir, &finfo) == FR_OK && finfo.fname[0] != 0)
			{
				UnicodeToUTF8_Str(fname, finfo.fname, sizeof(fname));
				char *fext = FATFS_GetFileExtension(fname);
				if (finfo.fattrib & AM_DIR)
				{
					if (currfilenum >= lastfilenum)
						ftype = FTYPE_DIR;
					currfilenum++;
				}
				else if (strcmp(fext, (char*)"pws") == 0)
				{
					if (currfilenum >= lastfilenum)
						ftype = FTYPE_PWS;
					currfilenum++;
				}

				if (ftype != FTYPE_NONE)
				{
					if (currfilenum > lastfilenum)
					{
						files[fcount].type = ftype;
						strcpy(files[fcount].fname, fname);
						fcount++;
					}
				}
			}
			else
			{
				nomorefiles = 1;
				break;
			}
		}
		f_closedir(&dir);
		lastfilenum = currfilenum;
		
		uint8_t		fcnt = fcount;
		if (fcount > 0)
		{
			// if readed < 4 files
			while (fcnt < 4)
			{
				files[fcnt].type = FTYPE_NONE;
				fcnt++;
			}
		}
	}
	else
	{
		nomorefiles = 1;
	}
	for (uint8_t i = 0; i < thisscr->btns_count; i++)
	{
		if (thisscr->buttons[i].button_id == TG_SCR_FILES_UP_ID)
		{
			if (lastfilenum > 4)
				thisscr->buttons[i].options.disabled = 0;
			else
				thisscr->buttons[i].options.disabled = 1;
		}
		if (thisscr->buttons[i].button_id == TG_SCR_FILES_DOWN_ID)
		{
			if (nomorefiles)
				thisscr->buttons[i].options.disabled = 1;
			else
				thisscr->buttons[i].options.disabled = 0;
		}
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
			subdir = 0;
			lastfilenum = 255;
			currdir[0] = '/';
			currdir[1] = 0;
			uint32_t gg = sizeof(files);
			memset(&files, 0, gg);
		}
		// USB disk inserted
		else
		{
			_tgui_FilesReadDir();
		}
		// repaint buttons if this screen is active
		for (uint8_t i = 0; i < thisscr->btns_count; i++)
		{
			if (thisscr->buttons[i].button_id == TG_SCR_FILES_PREV_ID)
				thisscr->buttons[i].options.disabled = 1;
			if (tguiActiveScreen == thisscr)
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
	if (UsbMounted == 1 && lastfilenum == 255)
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
		if (files[thisbtn->button_id-1].type == FTYPE_PWS)
		{
			uint16_t	fh = LCDUI_GetCurrentFontHeight() * 2 + 2;
			TG_RECT		rc;
			rc.left = thisbtn->position.left + 10;
			rc.right = thisbtn->position.right - 10;
			rc.top = thisbtn->position.top + 10;
			rc.bottom = thisbtn->position.bottom - fh - 10;
			// preview paint
			_tgui_FilesDrawPreview(&rc, &files[thisbtn->button_id-1]);
		}
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
	if (subdir > 0)
		LCDUI_DrawText((char*)"/");


	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================




void		_tgui_FilesDownButtonPress(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	TG_SCREEN		*thisscr = (TG_SCREEN*)(thisbtn->parentscreen);
	// if no more files
	if (files[3].fname[0] == 0)
		return;
	
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




void		_tgui_FilesUpButtonPress(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	TG_SCREEN		*thisscr = (TG_SCREEN*)(thisbtn->parentscreen);
	// if no more files
	if (lastfilenum < 5)
		return;
	
	uint8_t		cpage = lastfilenum % 4;
	lastfilenum -= (4 + cpage);
	if (cpage == 0)
		lastfilenum -= 4;
	
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
	}
	
}
//==============================================================================




