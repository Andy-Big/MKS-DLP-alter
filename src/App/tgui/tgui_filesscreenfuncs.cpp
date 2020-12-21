/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "tgui_filesscreenfuncs.h"
#include "tgui_fileviewscreenfuncs.h"
#include "tgui_defaultfuncs.h"
#include "usb_host.h"
#include "config.h"


extern uint8_t					UsbMounted;

extern __no_init uint8_t 		fbuff[8192] PLACE_TO_CCMRAM;
extern __no_init FIL			ufile PLACE_TO_CCMRAM;
extern __no_init FIL			sfile PLACE_TO_CCMRAM;

extern TG_SCREEN				*tguiActiveScreen;


uint8_t							oldUsbMounted;
uint8_t							activefilelist = 0;

uint8_t			lastfilenum = 255;
char			currdir[512] = {'/'};
FILES_ITEM		files[8];
uint8_t			subdir = 0;
uint8_t			nomorefiles = 0;


void		_tgui_FilesDrawPreview(TG_RECT *rect, FILES_ITEM *fitem)
{
	TCHAR			fname[512];

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

	PFILE_DrawPreview(&ufile, rect, 1);
	
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
	uint8_t		filesreq = 4;
	if (activefilelist == 1)
	{
		thisscr = &tguiScreenFilesList;
		filesreq = 8;
	}

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
		while(fcount < filesreq + 1)
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
				else if (strcmp(fext, (char*)"photon") == 0 || strcmp(fext, (char*)"cbddlp") == 0)
				{
					if (currfilenum >= lastfilenum)
						ftype = FTYPE_PHOTON;
					currfilenum++;
				}
				else if (strcmp(fext, (char*)"ctb") == 0)
				{
					if (currfilenum >= lastfilenum)
						ftype = FTYPE_CBDDLP;
					currfilenum++;
				}
				else if (strcmp(fext, (char*)"cimg") == 0)
				{
					if (currfilenum >= lastfilenum)
						ftype = FTYPE_IMAGE;
					currfilenum++;
				}
				else if (strcmp(fext, (char*)"acfg") == 0)
				{
					if (currfilenum >= lastfilenum)
						ftype = FTYPE_CONFIG;
					currfilenum++;
				}

				if (ftype != FTYPE_NONE)
				{
					if (currfilenum > lastfilenum)
					{
						if (fcount < filesreq)
						{
							files[fcount].type = ftype;
							files[fcount].date = finfo.fdate;
							files[fcount].time = finfo.ftime;
							strcpy(files[fcount].fname, fname);
						}
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
		if (nomorefiles == 0 && lastfilenum > 0)
			lastfilenum--;
		
		uint8_t		fcnt = fcount;
		if (fcount > 0)
		{
			// if readed < 'filesreq' files
			while (fcnt < filesreq)
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
			if (lastfilenum > filesreq)
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
		if (thisscr->buttons[i].button_id == TG_SCR_FILES_PREV_ID)
		{
			if (subdir == 0)
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
//	TG_SCREEN		*thisscr = (TG_SCREEN*)tguiobj;
	// USB disk removed/inserted after last read files
	if (UsbMounted == 1 && lastfilenum == 255)
	{
	}
	if (activefilelist == 1)
		tguiActiveScreen = &tguiScreenFilesList;
	else
		tguiActiveScreen = &tguiScreenFiles;
	_tgui_DefaultScreenPaint((void*)tguiActiveScreen, NULL);
}
//==============================================================================




void		_tgui_FilesFileButtonPaint(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	
	uint16_t			oldcolor = LCDUI_SetColor(thisbtn->textcolor_en);

	if (files[thisbtn->button_id-1].type == FTYPE_NONE)
	{
		LCDUI_SetColor(thisbtn->backcolor_dis);
		LCDUI_FillRect(thisbtn->position.left, thisbtn->position.top, thisbtn->position.right - thisbtn->position.left + 1, thisbtn->position.bottom - thisbtn->position.top + 1);
		LCDUI_SetColor(oldcolor);
		return;
	}

	uint16_t			oldbackcolor = LCDUI_SetBackColor(thisbtn->backcolor_en);
	LCDUI_FONT_TYPE 	oldfont = LCDUI_SetFont(thisbtn->font);

	LCDUI_SetColor(thisbtn->backcolor_en);
	LCDUI_FillRect(thisbtn->position.left, thisbtn->position.top, thisbtn->position.right - thisbtn->position.left, thisbtn->position.bottom - thisbtn->position.top);
	LCDUI_SetColor(thisbtn->textcolor_en);

	uint16_t	fh = LCDUI_GetCurrentFontHeight() * 2 + 2;
	TG_RECT		rc;
	rc.left = thisbtn->position.left + 5;
	rc.right = thisbtn->position.right - 5;
	rc.top = thisbtn->position.top + 5;
	rc.bottom = thisbtn->position.bottom - fh - 5;
	switch (files[thisbtn->button_id-1].type)
	{
		// directory icon
		case FTYPE_DIR:
			_tgui_DrawFileCimg(FNAME_ICN_FILES_DIRECTORY, thisbtn->position.left, thisbtn->position.top);
			break;
		// config icon
		case FTYPE_CONFIG:
			_tgui_DrawFileCimg(FNAME_ICN_FILES_CONFIG, thisbtn->position.left, thisbtn->position.top);
			break;
		// image icon
		case FTYPE_IMAGE:
			_tgui_DrawFileCimg(FNAME_ICN_FILES_IMAGE, thisbtn->position.left, thisbtn->position.top);
			break;
		// PWS, PHOTON preview
		case FTYPE_PWS:
		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			// preview paint
			_tgui_FilesDrawPreview(&rc, &files[thisbtn->button_id-1]);
			break;
	}	
	
	
	// filename paint
	LCDUI_DrawText(files[thisbtn->button_id-1].fname, LCDUI_TEXT_ALIGN_CENTER, thisbtn->textposition.left, thisbtn->textposition.top, thisbtn->textposition.right, thisbtn->textposition.bottom);


	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================




void		_tgui_FilesListFileButtonPaint(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	
	uint16_t		oldcolor = LCDUI_SetColor(thisbtn->textcolor_en);
	uint16_t		oldbackcolor = LCDUI_SetBackColor(thisbtn->backcolor_en);
	LCDUI_FONT_TYPE 	oldfont = LCDUI_SetFont(thisbtn->font);

	uint16_t		bcolor = thisbtn->backcolor_en, fcolor = thisbtn->textcolor_en;

	if (files[thisbtn->button_id-1].type == FTYPE_NONE)
	{
		LCDUI_SetColor(thisbtn->backcolor_dis);
		LCDUI_FillRect(thisbtn->position.left, thisbtn->position.top, thisbtn->position.right - thisbtn->position.left + 1, thisbtn->position.bottom - thisbtn->position.top + 1);
		LCDUI_SetColor(oldcolor);
		LCDUI_SetBackColor(oldbackcolor);
		LCDUI_SetFont(oldfont);
		return;
	}
	else
	{
		if (files[thisbtn->button_id-1].type == FTYPE_DIR)
		{
			bcolor = LCDUI_RGB(0xE0EA48);
		}
		else
		{
		}
	}
	if (thisbtn->options.pressed == 1)
	{
		bcolor = thisbtn->backcolor_press;
		fcolor = thisbtn->textcolor_press;
	}
	LCDUI_SetColor(bcolor);
	LCDUI_FillRect(thisbtn->position.left, thisbtn->position.top, thisbtn->position.right - thisbtn->position.left + 1, thisbtn->position.bottom - thisbtn->position.top + 1);

	LCDUI_SetColor(fcolor);
	LCDUI_SetBackColor(bcolor);

	uint16_t	xc = 2, wc = 200, yc = (30 - 18) / 2, tw = 0, ii = 0;
	char		*fext, fname[256];

	strcpy_utf(fname, files[thisbtn->button_id-1].fname);
	fext = FATFS_GetFileExtensionUTF(files[thisbtn->button_id-1].fname);
	if (*fext != 0)
		fext--;
	FATFS_DelFileExtensionUTF(fname);

	// file name
	ii = 0;
	tw = 0;
	wc = 182;
	xc = 2;
	while (fname[ii])
	{
		tw += _lcdui_GetCharWidth(UTF8toANSI((char*)(fname + ii)));
		if (tw > wc)
		{
			fname[ii] = 0;
			strcat(fname, (char*)"...");
			break;
		}
		if (fname[ii] < 0x80)
			ii++;
		else
			ii += 2;
	}
	LCDUI_DrawText(fname, 0, thisbtn->textposition.left + xc, thisbtn->textposition.top + yc, thisbtn->textposition.right, thisbtn->textposition.bottom);

	// file extension
	ii = 0;
	tw = 0;
	wc = 65;
	xc = 205;
	while (fext[ii])
	{
		tw += _lcdui_GetCharWidth(UTF8toANSI((char*)(fext + ii)));
		if (tw > wc)
		{
			fext[ii] = 0;
			break;
		}
		if (fext[ii] < 0x80)
			ii++;
		else
			ii += 2;
	}
	LCDUI_DrawText(fext, 0, thisbtn->textposition.left + xc, thisbtn->textposition.top + yc, thisbtn->textposition.right, thisbtn->textposition.bottom);

	// file date time
	xc = 278;

	char		dt[16];
	if (files[thisbtn->button_id-1].type == FTYPE_DIR)
	{
		strcpy(dt, (char*)"DIR ");
	}
	else
	{
		uint16_t	date = files[thisbtn->button_id-1].date;
		uint16_t	time = files[thisbtn->button_id-1].time;
		sprintf(dt, "%02u.%02u %02u:%02u", date & 0x1F, (date >> 5) & 0x0F, (time >> 11) & 0x1F, (time >> 5) & 0x3F);
	}
	LCDUI_DrawText(dt, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->textposition.left + xc, thisbtn->textposition.top + yc, thisbtn->textposition.right - 1, thisbtn->textposition.bottom);

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
	uint8_t		filesreq = 4;
	if (activefilelist == 1)
		filesreq = 8;

	// if no more files
	if (files[filesreq-1].fname[0] == 0)
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
			TGUI_MessageBoxYesNo(LANG_GetString(LSTR_CONFIRM_ACT), LANG_GetString(LSTR_MSG_CFGFILE_LOAD_QUEST), CFG_LoadFromFile);
			break;

		case FTYPE_PWS:
		case FTYPE_PHOTON:
		case FTYPE_CBDDLP:
			TGUI_FileviewScreenShow(files[thisbtn->button_id-1].fname, currdir, files[thisbtn->button_id-1].type);
			thisbtn->options.pressed = 0;
			tguiScreenFileview.prevscreen = tguiActiveScreen;
			tguiActiveScreen = &tguiScreenFileview;
			TGUI_ForceRepaint();
			break;
	}
	
}
//==============================================================================




