/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "tgui_fileviewscreenfuncs.h"
#include "tgui_defaultfuncs.h"
#include "config.h"
#include "usb_host.h"
#include "ff.h"


extern TG_SCREEN		*tguiActiveScreen;
extern char				msg[512];
extern __no_init FIL	ufile @ "CCMRAM";


char					filename[256];
char					dirname[256];
FILES_TYPE				filetype;
TCHAR					tfilename[512];
uint32_t				filesize = 0;
uint16_t				filedate = 0;
uint16_t				filetime = 0;



void		TGUI_FileviewScreenShow(char *fname, char *dname, FILES_TYPE ftype)
{
	memset(tfilename, 0, sizeof(tfilename));
	filesize = 0;
	filedate = 0;
	filetime = 0;
	
	strcpy(filename, fname);
	strcpy(dirname, dname);
	if (dirname[strlen(dirname)-1] != '/')
		strcat(dirname, (char*)"/");
	filetype = ftype;
	tguiScreenFileview.prevscreen = tguiActiveScreen;

	tstrcpy(tfilename, UsbPath);
	tstrcat_utf(tfilename, dirname);
	tstrcat_utf(tfilename, filename);

	FILINFO finfo;
	memset(&finfo, 0, sizeof(FILINFO));
	if (f_stat(tfilename, &finfo) != FR_OK)
		return;
	filesize = finfo.fsize;
	filedate = finfo.fdate;
	filetime = finfo.ftime;
	
	if (f_open(&ufile, tfilename, FA_OPEN_EXISTING | FA_READ) != FR_OK)
		return;

	PFILE_Init(&ufile, ftype);

	f_close(&ufile);
}
//==============================================================================




