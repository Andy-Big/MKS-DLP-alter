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
#include "tgui_printscreenfuncs.h"


extern TG_SCREEN		*tguiActiveScreen;
extern char				msg[512];
extern __no_init FIL	ufile @ "CCMRAM";


char					fv_filename[256];
char					fv_dirname[256];
FILES_TYPE				fv_filetype;
TCHAR					fv_tfilename[512];
uint32_t				filesize = 0;
uint16_t				filedate = 0;
uint16_t				filetime = 0;



void		TGUI_FileviewScreenShow(char *fname, char *dname, FILES_TYPE ftype)
{
	memset(fv_tfilename, 0, sizeof(fv_tfilename));
	filesize = 0;
	filedate = 0;
	filetime = 0;
	
	strcpy(fv_filename, fname);
	strcpy(fv_dirname, dname);
	if (fv_dirname[strlen(fv_dirname)-1] != '/')
		strcat(fv_dirname, (char*)"/");
	fv_filetype = ftype;
	tguiScreenFileview.prevscreen = tguiActiveScreen;

	tstrcpy(fv_tfilename, UsbPath);
	tstrcat_utf(fv_tfilename, fv_dirname);
	tstrcat_utf(fv_tfilename, fv_filename);

	FILINFO finfo;
	memset(&finfo, 0, sizeof(FILINFO));
	if (f_stat(fv_tfilename, &finfo) != FR_OK)
		return;
	filesize = finfo.fsize;
	filedate = finfo.fdate;
	filetime = finfo.ftime;
	
	if (f_open(&ufile, fv_tfilename, FA_OPEN_EXISTING | FA_READ) != FR_OK)
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
	LCDUI_DrawText(fv_filename, LCDUI_TEXT_GETSIZE, thisbtn->position.left, thisbtn->position.top, thisbtn->position.right, -1, &namesize);
	uint16_t	ybot = thisbtn->position.top + (thisbtn->position.bottom - thisbtn->position.top) / 2;
	uint16_t	ytop = thisbtn->position.top + (ybot - thisbtn->position.top - namesize.y_size) / 2;
	LCDUI_DrawText(fv_filename, LCDUI_TEXT_ALIGN_CENTER, thisbtn->position.left, ytop, thisbtn->position.right, -1);
	
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

	if (f_open(&ufile, fv_tfilename, FA_OPEN_EXISTING | FA_READ) != FR_OK)
		return;

	switch (fv_filetype)
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


void		_tgui_FileviewPrintPress(void *tguiobj, void *param)
{
	if (PFILE_IsInited() == 0)
		return;

	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	
	TGUI_MessageBoxOkCancel(LANG_GetString(LSTR_WARNING), LANG_GetString(LSTR_MSG_SURE_NO_MODEL_ON_PLATFORM), TGUI_PrintScreenShow);
	
}
//==============================================================================