void		_tgui_FileviewFileinfoPaint(void *tguiobj, void *param)
{
	if (PFILE_IsInited() == 0)
		return;

	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;

	LCDUI_FONT_TYPE oldfont = LCDUI_SetFont(LCDUI_FONT_H12BOLD);
	uint16_t 		oldcolor = LCDUI_SetColor(LCDUI_RGB(0x00496C));
	uint16_t 		oldbackcolor = LCDUI_SetBackColor(LCDUI_RGB(0xDDDDDD));

	// file name
	TSIZE		namesize = {0,0};
	LCDUI_DrawText(filename, LCDUI_TEXT_GETSIZE, thisbtn->position.left, thisbtn->position.top, thisbtn->position.right, -1, &namesize);
	uint16_t	ybot = thisbtn->position.top + (thisbtn->position.bottom - thisbtn->position.top) / 2;
	uint16_t	ytop = thisbtn->position.top + (ybot - thisbtn->position.top - namesize.y_size) / 2;
	LCDUI_DrawText(filename, LCDUI_TEXT_ALIGN_CENTER, thisbtn->position.left, ytop, thisbtn->position.right, -1);
	
	// file info
	uint16_t	fntheight = LCDUI_GetCurrentFontHeight();
	uint16_t	yinc = (thisbtn->position.bottom - ybot - fntheight * 2) / 3;	// 2 text lines and 3 intervals
	// size
	ytop = ybot + yinc;
	LCDUI_SetColor(LCDUI_RGB(0x000000));
	LCDUI_DrawText(LANG_GetString(LSTR_FILESIZE), 0, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	LCDUI_SetColor(LCDUI_RGB(0x00496C));
	sprintf(msg, "%0.2f %s", (float)filesize / 1000000, LANG_GetString(LSTR_MB));
	LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	// date-time
	ytop += fntheight + yinc;
	LCDUI_SetColor(LCDUI_RGB(0x000000));
	LCDUI_DrawText(LANG_GetString(LSTR_FILETIME), 0, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	LCDUI_SetColor(LCDUI_RGB(0x00496C));
	sprintf(msg, "%02u:%02u  %02u/%02u/%02u", filetime >> 11, (filetime >> 5) & 0x3F, (filedate) & 0x1F, (filedate >> 5) & 0x0F, (filedate >> 9) + 1980);
	LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	

	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================




void		_tgui_FileviewPrintinfoPaint(void *tguiobj, void *param)
{
	if (PFILE_IsInited() == 0)
		return;

	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;

	LCDUI_FONT_TYPE oldfont = LCDUI_SetFont(LCDUI_FONT_H12BOLD);
	uint16_t 		oldcolor = LCDUI_SetColor(LCDUI_RGB(0x00496C));
	uint16_t 		oldbackcolor = LCDUI_SetBackColor(LCDUI_RGB(0xDDDDDD));

	uint16_t	fntheight = LCDUI_GetCurrentFontHeight();
	uint16_t	yinc = (thisbtn->position.bottom - thisbtn->position.top - fntheight * 12) / 13;	// 12 text lines and 13 intervals
	uint16_t	ytop = thisbtn->position.top + yinc;
	// total layers
	LCDUI_SetColor(LCDUI_RGB(0x000000));
	LCDUI_DrawText(LANG_GetString(LSTR_TOTAL_LAYERS), 0, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	LCDUI_SetColor(LCDUI_RGB(0x00496C));
	sprintf(msg, "%u", PFILE_GetTotalLayers());
	LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	// print time
	ytop += fntheight + yinc;
	uint32_t	ptime = (uint32_t)PFILE_GetPrintTime();
	LCDUI_SetColor(LCDUI_RGB(0x000000));
	LCDUI_DrawText(LANG_GetString(LSTR_PRINT_TIME), 0, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	LCDUI_SetColor(LCDUI_RGB(0x00496C));
	sprintf(msg, "%02u:%02u", ptime / 3600, (ptime % 3600) / 60);
	LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	// layer thickness
	ytop += fntheight + yinc;
	LCDUI_SetColor(LCDUI_RGB(0x000000));
	LCDUI_DrawText(LANG_GetString(LSTR_LAYER_THICKNESS), 0, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	LCDUI_SetColor(LCDUI_RGB(0x00496C));
	sprintf(msg, "%0.3f %s", PFILE_GetLayerThickness(), LANG_GetString(LSTR_SHORTMILLIMETERS));
	LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	// antialiasing
	ytop += fntheight + yinc;
	LCDUI_SetColor(LCDUI_RGB(0x000000));
	LCDUI_DrawText(LANG_GetString(LSTR_ANTIALIASING), 0, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	LCDUI_SetColor(LCDUI_RGB(0x00496C));
	sprintf(msg, "%u", PFILE_GetAntialiasing());
	LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	// layer light
	ytop += fntheight + yinc;
	LCDUI_SetColor(LCDUI_RGB(0x000000));
	LCDUI_DrawText(LANG_GetString(LSTR_TIME_LIGHT), 0, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	LCDUI_SetColor(LCDUI_RGB(0x00496C));
	sprintf(msg, "%0.1f %s", PFILE_GetLightLayer(), LANG_GetString(LSTR_SHORTSECOND));
	LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	// bottom layer light
	ytop += fntheight + yinc;
	LCDUI_SetColor(LCDUI_RGB(0x000000));
	LCDUI_DrawText(LANG_GetString(LSTR_TIME_BOTTOM_LIGHT), 0, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	LCDUI_SetColor(LCDUI_RGB(0x00496C));
	sprintf(msg, "%0.1f %s", PFILE_GetLightBottom(), LANG_GetString(LSTR_SHORTSECOND));
	LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	// light pause
	ytop += fntheight + yinc;
	LCDUI_SetColor(LCDUI_RGB(0x000000));
	LCDUI_DrawText(LANG_GetString(LSTR_TIME_PAUSE_LIGHT), 0, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	LCDUI_SetColor(LCDUI_RGB(0x00496C));
	sprintf(msg, "%0.1f %s", PFILE_GetLightPause(), LANG_GetString(LSTR_SHORTSECOND));
	LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	// lift height
	ytop += fntheight + yinc;
	LCDUI_SetColor(LCDUI_RGB(0x000000));
	LCDUI_DrawText(LANG_GetString(LSTR_LIFT_HEIGHT), 0, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	LCDUI_SetColor(LCDUI_RGB(0x00496C));
	sprintf(msg, "%0.1f %s", PFILE_GetLiftHeight(), LANG_GetString(LSTR_SHORTMILLIMETERS));
	LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	// lift speed
	ytop += fntheight + yinc;
	LCDUI_SetColor(LCDUI_RGB(0x000000));
	LCDUI_DrawText(LANG_GetString(LSTR_LIFT_SPEED), 0, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	LCDUI_SetColor(LCDUI_RGB(0x00496C));
	sprintf(msg, "%0.1f %s", PFILE_GetLiftSpeed(), LANG_GetString(LSTR_MM_SEC));
	LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	// drop speed
	ytop += fntheight + yinc;
	LCDUI_SetColor(LCDUI_RGB(0x000000));
	LCDUI_DrawText(LANG_GetString(LSTR_DROP_SPEED), 0, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	LCDUI_SetColor(LCDUI_RGB(0x00496C));
	sprintf(msg, "%0.1f %s", PFILE_GetDropSpeed(), LANG_GetString(LSTR_MM_SEC));
	LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	// resin volume
	ytop += fntheight + yinc;
	LCDUI_SetColor(LCDUI_RGB(0x000000));
	LCDUI_DrawText(LANG_GetString(LSTR_RESIN_VOLUME), 0, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	LCDUI_SetColor(LCDUI_RGB(0x00496C));
	sprintf(msg, "%0.1f %s", PFILE_GetResinVolume(), LANG_GetString(LSTR_SHORTMILLILITERS));
	LCDUI_DrawText(msg, LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	// resin volume
	ytop += fntheight + yinc;
	LCDUI_SetColor(LCDUI_RGB(0x000000));
	LCDUI_DrawText(LANG_GetString(LSTR_IND_LAYERS_SETTINGS), 0, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	LCDUI_SetColor(LCDUI_RGB(0x00496C));
	if (PFILE_GetIndLayerSettings())
		LCDUI_DrawText(LANG_GetString(LSTR_YES), LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	else
		LCDUI_DrawText(LANG_GetString(LSTR_NO), LCDUI_TEXT_ALIGN_RIGHT, thisbtn->position.left + 5, ytop, thisbtn->position.right - 5, -1);
	

	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================




void		_tgui_FileviewPreviewPaint(void *tguiobj, void *param)
{
	if (PFILE_IsInited() == 0)
		return;

	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	
	uint16_t	fh = LCDUI_GetCurrentFontHeight() * 2 + 2;
	TG_RECT		rc;

	if (f_open(&ufile, tfilename, FA_OPEN_EXISTING | FA_READ) != FR_OK)
		return;

	switch (filetype)
	{
		// PWS preview
		case FTYPE_PWS:
			rc.left = thisbtn->position.left;
			rc.right = thisbtn->position.right;
			rc.top = thisbtn->position.top;
			rc.bottom = thisbtn->position.bottom;
			FPWS_DrawPreview(&ufile, &rc);
			break;
	}	

	f_close(&ufile);
}
//==============================================================================


/*
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



void		_tgui_NumenterOKPress(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;

	switch(typeval)
	{
		case NT_UINT:
			sscanf(numstr, "%u", ((uint32_t*)(edit_value)));
			break;
		case NT_INT:
			sscanf(numstr, "%d", ((int32_t*)(edit_value)));
			break;
		case NT_UFLOAT:
			sscanf(numstr, "%f", ((float*)(edit_value)));
			break;
		case NT_FLOAT:
			sscanf(numstr, "%f", ((float*)(edit_value)));
			break;
		default:
			break;
	}
	thisbtn->options.pressed = 0;
	tguiActiveScreen = (TG_SCREEN*)tguiActiveScreen->prevscreen;
	TGUI_ForceRepaint();
	
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
*/


