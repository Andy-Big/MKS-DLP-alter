/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "tgui.h"
#include "tgui_defaultfuncs.h"
#include "tgui_langscreenfuncs.h"
#include "tgui_infoscreenfuncs.h"
#include "tgui_mainscreenfuncs.h"
#include "tgui_servicescreenfuncs.h"
#include "tgui_filesscreenfuncs.h"
#include "tgui_movezscreenfuncs.h"
#include "tgui_messagebox.h"
#include "tgui_settingsscreenfuncs.h"
#include "tgui_numenterscreenfuncs.h"
#include "tgui_fileviewscreenfuncs.h"
#include "tgui_printscreenfuncs.h"
#include "tgui_clocksetscreenfuncs.h"
#include "tgui_screensaverfuncs.h"
#include "tgui_uvtestscreenfuncs.h"
#include "tgui_prnparamsscreenfuncs.h"
#include "tgui_calibscreenfuncs.h"


__no_init uint8_t 		tguiDBuff[UIDBUFF_SIZE];
__no_init uint8_t		tguiFBuff[UIFBUFF_SIZE];
__no_init FIL			tguiFile;
__no_init TCHAR			tfname[512] @ "CCMRAM";

TG_CONFIG		tguiDefaultConfig;

TG_MSGBOX		tguiMsgBox;

TG_BUTTON		tguiScrMainButtons[TG_BTN_CNT_SCREEN_MAIN];
TG_SCREEN		tguiScreenMain;

TG_BUTTON		tguiScrServiceButtons[TG_BTN_CNT_SCREEN_SERVICE];
TG_SCREEN		tguiScreenService;

TG_BUTTON		tguiScrLanguageButtons[TG_BTN_CNT_SCREEN_LANGUAGE];
TG_SCREEN		tguiScreenLanguage;

TG_BUTTON		tguiScrInfoButtons[TG_BTN_CNT_SCREEN_INFO];
TG_SCREEN		tguiScreenInfo;

TG_BUTTON		tguiScrMovezButtons[TG_BTN_CNT_SCREEN_MOVEZ];
TG_SCREEN		tguiScreenMovez;

TG_BUTTON		tguiScrFilesButtons[TG_BTN_CNT_SCREEN_FILES];
TG_SCREEN		tguiScreenFiles;
TG_BUTTON		tguiScrFilesListButtons[TG_BTN_CNT_SCREEN_FILESICONS];
TG_SCREEN		tguiScreenFilesList;

TG_BUTTON		tguiScrSettingsButtons[TG_BTN_CNT_SCREEN_SETTINGS];
TG_SCREEN		tguiScreenSettings;

TG_BUTTON		tguiScrNumenterButtons[TG_BTN_CNT_SCREEN_NUMENTER];
TG_SCREEN		tguiScreenNumenter;

TG_BUTTON		tguiScrFileviewButtons[TG_BTN_CNT_SCREEN_FILEVIEW];
TG_SCREEN		tguiScreenFileview;

TG_BUTTON		tguiScrPrintButtons[TG_BTN_CNT_SCREEN_PRINT];
TG_SCREEN		tguiScreenPrint;

TG_BUTTON		tguiScrClockSetButtons[TG_BTN_CNT_SCREEN_CLOCKSET];
TG_SCREEN		tguiScreenClockSet;

TG_BUTTON		tguiScreenSaverButtons[TG_BTN_CNT_SCREENSAVER];
TG_SCREEN		tguiScreenSaver;

TG_BUTTON		tguiScrUVTestButtons[TG_BTN_CNT_SCREEN_CLOCKSET];
TG_SCREEN		tguiScreenUVTest;

TG_BUTTON		tguiScrPrnparamsButtons[TG_BTN_CNT_SCREEN_PRNPARAMS];
TG_SCREEN		tguiScreenPrnparams;

TG_BUTTON		tguiScrCalibButtons[TG_BTN_CNT_SCREEN_CALIB];
TG_SCREEN		tguiScreenCalib;



TG_SCREEN		*tguiActiveScreen;




uint8_t		TGUI_PointInRect(TOUCH_POINT *pt, TG_RECT *rc)
{
	if (pt->xc >= rc->left && pt->xc <= rc->right)
	{
		if (pt->yc >= rc->top && pt->yc <= rc->bottom)
			return 1;
	}
	
	return 0;
}
//==============================================================================










void		TGUI_Init()
{
	TG_CONFIG		*tgc;
	TG_SCREEN		*tgs;
	TG_BUTTON		*tgb;
	
	
	uint8_t			bi = 0;
	uint8_t			id = 0;
	int16_t			by;
	
	
	tguiActiveScreen = &tguiScreenMain;
	
	// DEFAULT CONFIG
	tgc = &tguiDefaultConfig;
	memset((void*)tgc, 0, sizeof(TG_CONFIG));
	
	tgc->scrnametextcolor = LCDUI_RGB(0xBAC5D5);
	tgc->scrtextcolor = LCDUI_RGB(0x00272E);
	tgc->scrbackcolor = LCDUI_RGB(0x648E62);
	tgc->scrfont = LCDUI_FONT_H18;
	tgc->scrnamefont = LCDUI_FONT_H24BOLD;

	tgc->btntextcolor_en = LCDUI_RGB(0x00272E);
	tgc->btntextcolor_press = LCDUI_RGB(0x8080FF);
	tgc->btntextcolor_dis = LCDUI_RGB(0x404040);
	tgc->btntextcolor_act = LCDUI_RGB(0x8080FF);
	tgc->btnbackcolor_en = LCDUI_RGB(0xA6BFCB);
	tgc->btnbackcolor_press = LCDUI_RGB(0xA6BFCB);
	tgc->btnbackcolor_dis = LCDUI_RGB(0xCBCBCB);
	tgc->btnbackcolor_act = LCDUI_RGB(0xA6BFCB);
	tgc->btnfont = LCDUI_FONT_H24BOLD;
	
	tgc->mb_text_font = LCDUI_FONT_H18;
	tgc->mb_capt_font = LCDUI_FONT_H18BOLD;
	tgc->mb_text_color = LCDUI_RGB(0x000000);
	tgc->mb_box_backcolor = LCDUI_RGB(0xEEEEEE);
	tgc->mb_capt_textcolor = LCDUI_RGB(0xFFFFFF);
	tgc->mb_capt_backcolor = LCDUI_RGB(0x006BA7);
	
	_tgui_ScreenTimeInit();
	
	
	// -------------------- Messagebox elements -----------------------
{
	bi = 0;
	memset((void*)&tguiMsgBox, 0, sizeof(TG_MSGBOX));
	for (uint8_t ii = 0; ii < TG_BTN_CNT_MSGBOX; ii++)
	{
		tgb = &(tguiMsgBox.buttons[bi++]);

		tgb->button_id = ii + 1;

		tgb->text = LSTR____;
		
		tgb->position = {0, 0, 109, 35};
		tgb->textposition = {0, 0, 109, 35};

		tgb->font = tgc->btnfont;
		tgb->textcolor_en = tgc->btntextcolor_en;
		tgb->textcolor_press = tgc->btntextcolor_press;
		tgb->textcolor_dis = tgc->btntextcolor_dis;
		tgb->backcolor_en = tgc->btnbackcolor_en;
		tgb->backcolor_press = tgc->btnbackcolor_press;
		tgb->backcolor_dis = tgc->btnbackcolor_dis;
		
		tgb->options.disabled = 0;
		tgb->options.bgpaint = BGP_IMAGE;
		tgb->options.repaintonpress = 1;

		tgb->bgimagename_en = FNAME_BTN_MSGBOX_EN;
		tgb->bgimagename_press = FNAME_BTN_MSGBOX_PRESS;

		tgb->textoptions.textalign_h = HTA_CENTER;
		tgb->textoptions.textalign_v = VTA_CENTER;

		tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
		tgb->funcs._call_press = NULL;
		tgb->funcs._call_process = _tgui_MsgBoxButtonProcess;

		tgb->parentscreen = NULL;
		tgb->childscreen = NULL;
	}


	// MESSAGEBOX
	
	tguiMsgBox.type = MSGBOX_OK;
	tguiMsgBox.prevscreen = NULL;
	
	tguiMsgBox.btns_count = TG_BTN_CNT_MSGBOX;
	
	tguiMsgBox.caption_height = 26;
	
	tguiMsgBox.font_caption = tgc->mb_capt_font;
	tguiMsgBox.font_text = tgc->mb_text_font;
	tguiMsgBox.text_color = tgc->mb_text_color;
	tguiMsgBox.box_backcolor = tgc->mb_box_backcolor;
	tguiMsgBox.capt_textcolor = tgc->mb_capt_textcolor;
	tguiMsgBox.capt_backcolor = tgc->mb_capt_backcolor;

}



	// -------------------- Main Screen elements -----------------------
{
	bi = 0;
	// FILES button
	tgb = &(tguiScrMainButtons[bi++]);
	memset((void*)tgb, 0, sizeof(TG_BUTTON));
	
	tgb->position = {15, 100, 465, 165};

	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	tgb->bgimagename_dis = NULL;

	tgb->text = LSTR_PRINT;
	tgb->textposition = {115, 103, 450, 162};
	tgb->font = tgc->btnfont;
	tgb->textcolor_en = tgc->btntextcolor_en;
	tgb->textcolor_press = tgc->btntextcolor_press;
	tgb->textcolor_dis = tgc->btntextcolor_dis;
	tgb->backcolor_en = tgc->btnbackcolor_en;
	tgb->backcolor_press = tgc->btnbackcolor_press;
	tgb->backcolor_dis = tgc->btnbackcolor_dis;
	
	tgb->options.disabled = 0;
	tgb->options.bgpaint = BGP_NONE;
	tgb->options.repaintonpress = 1;
	
	tgb->textoptions.textalign_h = HTA_CENTER;
	tgb->textoptions.textalign_v = VTA_CENTER;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = (pressfunc)BTNA_GOCHILDSCR;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;

	tgb->parentscreen = &tguiScreenMain;
	tgb->childscreen = (void*)&tguiScreenFiles;

	// INFO button
	tgb = &(tguiScrMainButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrMainButtons[0]), sizeof(TG_BUTTON));
	
	tgb->position = {245, 185, 465, 250};

	tgb->text = LSTR_INFO;
	tgb->textposition = {320, 188, 460, 247};

	tgb->funcs._call_press = (pressfunc)BTNA_GOCHILDSCR;

	tgb->childscreen = (void*)&tguiScreenInfo;

	// SERVICE button
	tgb = &(tguiScrMainButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrMainButtons[0]), sizeof(TG_BUTTON));
	
	tgb->position = {15, 185, 235, 250};

	tgb->text = LSTR_SERVICE;
	tgb->textposition = {85, 188, 230, 247};

	tgb->funcs._call_press = (pressfunc)BTNA_GOCHILDSCR;

	tgb->childscreen = (void*)&tguiScreenService;

	// TIME text-button
	tgb = &(tguiScrMainButtons[bi++]);
	memset((void*)tgb, 0, sizeof(TG_BUTTON));
	
	tgb->button_id = TG_TIMEBUTTON_ID;
	
	tgb->position = {6, 4, 169, 29};

	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	tgb->bgimagename_dis = NULL;

	tgb->text = LSTR____;
	tgb->textposition = {6, 4, 169, 29};
	tgb->font = LCDUI_FONT_H24;
	tgb->textcolor_en = LCDUI_RGB(0xBAC5D5);
	tgb->textcolor_press = LCDUI_RGB(0xBAC5D5);
	tgb->textcolor_dis = LCDUI_RGB(0xBAC5D5);
	tgb->backcolor_en = LCDUI_RGB(0x0000);
	tgb->backcolor_press = LCDUI_RGB(0x0000);
	tgb->backcolor_dis = LCDUI_RGB(0x0000);
	
	tgb->options.disabled = 0;
	tgb->options.bgpaint = BGP_FILL;
	tgb->options.repaintonpress = 0;
	
	tgb->textoptions.textalign_h = HTA_CENTER;
	tgb->textoptions.textalign_v = VTA_CENTER;

	tgb->funcs._call_paint = _tgui_ScreenTimePaint;
	tgb->funcs._call_press = NULL;
	tgb->funcs._call_process = _tgui_ScreenTimeProcess;

	tgb->parentscreen = &tguiScreenMain;
	tgb->childscreen = NULL;

	
	
	// MAIN SCREEN
	tgs = &tguiScreenMain;
	memset((void*)tgs, 0, sizeof(TG_SCREEN));
	
	tgs->bgimagename = FNAME_BKGR_MAIN;
	tgs->prevscreen = NULL;

	tgs->name = LSTR_MAINMENU;
	tgs->nameposition = {205, 3, 475, 30};
	tgs->nameoptions.textalign_h = HTA_CENTER;
	tgs->nameoptions.textalign_v = VTA_CENTER;

	tgs->btns_count = TG_BTN_CNT_SCREEN_MAIN;
	tgs->buttons = tguiScrMainButtons;

	tgs->font = tgc->scrfont;
	tgs->namefont = tgc->scrnamefont;
	tgs->textcolor = tgc->scrtextcolor;
	tgs->nametextcolor = tgc->scrnametextcolor;
	tgs->backcolor = tgc->scrbackcolor;

	tgs->funcs._callpaint = _tgui_DefaultScreenPaint;
	tgs->funcs._process = _tgui_DefaultScreenProcess;
	
}



	// -------------------- Service Screen elements -----------------------
{
	bi = 0;
	// BACK button
	tgb = &(tguiScrServiceButtons[bi++]);
	memset((void*)tgb, 0, sizeof(TG_BUTTON));
	
	tgb->position = {4, 4, 167, 49};

	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	tgb->bgimagename_dis = NULL;

	tgb->text = LSTR_BACK;
	tgb->textposition = {54, 6, 165, 47};
	tgb->font = tgc->btnfont;
	tgb->textcolor_en = LCDUI_RGB(0x074B19);
	tgb->textcolor_press = tgc->btntextcolor_press;
	tgb->textcolor_dis = tgc->btntextcolor_dis;
	tgb->backcolor_en = tgc->btnbackcolor_en;
	tgb->backcolor_press = tgc->btnbackcolor_press;
	tgb->backcolor_dis = tgc->btnbackcolor_dis;
	
	tgb->options.disabled = 0;
	tgb->options.bgpaint = BGP_NONE;
	tgb->options.repaintonpress = 1;

	tgb->textoptions.textalign_h = HTA_CENTER;
	tgb->textoptions.textalign_v = VTA_CENTER;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = (pressfunc)BTNA_GOPREVSCR;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;

	tgb->parentscreen = &tguiScreenService;
	tgb->childscreen = NULL;

	// UP button
	tgb = &(tguiScrServiceButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrServiceButtons[0]), sizeof(TG_BUTTON));

	tgb->button_id = TG_SCR_SERVICE_UP_ID;
	tgb->text = LSTR____;

	tgb->position = {403, 58, 468, 117};

	tgb->bgimagename_en = FNAME_BTN_SERVICE_UP_EN;
	tgb->bgimagename_press = FNAME_BTN_SERVICE_UP_PRESS;
	tgb->bgimagename_dis = FNAME_BTN_SERVICE_UP_DIS;
	
	tgb->options.disabled = 1;
	tgb->options.bgpaint = BGP_IMAGE;
	tgb->options.repaintonpress = 1;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = _tgui_ServiceUpButtonPress;

	// DOWN button
	tgb = &(tguiScrServiceButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrServiceButtons[bi-2]), sizeof(TG_BUTTON));

	tgb->button_id = TG_SCR_SERVICE_DOWN_ID;

	tgb->position = {403, 250, 468, 309};

	tgb->bgimagename_en = FNAME_BTN_SERVICE_DN_EN;
	tgb->bgimagename_press = FNAME_BTN_SERVICE_DN_PRESS;
	tgb->bgimagename_dis = FNAME_BTN_SERVICE_DN_DIS;

	tgb->options.disabled = 0;

	tgb->funcs._call_press = _tgui_ServiceDownButtonPress;

	// LANGUAGE button
	tgb = &(tguiScrServiceButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrServiceButtons[bi-2]), sizeof(TG_BUTTON));

	tgb->button_id = TG_SCR_SERVICE_LANG_ID;

	tgb->position = {15, 58, 394, 117};
	tgb->textposition = {90, 63, 388, 112};

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->text = LSTR_LANGUAGE;

	tgb->bgimagename_en = FNAME_BTN_SERVICE_LANG_EN;
	tgb->bgimagename_press = FNAME_BTN_SERVICE_LANG_PRESS;
	tgb->bgimagename_dis = NULL;

	tgb->options.disabled = 0;

	tgb->options.bgpaint = BGP_IMAGE;
	tgb->options.repaintonpress = 1;

	tgb->funcs._call_paint = _tgui_ServiceItemButtonPaint;
	tgb->funcs._call_press = (pressfunc)BTNA_GOCHILDSCR;

	tgb->childscreen = &tguiScreenLanguage;

	// SETTINGS button
	tgb = &(tguiScrServiceButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrServiceButtons[bi-2]), sizeof(TG_BUTTON));

	tgb->button_id = TG_SCR_SERVICE_SET_ID;

	tgb->position = {15, 122, 394, 181};
	tgb->textposition = {90, 127, 388, 176};

	tgb->text = LSTR_SETTINGS_FULL;

	tgb->bgimagename_en = FNAME_BTN_SERVICE_SET_EN;
	tgb->bgimagename_press = FNAME_BTN_SERVICE_SET_PRESS;

	tgb->options.disabled = 0;

	tgb->childscreen = &tguiScreenSettings;
	
	// MOVEZ button
	tgb = &(tguiScrServiceButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrServiceButtons[bi-2]), sizeof(TG_BUTTON));

	tgb->button_id = TG_SCR_SERVICE_ZMOVE_ID;

	tgb->position = {15, 186, 394, 245};
	tgb->textposition = {90, 191, 388, 240};

	tgb->text = LSTR_MOVEZ;

	tgb->bgimagename_en = FNAME_BTN_SERVICE_ZMOVE_EN;
	tgb->bgimagename_press = FNAME_BTN_SERVICE_ZMOVE_PRESS;

	tgb->options.disabled = 0;

	tgb->childscreen = &tguiScreenMovez;
	
	// UVTEST button
	tgb = &(tguiScrServiceButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrServiceButtons[bi-2]), sizeof(TG_BUTTON));

	tgb->button_id = TG_SCR_SERVICE_UVTEST_ID;

	tgb->position = {15, 250, 394, 309};
	tgb->textposition = {90, 255, 388, 304};

	tgb->text = LSTR_UVTEST;

	tgb->bgimagename_en = FNAME_BTN_SERVICE_UVTEST_EN;
	tgb->bgimagename_press = FNAME_BTN_SERVICE_UVTEST_PRESS;

	tgb->options.disabled = 0;

	tgb->childscreen = &tguiScreenUVTest;
	
	// CALIB button
	tgb = &(tguiScrServiceButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrServiceButtons[bi-2]), sizeof(TG_BUTTON));

	tgb->button_id = TG_SCR_SERVICE_CALIB_ID;

	tgb->position = {15, 58, 394, 117};
	tgb->textposition = {90, 63, 388, 112};

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->text = LSTR_TOUCH_CALIBRATION;

	tgb->bgimagename_en = FNAME_BTN_SERVICE_CALIB_EN;
	tgb->bgimagename_press = FNAME_BTN_SERVICE_CALIB_PRESS;
	tgb->bgimagename_dis = NULL;

	tgb->options.disabled = 1;

	tgb->funcs._call_press = (pressfunc)BTNA_GOCHILDSCR;

	tgb->childscreen = &tguiScreenCalib;

	
	// SERVICE SCREEN
	tgs = &tguiScreenService;
	memset((void*)tgs, 0, sizeof(TG_SCREEN));
	
	tgs->bgimagename = FNAME_BKGR_EMPTY;
	tgs->prevscreen = &tguiScreenMain;

	tgs->name = LSTR_SERVICE;
	tgs->nameposition = {205, 3, 475, 30};
	tgs->nameoptions.textalign_h = HTA_CENTER;
	tgs->nameoptions.textalign_v = VTA_CENTER;

	tgs->btns_count = TG_BTN_CNT_SCREEN_SERVICE;
	tgs->buttons = tguiScrServiceButtons;

	tgs->font = tgc->scrfont;
	tgs->namefont = tgc->scrnamefont;
	tgs->textcolor = tgc->scrtextcolor;
	tgs->nametextcolor = tgc->scrnametextcolor;
	tgs->backcolor = tgc->scrbackcolor;

	tgs->funcs._callpaint = _tgui_DefaultScreenPaint;
	tgs->funcs._process = _tgui_DefaultScreenProcess;
	
}



	// -------------------- Language Screen elements -----------------------
{
	bi = 0;
	// BACK button
	tgb = &(tguiScrLanguageButtons[bi++]);
	memset((void*)tgb, 0, sizeof(TG_BUTTON));
	
	tgb->position = {4, 4, 167, 49};

	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	tgb->bgimagename_dis = NULL;

	tgb->text = LSTR_BACK;
	tgb->textposition = {54, 6, 165, 47};
	tgb->font = tgc->btnfont;
	tgb->textcolor_en = LCDUI_RGB(0x074B19);
	tgb->textcolor_press = tgc->btntextcolor_press;
	tgb->textcolor_dis = tgc->btntextcolor_dis;
	tgb->backcolor_en = tgc->btnbackcolor_en;
	tgb->backcolor_press = tgc->btnbackcolor_press;
	tgb->backcolor_dis = tgc->btnbackcolor_dis;
	
	tgb->options.disabled = 0;
	tgb->options.bgpaint = BGP_NONE;
	tgb->options.repaintonpress = 1;
	
	tgb->textoptions.textalign_h = HTA_CENTER;
	tgb->textoptions.textalign_v = VTA_CENTER;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = (pressfunc)BTNA_GOPREVSCR;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;

	tgb->parentscreen = &tguiScreenLanguage;
	tgb->childscreen = NULL;

	// LANGUAGE buttons
	char		*txt;
	for (id = 0; id < LANG_GetLanduagesCount(); id++)
	{
		tgb = &(tguiScrLanguageButtons[bi++]);
		memset((void*)tgb, 0, sizeof(TG_BUTTON));
		
		tgb->button_id = id;
		
		tgb->position = {(int16_t)25, (int16_t)(65 + (id * 50)), (int16_t)455, (int16_t)(110 + (id * 50))};

		tgb->bgimagename_en = NULL;
		tgb->bgimagename_press = NULL;
		tgb->bgimagename_dis = NULL;

		txt = LANG_GetLanguageName(id);
		tgb->text = (LNG_STRING_ID)((DWORD)txt);
		tgb->textposition = {(int16_t)55, (int16_t)(67 + (id * 50)), (int16_t)425, (int16_t)(108 + (id * 50))};
		tgb->font = tgc->btnfont;

		tgb->textcolor_en = tgc->btntextcolor_en;
		tgb->textcolor_press = tgc->btntextcolor_press;
		tgb->textcolor_dis = tgc->btntextcolor_dis;
		tgb->backcolor_en = tgc->btnbackcolor_en;
		tgb->backcolor_press = tgc->btnbackcolor_press;
		tgb->backcolor_dis = tgc->btnbackcolor_dis;
		
		tgb->options.disabled = 0;
		tgb->options.bgpaint = BGP_NONE;
		tgb->options.repaintonpress = 1;
		
		tgb->textoptions.textalign_h = HTA_CENTER;
		tgb->textoptions.textalign_v = VTA_CENTER;

		tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
		tgb->funcs._call_press = _tgui_LanguageButtonPress;		// must not be zero or a predefined value
		tgb->funcs._call_process = _tgui_DefaultButtonProcess;

		tgb->parentscreen = &tguiScreenLanguage;
		tgb->childscreen = NULL;
	}
	
	
	// LANGUAGE SCREEN
	tgs = &tguiScreenLanguage;
	memset((void*)tgs, 0, sizeof(TG_SCREEN));
	
	tgs->bgimagename = FNAME_BKGR_LANGUAGE;
	tgs->prevscreen = &tguiScreenService;

	tgs->name = LSTR_LANGUAGE;
	tgs->nameposition = {205, 3, 475, 30};
	tgs->nameoptions.textalign_h = HTA_CENTER;
	tgs->nameoptions.textalign_v = VTA_CENTER;

	tgs->btns_count = TG_BTN_CNT_SCREEN_LANGUAGE;
	tgs->buttons = tguiScrLanguageButtons;

	tgs->font = tgc->scrfont;
	tgs->namefont = tgc->scrnamefont;
	tgs->textcolor = tgc->scrtextcolor;
	tgs->nametextcolor = tgc->scrnametextcolor;
	tgs->backcolor = tgc->scrbackcolor;

	tgs->funcs._callpaint = _tgui_DefaultScreenPaint;
	tgs->funcs._process = _tgui_DefaultScreenProcess;
	
}



	// -------------------- Info Screen elements -----------------------
{
	bi = 0;
	// BACK button
	tgb = &(tguiScrInfoButtons[bi++]);
	memset((void*)tgb, 0, sizeof(TG_BUTTON));
	
	tgb->position = {4, 4, 167, 49};

	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	tgb->bgimagename_dis = NULL;

	tgb->text = LSTR_BACK;
	tgb->textposition = {54, 6, 165, 47};
	tgb->font = tgc->btnfont;
	tgb->textcolor_en = LCDUI_RGB(0x074B19);
	tgb->textcolor_press = tgc->btntextcolor_press;
	tgb->textcolor_dis = tgc->btntextcolor_dis;
	tgb->backcolor_en = tgc->btnbackcolor_en;
	tgb->backcolor_press = tgc->btnbackcolor_press;
	tgb->backcolor_dis = tgc->btnbackcolor_dis;
	
	tgb->options.disabled = 0;
	tgb->options.bgpaint = BGP_NONE;
	tgb->options.repaintonpress = 1;
	
	tgb->textoptions.textalign_h = HTA_CENTER;
	tgb->textoptions.textalign_v = VTA_CENTER;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = (pressfunc)BTNA_GOPREVSCR;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;

	tgb->parentscreen = &tguiScreenInfo;
	tgb->childscreen = NULL;

	
	// information button
	tgb = &(tguiScrInfoButtons[bi++]);
	memset((void*)tgb, 0, sizeof(TG_BUTTON));
	
	tgb->position = {12, 58, 465, 304};

	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	tgb->bgimagename_dis = NULL;

	tgb->text = LSTR____;
	tgb->textposition = {12, 58, 465, 304};
	tgb->font = LCDUI_FONT_H24;
	tgb->textcolor_en = LCDUI_RGB(0x00496C);
	tgb->textcolor_press = tgc->btntextcolor_press;
	tgb->textcolor_dis = tgc->btntextcolor_dis;
	tgb->backcolor_en = tgc->btnbackcolor_en;
	tgb->backcolor_press = tgc->btnbackcolor_press;
	tgb->backcolor_dis = tgc->btnbackcolor_dis;
	
	tgb->options.disabled = 0;
	tgb->options.bgpaint = BGP_NONE;
	tgb->options.repaintonpress = 0;
	
	tgb->funcs._call_paint = _tgui_InfoScreenInfoPaint;
	tgb->funcs._call_press = NULL;
	tgb->funcs._call_process = NULL;

	tgb->parentscreen = &tguiScreenInfo;
	tgb->childscreen = NULL;

	
	// INFO SCREEN
	tgs = &tguiScreenInfo;
	memset((void*)tgs, 0, sizeof(TG_SCREEN));
	
	tgs->bgimagename = FNAME_BKGR_INFO;
	tgs->prevscreen = &tguiScreenMain;

	tgs->name = LSTR_INFO;
	tgs->nameposition = {205, 3, 475, 30};
	tgs->nameoptions.textalign_h = HTA_CENTER;
	tgs->nameoptions.textalign_v = VTA_CENTER;

	tgs->btns_count = TG_BTN_CNT_SCREEN_INFO;
	tgs->buttons = tguiScrInfoButtons;

	tgs->font = tgc->scrfont;
	tgs->namefont = tgc->scrnamefont;
	tgs->textcolor = tgc->scrtextcolor;
	tgs->nametextcolor = tgc->scrnametextcolor;
	tgs->backcolor = tgc->scrbackcolor;

	tgs->funcs._callpaint = _tgui_DefaultScreenPaint;
	tgs->funcs._process = _tgui_DefaultScreenProcess;
	
}



	// -------------------- MoveZ Screen elements -----------------------
{
	bi = 0;
	// BACK button
	tgb = &(tguiScrMovezButtons[bi++]);
	memset((void*)tgb, 0, sizeof(TG_BUTTON));
	
	tgb->position = {4, 4, 167, 49};

	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	tgb->bgimagename_dis = NULL;

	tgb->text = LSTR_BACK;
	tgb->textposition = {54, 6, 165, 47};
	tgb->font = tgc->btnfont;
	tgb->textcolor_en = LCDUI_RGB(0x074B19);
	tgb->textcolor_press = tgc->btntextcolor_press;
	tgb->textcolor_dis = tgc->btntextcolor_dis;
	tgb->backcolor_en = tgc->btnbackcolor_en;
	tgb->backcolor_press = tgc->btnbackcolor_press;
	tgb->backcolor_dis = tgc->btnbackcolor_dis;
	
	tgb->options.disabled = 0;
	tgb->options.bgpaint = BGP_NONE;
	tgb->options.repaintonpress = 1;
	
	tgb->textoptions.textalign_h = HTA_CENTER;
	tgb->textoptions.textalign_v = VTA_CENTER;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = _tgui_MovezBackButtonPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;

	tgb->parentscreen = &tguiScreenMovez;
	tgb->childscreen = NULL;

	// 30mm button
	tgb = &(tguiScrMovezButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrMovezButtons[0]), sizeof(TG_BUTTON));
	
	tgb->group_id = TG_SCR_MOVEZ_STEP_GROUP_ID;
	tgb->button_id = TG_SCR_MOVEZ_30_ID;
	
	tgb->position = {11, 65, 99, 114};

	tgb->textcolor_en = tgc->btntextcolor_en;
	tgb->textcolor_act = LCDUI_RGB(0xC80000);

	tgb->text = LSTR_30MM;
	tgb->textposition = {11, 65, 99, 114};

	tgb->funcs._call_press = _tgui_MovezStepSelectButtonPress;

	tgb->childscreen = NULL;

	// 10mm button
	tgb = &(tguiScrMovezButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrMovezButtons[0]), sizeof(TG_BUTTON));
	
	tgb->group_id = TG_SCR_MOVEZ_STEP_GROUP_ID;
	tgb->button_id = TG_SCR_MOVEZ_10_ID;
	
	tgb->position = {106, 65, 190, 114};

	tgb->textcolor_en = tgc->btntextcolor_en;
	tgb->textcolor_act = LCDUI_RGB(0xC80000);

	tgb->text = LSTR_10MM;
	tgb->textposition = {106, 65, 190, 114};

	tgb->funcs._call_press = _tgui_MovezStepSelectButtonPress;

	tgb->childscreen = NULL;

	// 1mm button
	tgb = &(tguiScrMovezButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrMovezButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_MOVEZ_1_ID;

	tgb->position = {197, 65, 281, 114};

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->text = LSTR_1MM;
	tgb->textposition = {197, 65, 281, 114};

	tgb->funcs._call_press = _tgui_MovezStepSelectButtonPress;

	tgb->childscreen = NULL;

	// 0.2mm button
	tgb = &(tguiScrMovezButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrMovezButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_MOVEZ_02_ID;
	tgb->options.active = 1;

	tgb->position = {288, 65, 372, 114};

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->text = LSTR_02MM;
	tgb->textposition = {288, 65, 372, 114};

	tgb->funcs._call_press = _tgui_MovezStepSelectButtonPress;

	tgb->childscreen = NULL;

	// 0.05mm button
	tgb = &(tguiScrMovezButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrMovezButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_MOVEZ_005_ID;
	tgb->options.active = 0;

	tgb->position = {379, 65, 463, 114};

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->text = LSTR_005MM;
	tgb->textposition = {379, 65, 463, 114};

	tgb->funcs._call_press = _tgui_MovezStepSelectButtonPress;

	tgb->childscreen = NULL;

	// UP button
	tgb = &(tguiScrMovezButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrMovezButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->group_id = 0;
	tgb->position = {365, 125, 464, 204};

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->text = LSTR____;
	tgb->textposition = {365, 125, 464, 204};
	
	tgb->options.bgpaint = BGP_IMAGE;
	tgb->bgimagename_en = FNAME_BTN_MOVEZ_UP_EN;
	tgb->bgimagename_press = FNAME_BTN_MOVEZ_UP_PRESS;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = _tgui_MovezUpButtonPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	
	// DOWN button
	tgb = &(tguiScrMovezButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrMovezButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {365, 230, 464, 309};

	tgb->textposition = {365, 230, 464, 309};
	
	tgb->bgimagename_en = FNAME_BTN_MOVEZ_DN_EN;
	tgb->bgimagename_press = FNAME_BTN_MOVEZ_DN_PRESS;

	tgb->funcs._call_press = _tgui_MovezDownButtonPress;

	// HOME button
	tgb = &(tguiScrMovezButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrMovezButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {16, 125, 335, 169};

	tgb->button_id = TG_SCR_MOVEZ_HOME_ID;

	tgb->textcolor_en = tgc->btntextcolor_en;
	tgb->textcolor_act = tgc->btntextcolor_en;
	tgb->textposition = {75, 125, 335, 169};
	tgb->text = LSTR_MOVEHOME;

	tgb->options.bgpaint = BGP_IMAGE;
	tgb->bgimagename_en = FNAME_BTN_MOVEZ_HOMENRDY_EN;
	tgb->bgimagename_press = FNAME_BTN_MOVEZ_HOMENRDY_EN;
	tgb->bgimagename_act = FNAME_BTN_MOVEZ_HOMERDY_EN;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = _tgui_MovezHomeButtonPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	
	// SET Z=0 button
	tgb = &(tguiScrMovezButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrMovezButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {16, 177, 335, 221};
	
	tgb->options.disabled = 0;

	tgb->textcolor_en = tgc->btntextcolor_en;
	tgb->textcolor_dis = tgc->btntextcolor_dis;
	tgb->textposition = {75, 177, 335, 221};
	tgb->text = LSTR_SETZ0;

	tgb->options.bgpaint = BGP_NONE;
	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	tgb->bgimagename_act = NULL;

	tgb->funcs._call_press = _tgui_MovezSetZ0ButtonPress;

	// STOP button
	tgb = &(tguiScrMovezButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrMovezButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {16, 230, 335, 309};

	tgb->options.disabled = 0;

	tgb->textcolor_en = tgc->btntextcolor_en;
	tgb->textposition = {75, 230, 335, 309};
	tgb->text = LSTR_STOPMOVE;

	tgb->options.bgpaint = BGP_NONE;
	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	tgb->bgimagename_act = NULL;

	tgb->funcs._call_press = _tgui_MovezStopButtonPress;
	
	
	// MOVEZ SCREEN
	tgs = &tguiScreenMovez;
	memset((void*)tgs, 0, sizeof(TG_SCREEN));
	
	tgs->bgimagename = FNAME_BKGR_MOVEZ;
	tgs->prevscreen = &tguiScreenService;

	tgs->name = LSTR_MOVEZ;
	tgs->nameposition = {205, 3, 475, 30};
	tgs->nameoptions.textalign_h = HTA_CENTER;
	tgs->nameoptions.textalign_v = VTA_CENTER;

	tgs->btns_count = TG_BTN_CNT_SCREEN_MOVEZ;
	tgs->buttons = tguiScrMovezButtons;

	tgs->font = tgc->scrfont;
	tgs->namefont = tgc->scrnamefont;
	tgs->textcolor = tgc->scrtextcolor;
	tgs->nametextcolor = tgc->scrnametextcolor;
	tgs->backcolor = tgc->scrbackcolor;

	tgs->funcs._callpaint = _tgui_DefaultScreenPaint;
	tgs->funcs._process = _tgui_MovezScreenProcess;
	
}



	// -------------------- Files Screen elements -----------------------
{
	bi = 0;
	// BACK button
	tgb = &(tguiScrFilesButtons[bi++]);
	memset((void*)tgb, 0, sizeof(TG_BUTTON));
	
	tgb->position = {4, 4, 167, 49};

	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	tgb->bgimagename_dis = NULL;

	tgb->text = LSTR_BACK;
	tgb->textposition = {54, 6, 165, 47};
	tgb->font = tgc->btnfont;
	tgb->textcolor_en = LCDUI_RGB(0x074B19);
	tgb->textcolor_press = tgc->btntextcolor_press;
	tgb->textcolor_dis = tgc->btntextcolor_dis;
	tgb->backcolor_en = tgc->btnbackcolor_en;
	tgb->backcolor_press = tgc->btnbackcolor_press;
	tgb->backcolor_dis = tgc->btnbackcolor_dis;
	
	tgb->options.disabled = 0;
	tgb->options.bgpaint = BGP_NONE;
	tgb->options.repaintonpress = 1;
	
	tgb->textoptions.textalign_h = HTA_CENTER;
	tgb->textoptions.textalign_v = VTA_CENTER;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = _tgui_FilesBackButtonPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;

	tgb->parentscreen = &tguiScreenFiles;
	tgb->childscreen = NULL;

	// DIRECTORY text-button
	tgb = &(tguiScrFilesButtons[bi++]);
	memset((void*)tgb, 0, sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_FILES_DIR_ID;

	tgb->position = {8, 53, 385, 68};

	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	tgb->bgimagename_dis = NULL;

	tgb->text = LSTR_TESTSTRING;
	tgb->textposition = {9, 54, 384, 67};
	tgb->font = LCDUI_FONT_H12BOLD;
	tgb->textcolor_en = LCDUI_RGB(0x0000);
	tgb->backcolor_en = LCDUI_RGB(0xC7C7C7);
	
	tgb->options.disabled = 0;
	tgb->options.bgpaint = BGP_NONE;
	tgb->options.repaintonpress = 0;
	
	tgb->textoptions.textalign_h = HTA_LEFT;
	tgb->textoptions.textalign_v = VTA_CENTER;

	tgb->funcs._call_paint = _tgui_FilesDirNamePaint;
	tgb->funcs._call_press = NULL;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;

	tgb->parentscreen = &tguiScreenFiles;
	tgb->childscreen = NULL;

	id = 1;
	// FILE-1 buttons
	tgb = &(tguiScrFilesButtons[bi++]);
	memset((void*)tgb, 0, sizeof(TG_BUTTON));
	
	tgb->button_id = id++;
	
	tgb->position = {10, 74, 179, 183};

	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	tgb->bgimagename_dis = NULL;

	tgb->text = LSTR_TESTSTRING;
	tgb->textposition = {(int16_t)(tgb->position.left + 1), (int16_t)(tgb->position.bottom - 25), (int16_t)(tgb->position.right - 1), (int16_t)(tgb->position.bottom - 1)};
	tgb->font = LCDUI_FONT_H12BOLD;

	tgb->textcolor_en = LCDUI_RGB(0x000000);
	tgb->textcolor_press = LCDUI_RGB(0x000000);
	tgb->textcolor_dis = LCDUI_RGB(0x000000);
	tgb->backcolor_en = LCDUI_RGB(0xD7E9F2);
	tgb->backcolor_press = LCDUI_RGB(0xD7E9F2);
	tgb->backcolor_dis = LCDUI_RGB(0xA0A0A0);
	
	tgb->options.disabled = 0;
	tgb->options.bgpaint = BGP_NONE;
	tgb->options.repaintonpress = 0;
	
	tgb->textoptions.textalign_h = HTA_CENTER;
	tgb->textoptions.textalign_v = VTA_CENTER;

	tgb->funcs._call_paint = _tgui_FilesFileButtonPaint;
	tgb->funcs._call_press = _tgui_FilesFileButtonPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;

	tgb->parentscreen = &tguiScreenFiles;
	tgb->childscreen = &tguiScreenFileview;
	
	// FILE-2 buttons
	tgb = &(tguiScrFilesButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrFilesButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = id++;
	
	tgb->position = {198, 74, 367, 183};

	tgb->textposition = {(int16_t)(tgb->position.left + 1), (int16_t)(tgb->position.bottom - 25), (int16_t)(tgb->position.right - 1), (int16_t)(tgb->position.bottom - 1)};

	tgb->funcs._call_press = _tgui_FilesFileButtonPress;		// must not be zero or a predefined value
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;

	// FILE-3 buttons
	tgb = &(tguiScrFilesButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrFilesButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = id++;
	
	tgb->position = {10, 195, 179, 304};

	tgb->textposition = {(int16_t)(tgb->position.left + 1), (int16_t)(tgb->position.bottom - 25), (int16_t)(tgb->position.right - 1), (int16_t)(tgb->position.bottom - 1)};

	tgb->funcs._call_press = _tgui_FilesFileButtonPress;		// must not be zero or a predefined value
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;

	// FILE-4 buttons
	tgb = &(tguiScrFilesButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrFilesButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = id++;
	
	tgb->position = {198, 195, 367, 304};

	tgb->textposition = {(int16_t)(tgb->position.left + 1), (int16_t)(tgb->position.bottom - 25), (int16_t)(tgb->position.right - 1), (int16_t)(tgb->position.bottom - 1)};

	tgb->funcs._call_press = _tgui_FilesFileButtonPress;		// must not be zero or a predefined value
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;

	
		// LIST button
	tgb = &(tguiScrFilesButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrFilesButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_FILES_LIST_ID;
	tgb->position = {393, 43, 472, 107};

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->text = LSTR____;
	tgb->textposition = {393, 43, 472, 107};
	
	tgb->options.bgpaint = BGP_IMAGE;
	tgb->options.repaintonpress = 1;
	tgb->options.disabled = 0;

	tgb->bgimagename_en = FNAME_BTN_FILES_LIST_EN;
	tgb->bgimagename_press = FNAME_BTN_FILES_LIST_PRESS;
	tgb->bgimagename_dis = FNAME_BTN_FILES_LIST_DIS;
	
	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = _tgui_FilesListButtonPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	
		// PREV button
	tgb = &(tguiScrFilesButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrFilesButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_FILES_PREV_ID;
	tgb->position = {393, 111, 472, 175};

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->text = LSTR____;
	tgb->textposition = {393, 111, 472, 175};
	
	tgb->options.bgpaint = BGP_IMAGE;
	tgb->options.repaintonpress = 1;
	tgb->options.disabled = 1;

	tgb->bgimagename_en = FNAME_BTN_FILES_PREV_EN;
	tgb->bgimagename_press = FNAME_BTN_FILES_PREV_PRESS;
	tgb->bgimagename_dis = FNAME_BTN_FILES_PREV_DIS;
	
	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = _tgui_FilesPrevButtonPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	
		// UP button
	tgb = &(tguiScrFilesButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrFilesButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_FILES_UP_ID;
	tgb->position = {393, 179, 472, 243};

	tgb->textposition = {393, 179, 472, 243};
	
	tgb->options.disabled = 0;

	tgb->bgimagename_en = FNAME_BTN_FILES_UP_EN;
	tgb->bgimagename_press = FNAME_BTN_FILES_UP_PRESS;
	tgb->bgimagename_dis = FNAME_BTN_FILES_UP_DIS;
	
	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = _tgui_FilesUpButtonPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	
		// DOWN button
	tgb = &(tguiScrFilesButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrFilesButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_FILES_DOWN_ID;
	tgb->position = {393, 247, 472, 311};

	tgb->textposition = {393, 247, 472, 311};
	
	tgb->bgimagename_en = FNAME_BTN_FILES_DN_EN;
	tgb->bgimagename_press = FNAME_BTN_FILES_DN_PRESS;
	tgb->bgimagename_dis = FNAME_BTN_FILES_DN_DIS;
	

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = _tgui_FilesDownButtonPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	
	
	// FILES SCREEN
	tgs = &tguiScreenFiles;
	memset((void*)tgs, 0, sizeof(TG_SCREEN));
	
	tgs->bgimagename = FNAME_BKGR_FILES;
	tgs->prevscreen = &tguiScreenMain;

	tgs->name = LSTR_PRINT;
	tgs->nameposition = {205, 3, 475, 30};
	tgs->nameoptions.textalign_h = HTA_CENTER;
	tgs->nameoptions.textalign_v = VTA_CENTER;

	tgs->btns_count = TG_BTN_CNT_SCREEN_FILES;
	tgs->buttons = tguiScrFilesButtons;

	tgs->font = LCDUI_FONT_H12;
	tgs->namefont = tgc->scrnamefont;
	tgs->textcolor = tgc->scrtextcolor;
	tgs->nametextcolor = tgc->scrnametextcolor;
	tgs->backcolor = tgc->scrbackcolor;

	tgs->funcs._callpaint = _tgui_FilesScreenPaint;
	tgs->funcs._process = _tgui_FilesScreenProcess;
	
}



	// -------------------- FilesList Screen elements -----------------------
{
	bi = 0;
	// BACK button
	tgb = &(tguiScrFilesListButtons[bi++]);
	memset((void*)tgb, 0, sizeof(TG_BUTTON));
	
	tgb->position = {4, 4, 167, 49};

	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	tgb->bgimagename_dis = NULL;

	tgb->text = LSTR_BACK;
	tgb->textposition = {54, 6, 165, 47};
	tgb->font = tgc->btnfont;
	tgb->textcolor_en = LCDUI_RGB(0x074B19);
	tgb->textcolor_press = tgc->btntextcolor_press;
	tgb->textcolor_dis = tgc->btntextcolor_dis;
	tgb->backcolor_en = tgc->btnbackcolor_en;
	tgb->backcolor_press = tgc->btnbackcolor_press;
	tgb->backcolor_dis = tgc->btnbackcolor_dis;
	
	tgb->options.disabled = 0;
	tgb->options.bgpaint = BGP_NONE;
	tgb->options.repaintonpress = 1;
	
	tgb->textoptions.textalign_h = HTA_CENTER;
	tgb->textoptions.textalign_v = VTA_CENTER;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = _tgui_FilesBackButtonPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;

	tgb->parentscreen = &tguiScreenFilesList;
	tgb->childscreen = NULL;

	// DIRECTORY text-button
	tgb = &(tguiScrFilesListButtons[bi++]);
	memset((void*)tgb, 0, sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_FILES_DIR_ID;

	tgb->position = {8, 53, 385, 68};

	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	tgb->bgimagename_dis = NULL;

	tgb->text = LSTR_TESTSTRING;
	tgb->textposition = {9, 54, 384, 67};
	tgb->font = LCDUI_FONT_H12BOLD;
	tgb->textcolor_en = LCDUI_RGB(0x0000);
	tgb->backcolor_en = LCDUI_RGB(0xC7C7C7);
	
	tgb->options.disabled = 0;
	tgb->options.bgpaint = BGP_NONE;
	tgb->options.repaintonpress = 0;
	
	tgb->textoptions.textalign_h = HTA_LEFT;
	tgb->textoptions.textalign_v = VTA_CENTER;

	tgb->funcs._call_paint = _tgui_FilesDirNamePaint;
	tgb->funcs._call_press = NULL;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;

	tgb->parentscreen = &tguiScreenFilesList;
	tgb->childscreen = NULL;

	id = 1;
	// FILES buttons
	for (uint8_t i = 0; i < 8; i++)
	{
		tgb = &(tguiScrFilesListButtons[bi++]);
		memset((void*)tgb, 0, sizeof(TG_BUTTON));
		
		tgb->button_id = id++;
		
		tgb->position = {8, (int16_t)(71 + i * 30), 385, (int16_t)(101 + i * 30)};

		tgb->bgimagename_en = NULL;
		tgb->bgimagename_press = NULL;
		tgb->bgimagename_dis = NULL;

		tgb->text = LSTR_TESTSTRING;
		tgb->textposition = {8, (int16_t)(71 + i * 30), 385, (int16_t)(101 + i * 30)};
		tgb->font = LCDUI_FONT_H18;

		tgb->textcolor_en = LCDUI_RGB(0x000000);
		tgb->textcolor_press = LCDUI_RGB(0xFFFFFF);
		tgb->textcolor_dis = LCDUI_RGB(0x000000);
		tgb->backcolor_en = LCDUI_RGB(0xE0E0E0);
		tgb->backcolor_press = LCDUI_RGB(0x006BA7);
		tgb->backcolor_dis = LCDUI_RGB(0xE0E0E0);
		
		tgb->options.disabled = 0;
		tgb->options.bgpaint = BGP_FILL;
		tgb->options.repaintonpress = 1;
		
		tgb->textoptions.textalign_h = HTA_CENTER;
		tgb->textoptions.textalign_v = VTA_CENTER;

		tgb->funcs._call_paint = _tgui_FilesListFileButtonPaint;
		tgb->funcs._call_press = _tgui_FilesFileButtonPress;
		tgb->funcs._call_process = _tgui_DefaultButtonProcess;

		tgb->parentscreen = &tguiScreenFilesList;
		tgb->childscreen = &tguiScreenFileview;
	}
	
	
		// LIST button
	tgb = &(tguiScrFilesListButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrFilesListButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_FILES_LIST_ID;
	tgb->position = {393, 43, 472, 107};

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->text = LSTR____;
	tgb->textposition = {393, 43, 472, 107};
	
	tgb->options.bgpaint = BGP_IMAGE;
	tgb->options.repaintonpress = 1;
	tgb->options.disabled = 0;

	tgb->bgimagename_en = FNAME_BTN_FILES_ICONS_EN;
	tgb->bgimagename_press = FNAME_BTN_FILES_ICONS_PRESS;
	tgb->bgimagename_dis = FNAME_BTN_FILES_ICONS_DIS;
	
	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = _tgui_FilesListButtonPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	
		// PREV button
	tgb = &(tguiScrFilesListButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrFilesListButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_FILES_PREV_ID;
	tgb->position = {393, 111, 472, 175};

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->text = LSTR____;
	tgb->textposition = {393, 111, 472, 175};
	
	tgb->options.bgpaint = BGP_IMAGE;
	tgb->options.repaintonpress = 1;
	tgb->options.disabled = 1;

	tgb->bgimagename_en = FNAME_BTN_FILES_PREV_EN;
	tgb->bgimagename_press = FNAME_BTN_FILES_PREV_PRESS;
	tgb->bgimagename_dis = FNAME_BTN_FILES_PREV_DIS;
	
	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = _tgui_FilesPrevButtonPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	
		// UP button
	tgb = &(tguiScrFilesListButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrFilesListButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_FILES_UP_ID;
	tgb->position = {393, 179, 472, 243};

	tgb->textposition = {393, 179, 472, 243};
	
	tgb->options.disabled = 0;

	tgb->bgimagename_en = FNAME_BTN_FILES_UP_EN;
	tgb->bgimagename_press = FNAME_BTN_FILES_UP_PRESS;
	tgb->bgimagename_dis = FNAME_BTN_FILES_UP_DIS;
	
	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = _tgui_FilesUpButtonPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	
		// DOWN button
	tgb = &(tguiScrFilesListButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrFilesListButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_FILES_DOWN_ID;
	tgb->position = {393, 247, 472, 311};

	tgb->textposition = {393, 247, 472, 311};
	
	tgb->bgimagename_en = FNAME_BTN_FILES_DN_EN;
	tgb->bgimagename_press = FNAME_BTN_FILES_DN_PRESS;
	tgb->bgimagename_dis = FNAME_BTN_FILES_DN_DIS;
	

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = _tgui_FilesDownButtonPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	
	
	// FILES SCREEN
	tgs = &tguiScreenFilesList;
	memset((void*)tgs, 0, sizeof(TG_SCREEN));
	
	tgs->bgimagename = FNAME_BKGR_FILESLIST;
	tgs->prevscreen = &tguiScreenMain;

	tgs->name = LSTR_PRINT;
	tgs->nameposition = {205, 3, 475, 30};
	tgs->nameoptions.textalign_h = HTA_CENTER;
	tgs->nameoptions.textalign_v = VTA_CENTER;

	tgs->btns_count = TG_BTN_CNT_SCREEN_FILESICONS;
	tgs->buttons = tguiScrFilesListButtons;

	tgs->font = LCDUI_FONT_H12;
	tgs->namefont = tgc->scrnamefont;
	tgs->textcolor = tgc->scrtextcolor;
	tgs->nametextcolor = tgc->scrnametextcolor;
	tgs->backcolor = tgc->scrbackcolor;

	tgs->funcs._callpaint = _tgui_FilesScreenPaint;
	tgs->funcs._process = _tgui_FilesScreenProcess;
	
}



	// -------------------- Settings Screen elements -----------------------
	id = 1;
{
	bi = 0;
	// BACK button
	tgb = &(tguiScrSettingsButtons[bi++]);
	memset((void*)tgb, 0, sizeof(TG_BUTTON));
	
	tgb->position = {4, 4, 167, 49};

	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	tgb->bgimagename_dis = NULL;

	tgb->text = LSTR_BACK;
	tgb->textposition = {54, 6, 165, 47};
	tgb->font = tgc->btnfont;
	tgb->textcolor_en = LCDUI_RGB(0x074B19);
	tgb->textcolor_press = tgc->btntextcolor_press;
	tgb->textcolor_dis = tgc->btntextcolor_dis;
	tgb->backcolor_en = tgc->btnbackcolor_en;
	tgb->backcolor_press = tgc->btnbackcolor_press;
	tgb->backcolor_dis = tgc->btnbackcolor_dis;
	
	tgb->options.disabled = 0;
	tgb->options.bgpaint = BGP_NONE;
	tgb->options.repaintonpress = 1;
	
	tgb->textoptions.textalign_h = HTA_CENTER;
	tgb->textoptions.textalign_v = VTA_CENTER;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = (pressfunc)BTNA_GOPREVSCR;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;

	tgb->parentscreen = &tguiScreenSettings;
	tgb->childscreen = NULL;

	// UP button
	tgb = &(tguiScrSettingsButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrSettingsButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->group_id = 0;
	tgb->position = {393, 56, 472, 120};

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->text = LSTR____;
	tgb->textposition = {393, 56, 472, 120};
	
	tgb->options.bgpaint = BGP_IMAGE;
	tgb->bgimagename_en = FNAME_BTN_SETTINGS_UP_EN;
	tgb->bgimagename_press = FNAME_BTN_SETTINGS_UP_PRESS;
	tgb->bgimagename_dis = FNAME_BTN_SETTINGS_UP_DIS;
	
	tgb->options.disabled = 1;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = NULL;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	
	// DOWN button
	tgb = &(tguiScrSettingsButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrSettingsButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {393, 247, 472, 311};

	tgb->textposition = {393, 247, 472, 311};
	
	tgb->bgimagename_en = FNAME_BTN_SETTINGS_DN_EN;
	tgb->bgimagename_press = FNAME_BTN_SETTINGS_DN_PRESS;
	tgb->bgimagename_dis = FNAME_BTN_SETTINGS_DN_DIS;

	tgb->funcs._call_press = NULL;

	// SAVE button
	tgb = &(tguiScrSettingsButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrSettingsButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {393, 56, 472, 120};

	tgb->textposition = {393, 56, 472, 120};
	
	tgb->bgimagename_en = FNAME_BTN_SETTINGS_SAVE_EN;
	tgb->bgimagename_press = FNAME_BTN_SETTINGS_SAVE_PRESS;

	tgb->options.disabled = 0;

	tgb->funcs._call_press = _tgui_SettingsSaveButtonPress;

	// Clock
	by = 51;
	tgb = &(tguiScrSettingsButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrSettingsButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {8, 57, 387, 104};

	tgb->button_id = TG_SCR_SETTINGS_CLOCK_ID;

	tgb->font = LCDUI_FONT_H24;

	tgb->textcolor_en = tgc->btntextcolor_en;
	tgb->textcolor_act = tgc->btntextcolor_en;
	tgb->textposition = {13, 57, 382, 104};
	tgb->text = LSTR_CLOCK;

	tgb->options.bgpaint = BGP_IMAGE;
	tgb->bgimagename_en = FNAME_BTN_SETTINGS_ITEM1_EN;
	tgb->bgimagename_press = FNAME_BTN_SETTINGS_ITEM1_PRESS;

	tgb->options.disabled = 0;

	tgb->funcs._call_paint = _tgui_SettingsItem1ButtonPaint;
	tgb->funcs._call_press = _tgui_SettingsClockButtonPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	
	
	// Lift on pause
	tgb = &(tguiScrSettingsButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrSettingsButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {8, (int16_t)(tguiScrSettingsButtons[bi-2].position.top + by), 387, (int16_t)(tguiScrSettingsButtons[bi-2].position.bottom + by)};

	tgb->button_id = TG_SCR_SETTINGS_LIFTPAUSE_ID;

	tgb->textcolor_en = tgc->btntextcolor_en;
	tgb->textcolor_act = tgc->btntextcolor_en;
	tgb->textposition = {13, (int16_t)(tguiScrSettingsButtons[bi-2].position.top + by), 382, (int16_t)(tguiScrSettingsButtons[bi-2].position.bottom + by)};
	tgb->text = LSTR_LIFT_ON_PAUSE;

	tgb->options.bgpaint = BGP_IMAGE;
	tgb->bgimagename_en = FNAME_BTN_SETTINGS_ITEM_EN;
	tgb->bgimagename_press = FNAME_BTN_SETTINGS_ITEM_PRESS;

	tgb->options.disabled = 0;

	tgb->funcs._call_paint = _tgui_SettingsItemButtonPaint;
	tgb->funcs._call_press = _tgui_SettingsPauseliftButtonPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	
	
	// Lift on end
	tgb = &(tguiScrSettingsButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrSettingsButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {8, (int16_t)(tguiScrSettingsButtons[bi-2].position.top + by), 387, (int16_t)(tguiScrSettingsButtons[bi-2].position.bottom + by)};

	tgb->button_id = TG_SCR_SETTINGS_LIFTEND_ID;

	tgb->textcolor_en = tgc->btntextcolor_en;
	tgb->textcolor_act = tgc->btntextcolor_en;
	tgb->textposition = {13, (int16_t)(tguiScrSettingsButtons[bi-2].position.top + by), 382, (int16_t)(tguiScrSettingsButtons[bi-2].position.bottom + by)};
	tgb->text = LSTR_LIFT_ON_COMPLETION;

	tgb->options.bgpaint = BGP_IMAGE;
	tgb->bgimagename_en = FNAME_BTN_SETTINGS_ITEM_EN;
	tgb->bgimagename_press = FNAME_BTN_SETTINGS_ITEM_PRESS;

	tgb->options.disabled = 0;

	tgb->funcs._call_paint = _tgui_SettingsItemButtonPaint;
	tgb->funcs._call_press = _tgui_SettingsEndliftButtonPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	
	
	// Buzzer
	tgb = &(tguiScrSettingsButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrSettingsButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {8, (int16_t)(tguiScrSettingsButtons[bi-2].position.top + by), 387, (int16_t)(tguiScrSettingsButtons[bi-2].position.bottom + by)};

	tgb->button_id = TG_SCR_SETTINGS_BUZZER_ID;

	tgb->textcolor_en = tgc->btntextcolor_en;
	tgb->textcolor_act = tgc->btntextcolor_en;
	tgb->textposition = {13, (int16_t)(tguiScrSettingsButtons[bi-2].position.top + by), 382, (int16_t)(tguiScrSettingsButtons[bi-2].position.bottom + by)};
	tgb->text = LSTR_BUZZER;

	tgb->options.bgpaint = BGP_IMAGE;
	tgb->bgimagename_en = FNAME_BTN_SETTINGS_ITEM_EN;
	tgb->bgimagename_press = FNAME_BTN_SETTINGS_ITEM_PRESS;


	tgb->funcs._call_paint = _tgui_SettingsItemButtonPaint;
	tgb->funcs._call_press = _tgui_SettingsBuzzerButtonPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	
	
	// SETTINGS SCREEN
	tgs = &tguiScreenSettings;
	memset((void*)tgs, 0, sizeof(TG_SCREEN));
	
	tgs->bgimagename = FNAME_BKGR_EMPTY;
	tgs->prevscreen = &tguiScreenService;

	tgs->name = LSTR_SETTINGS_FULL;
	tgs->nameposition = {205, 3, 475, 30};
	tgs->nameoptions.textalign_h = HTA_CENTER;
	tgs->nameoptions.textalign_v = VTA_CENTER;

	tgs->btns_count = TG_BTN_CNT_SCREEN_SETTINGS;
	tgs->buttons = tguiScrSettingsButtons;

	tgs->font = tgc->scrfont;
	tgs->namefont = tgc->scrnamefont;
	tgs->textcolor = tgc->scrtextcolor;
	tgs->nametextcolor = tgc->scrnametextcolor;
	tgs->backcolor = tgc->scrbackcolor;

	tgs->funcs._callpaint = _tgui_DefaultScreenPaint;
	tgs->funcs._process = _tgui_DefaultScreenProcess;
	
}



	// -------------------- Numenter Screen elements -----------------------
	id = 1;
{
	bi = 0;
	
	// BACK button
	tgb = &(tguiScrNumenterButtons[bi++]);
	memset((void*)tgb, 0, sizeof(TG_BUTTON));
	
	tgb->position = {4, 4, 167, 49};

	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	tgb->bgimagename_dis = NULL;

	tgb->text = LSTR_BACK;
	tgb->textposition = {54, 6, 165, 47};
	tgb->font = tgc->btnfont;
	tgb->textcolor_en = LCDUI_RGB(0x074B19);
	tgb->textcolor_press = tgc->btntextcolor_press;
	tgb->textcolor_dis = tgc->btntextcolor_dis;
	tgb->backcolor_en = tgc->btnbackcolor_en;
	tgb->backcolor_press = tgc->btnbackcolor_press;
	tgb->backcolor_dis = tgc->btnbackcolor_dis;
	
	tgb->options.disabled = 0;
	tgb->options.bgpaint = BGP_NONE;
	tgb->options.repaintonpress = 1;
	
	tgb->textoptions.textalign_h = HTA_CENTER;
	tgb->textoptions.textalign_v = VTA_CENTER;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = (pressfunc)BTNA_GOPREVSCR;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;

	tgb->parentscreen = &tguiScreenNumenter;
	tgb->childscreen = NULL;

	// AC button
	tgb = &(tguiScrNumenterButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrNumenterButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->group_id = 0;
	tgb->position = {340, 106, 469, 155};

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->text = LSTR_AC;
	tgb->textposition = {340, 106, 469, 155};
	
	tgb->options.bgpaint = BGP_IMAGE;
	tgb->bgimagename_en = FNAME_BTN_NUMENTER_AC_EN;
	tgb->bgimagename_press = FNAME_BTN_NUMENTER_AC_PRESS;
	tgb->bgimagename_dis = FNAME_BTN_NUMENTER_AC_EN;
	
	tgb->options.disabled = 0;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = _tgui_NumenterACPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	
	// OK button
	tgb = &(tguiScrNumenterButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrNumenterButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->group_id = 0;
	tgb->position = {340, 262, 469, 311};

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->text = LSTR_OK;
	tgb->textposition = {340, 262, 469, 311};
	
	tgb->options.bgpaint = BGP_IMAGE;
	tgb->bgimagename_en = FNAME_BTN_NUMENTER_AC_EN;
	tgb->bgimagename_press = FNAME_BTN_NUMENTER_AC_PRESS;
	tgb->bgimagename_dis = FNAME_BTN_NUMENTER_AC_EN;
	
	tgb->options.disabled = 0;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = _tgui_NumenterOKPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	
	// DEL button
	tgb = &(tguiScrNumenterButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrNumenterButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->group_id = 0;
	tgb->position = {340, 158, 469, 207};

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->text = LSTR____;
	tgb->textposition = {340, 158, 469, 207};
	
	tgb->options.bgpaint = BGP_IMAGE;
	tgb->bgimagename_en = FNAME_BTN_NUMENTER_DEL_EN;
	tgb->bgimagename_press = FNAME_BTN_NUMENTER_DEL_PRESS;
	tgb->bgimagename_dis = FNAME_BTN_NUMENTER_DEL_EN;
	
	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = _tgui_NumenterDelPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	
	char *txt;
	// DOT button
	tgb = &(tguiScrNumenterButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrNumenterButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->group_id = 0;
	tgb->button_id = TG_SCR_NUMENTER_DOT_ID;
	tgb->position = {219, 262, 318, 311};

	tgb->textcolor_en = tgc->btntextcolor_en;

	txt = (char*)".";
	tgb->text = (LNG_STRING_ID)((DWORD)txt);
	tgb->textposition = {219, 262, 318, 311};
	
	tgb->options.bgpaint = BGP_IMAGE;
	tgb->bgimagename_en = FNAME_BTN_NUMENTER_DIG_EN;
	tgb->bgimagename_press = FNAME_BTN_NUMENTER_DIG_PRESS;
	tgb->bgimagename_dis = FNAME_BTN_NUMENTER_DIG_EN;
	
	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = _tgui_NumenterDotPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	
	// MINUS button
	tgb = &(tguiScrNumenterButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrNumenterButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->group_id = 0;
	tgb->button_id = TG_SCR_NUMENTER_MINUS_ID;
	tgb->position = {9, 262, 108, 311};

	tgb->textcolor_en = tgc->btntextcolor_en;

	txt = (char*)"-/+";
	tgb->text = (LNG_STRING_ID)((DWORD)txt);
	tgb->textposition = {9, 262, 108, 311};
	
	tgb->options.bgpaint = BGP_IMAGE;
	tgb->bgimagename_en = FNAME_BTN_NUMENTER_DIG_EN;
	tgb->bgimagename_press = FNAME_BTN_NUMENTER_DIG_PRESS;
	tgb->bgimagename_dis = FNAME_BTN_NUMENTER_DIG_EN;
	
	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = NULL;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	
	// digitals
	{
	// 7 button
	tgb = &(tguiScrNumenterButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrNumenterButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->group_id = 0;
	tgb->button_id = 8;
	tgb->position = {9, 106, 108, 155};

	tgb->textcolor_en = tgc->btntextcolor_en;

	txt = (char*)"7";
	tgb->text = (LNG_STRING_ID)((DWORD)txt);
	tgb->textposition = {9, 106, 108, 155};
	
	tgb->options.bgpaint = BGP_IMAGE;
	tgb->bgimagename_en = FNAME_BTN_NUMENTER_DIG_EN;
	tgb->bgimagename_press = FNAME_BTN_NUMENTER_DIG_PRESS;
	tgb->bgimagename_dis = FNAME_BTN_NUMENTER_DIG_EN;
	
	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = _tgui_NumenterNumbersPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	
	// 4 button
	tgb = &(tguiScrNumenterButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrNumenterButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->group_id = 0;
	tgb->button_id = 5;
	tgb->position = {9, 158, 108, 207};

	tgb->textcolor_en = tgc->btntextcolor_en;

	txt = (char*)"4";
	tgb->text = (LNG_STRING_ID)((DWORD)txt);
	tgb->textposition = {9, 158, 108, 207};
	
	// 1 button
	tgb = &(tguiScrNumenterButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrNumenterButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->group_id = 0;
	tgb->button_id = 2;
	tgb->position = {9, 210, 108, 259};

	tgb->textcolor_en = tgc->btntextcolor_en;

	txt = (char*)"1";
	tgb->text = (LNG_STRING_ID)((DWORD)txt);
	tgb->textposition = {9, 210, 108, 259};
	
	// 8 button
	tgb = &(tguiScrNumenterButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrNumenterButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->group_id = 0;
	tgb->button_id = 9;
	tgb->position = {114, 106, 213, 155};

	tgb->textcolor_en = tgc->btntextcolor_en;

	txt = (char*)"8";
	tgb->text = (LNG_STRING_ID)((DWORD)txt);
	tgb->textposition = {114, 106, 213, 155};
	
	// 5 button
	tgb = &(tguiScrNumenterButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrNumenterButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->group_id = 0;
	tgb->button_id = 6;
	tgb->position = {114, 158, 213, 207};

	tgb->textcolor_en = tgc->btntextcolor_en;

	txt = (char*)"5";
	tgb->text = (LNG_STRING_ID)((DWORD)txt);
	tgb->textposition = {114, 158, 213, 207};
	
	// 2 button
	tgb = &(tguiScrNumenterButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrNumenterButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->group_id = 0;
	tgb->button_id = 3;
	tgb->position = {114, 210, 213, 259};

	tgb->textcolor_en = tgc->btntextcolor_en;

	txt = (char*)"2";
	tgb->text = (LNG_STRING_ID)((DWORD)txt);
	tgb->textposition = {114, 210, 213, 259};
	
	// 0 button
	tgb = &(tguiScrNumenterButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrNumenterButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->group_id = 0;
	tgb->button_id = 1;
	tgb->position = {114, 262, 213, 311};

	tgb->textcolor_en = tgc->btntextcolor_en;

	txt = (char*)"0";
	tgb->text = (LNG_STRING_ID)((DWORD)txt);
	tgb->textposition = {114, 262, 213, 311};
	
	// 9 button
	tgb = &(tguiScrNumenterButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrNumenterButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->group_id = 0;
	tgb->button_id = 10;
	tgb->position = {219, 106, 318, 155};

	tgb->textcolor_en = tgc->btntextcolor_en;

	txt = (char*)"9";
	tgb->text = (LNG_STRING_ID)((DWORD)txt);
	tgb->textposition = {219, 106, 318, 155};
	
	// 6 button
	tgb = &(tguiScrNumenterButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrNumenterButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->group_id = 0;
	tgb->button_id = 7;
	tgb->position = {219, 158, 318, 207};

	tgb->textcolor_en = tgc->btntextcolor_en;

	txt = (char*)"6";
	tgb->text = (LNG_STRING_ID)((DWORD)txt);
	tgb->textposition = {219, 158, 318, 207};
	
	// 3 button
	tgb = &(tguiScrNumenterButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrNumenterButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->group_id = 0;
	tgb->button_id = 4;
	tgb->position = {219, 210, 318, 259};

	tgb->textcolor_en = tgc->btntextcolor_en;

	txt = (char*)"3";
	tgb->text = (LNG_STRING_ID)((DWORD)txt);
	tgb->textposition = {219, 210, 318, 259};
	
	}
	
	// NumScreen button
	tgb = &(tguiScrNumenterButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrNumenterButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->group_id = 0;
	tgb->button_id = TG_SCR_NUMENTER_DIGISCREEN;
	tgb->position = {12, 57, 317, 97};

	tgb->textcolor_en = LCDUI_RGB(0x2E2E2E);
	tgb->textcolor_dis = LCDUI_RGB(0x2E2E2E);
	tgb->backcolor_en = LCDUI_RGB(0xDDDDDD);
	tgb->backcolor_dis = LCDUI_RGB(0xDDDDDD);
	tgb->font = LCDUI_FONT_H36NUM;

	txt = (char*)"0.0";
	tgb->text = (LNG_STRING_ID)((DWORD)txt);
	tgb->textposition = {12, 57, 310, 97};
	
	tgb->options.bgpaint = BGP_FILL;
	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	tgb->bgimagename_dis = NULL;
	tgb->options.disabled = 1;
	
	tgb->funcs._call_paint = _tgui_NumenterDigiScreenPaint;
	tgb->funcs._call_press = NULL;
	tgb->funcs._call_process = NULL;
	
	
	// NUMENTER SCREEN
	tgs = &tguiScreenNumenter;
	memset((void*)tgs, 0, sizeof(TG_SCREEN));
	
	tgs->bgimagename = FNAME_BKGR_NUMENTER;
	tgs->prevscreen = &tguiScreenService;

	tgs->name = LSTR_LIFT_ON_PAUSE;
	tgs->nameposition = {205, 3, 475, 30};
	tgs->nameoptions.textalign_h = HTA_CENTER;
	tgs->nameoptions.textalign_v = VTA_CENTER;

	tgs->btns_count = TG_BTN_CNT_SCREEN_NUMENTER;
	tgs->buttons = tguiScrNumenterButtons;

	tgs->font = tgc->scrfont;
	tgs->namefont = tgc->scrnamefont;
	tgs->textcolor = tgc->scrtextcolor;
	tgs->nametextcolor = tgc->scrnametextcolor;
	tgs->backcolor = tgc->scrbackcolor;

	tgs->funcs._callpaint = _tgui_DefaultScreenPaint;
	tgs->funcs._process = _tgui_DefaultScreenProcess;
	
}



	// -------------------- Fileview Screen elements -----------------------
{
	bi = 0;
	// BACK button
	tgb = &(tguiScrFileviewButtons[bi++]);
	memset((void*)tgb, 0, sizeof(TG_BUTTON));
	
	tgb->position = {4, 4, 167, 49};

	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	tgb->bgimagename_dis = NULL;

	tgb->text = LSTR_BACK;
	tgb->textposition = {54, 6, 165, 47};
	tgb->font = tgc->btnfont;
	tgb->textcolor_en = LCDUI_RGB(0x074B19);
	tgb->textcolor_press = tgc->btntextcolor_press;
	tgb->textcolor_dis = tgc->btntextcolor_dis;
	tgb->backcolor_en = tgc->btnbackcolor_en;
	tgb->backcolor_press = tgc->btnbackcolor_press;
	tgb->backcolor_dis = tgc->btnbackcolor_dis;
	
	tgb->options.disabled = 0;
	tgb->options.bgpaint = BGP_NONE;
	tgb->options.repaintonpress = 1;
	
	tgb->textoptions.textalign_h = HTA_CENTER;
	tgb->textoptions.textalign_v = VTA_CENTER;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = (pressfunc)BTNA_GOPREVSCR;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;

	tgb->parentscreen = &tguiScrFilesButtons;
	tgb->childscreen = NULL;

	// DEL button
	tgb = &(tguiScrFileviewButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrFileviewButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->group_id = 0;
	tgb->position = {254, 247, 333, 311};

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->text = LSTR____;
	tgb->textposition = {254, 247, 333, 311};
	
	tgb->options.bgpaint = BGP_IMAGE;
	tgb->bgimagename_en = FNAME_BTN_FILEVIEW_DEL_EN;
	tgb->bgimagename_press = FNAME_BTN_FILEVIEW_DEL_PRESS;
	tgb->bgimagename_dis = NULL;
	
	tgb->options.disabled = 0;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = _tgui_FileviewDeletePress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	
	// PRINT button
	tgb = &(tguiScrFileviewButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrFileviewButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {389, 247, 468, 311};

	tgb->textposition = {389, 247, 468, 311};
	
	tgb->bgimagename_en = FNAME_BTN_FILEVIEW_PRINT_EN;
	tgb->bgimagename_press = FNAME_BTN_FILEVIEW_PRINT_PRESS;

	tgb->funcs._call_press = _tgui_FileviewPrintPress;

	// FILEINFO region
	tgb = &(tguiScrFileviewButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrFileviewButtons[bi-2]), sizeof(TG_BUTTON));

	tgb->position = {7, 235, 246, 311};

	tgb->textposition = {7, 235, 246, 311};

	tgb->options.bgpaint = BGP_NONE;
	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;

	tgb->funcs._call_paint = _tgui_FileviewFileinfoPaint;

	// PRINTINFO region
	tgb = &(tguiScrFileviewButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrFileviewButtons[bi-2]), sizeof(TG_BUTTON));

	tgb->position = {248, 54, 472, 233};

	tgb->textposition = {248, 54, 472, 233};

	tgb->funcs._call_process = NULL;
	tgb->funcs._call_paint = _tgui_FileviewPrintinfoPaint;

	// PREVIEW image
	tgb = &(tguiScrFileviewButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrFileviewButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {7, 54, 246, 233};

	tgb->textposition = {7, 54, 246, 233};
	
	tgb->funcs._call_process = NULL;
	tgb->funcs._call_paint = _tgui_FileviewPreviewPaint;

	// FILEVIEW SCREEN
	tgs = &tguiScreenFileview;
	memset((void*)tgs, 0, sizeof(TG_SCREEN));
	
	tgs->bgimagename = FNAME_BKGR_FILEVIEW;
	tgs->prevscreen = &tguiScreenService;

	tgs->name = LSTR_FILEVIEW_CAPTION;
	tgs->nameposition = {205, 3, 475, 30};
	tgs->nameoptions.textalign_h = HTA_CENTER;
	tgs->nameoptions.textalign_v = VTA_CENTER;

	tgs->btns_count = TG_BTN_CNT_SCREEN_FILEVIEW;
	tgs->buttons = tguiScrFileviewButtons;

	tgs->font = tgc->scrfont;
	tgs->namefont = tgc->scrnamefont;
	tgs->textcolor = tgc->scrtextcolor;
	tgs->nametextcolor = tgc->scrnametextcolor;
	tgs->backcolor = tgc->scrbackcolor;

	tgs->funcs._callpaint = _tgui_DefaultScreenPaint;
	tgs->funcs._process = _tgui_DefaultScreenProcess;
	
}


	// -------------------- Printing Screen elements -----------------------
{
	bi = 0;
	// PAUSE button
	tgb = &(tguiScrPrintButtons[bi++]);
	memset((void*)tgb, 0, sizeof(TG_BUTTON));
	
	tgb->position = {334, 38, 468, 95};
	tgb->group_id = 1;
	tgb->button_id = TG_SCR_PRINT_PAUSE_BTN_ID;

	tgb->bgimagename_en = FNAME_BTN_PRINT_PAUSE_EN;
	tgb->bgimagename_press = FNAME_BTN_PRINT_PAUSE_PRESS;
	tgb->bgimagename_act = FNAME_BTN_PRINT_PAUSE_ACT;
	tgb->bgimagename_dis = FNAME_BTN_PRINT_PAUSE_PRESS;

	tgb->text = LSTR____;
	tgb->textposition = {334, 38, 468, 95};
	tgb->font = tgc->btnfont;
	tgb->textcolor_en = LCDUI_RGB(0x074B19);
	tgb->textcolor_press = tgc->btntextcolor_press;
	tgb->textcolor_dis = tgc->btntextcolor_dis;
	tgb->backcolor_en = tgc->btnbackcolor_en;
	tgb->backcolor_press = tgc->btnbackcolor_press;
	tgb->backcolor_dis = tgc->btnbackcolor_dis;
	
	tgb->options.disabled = 0;
	tgb->options.bgpaint = BGP_IMAGE;
	tgb->options.repaintonpress = 1;
	
	tgb->textoptions.textalign_h = HTA_CENTER;
	tgb->textoptions.textalign_v = VTA_CENTER;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	tgb->funcs._call_press = _tgui_PrintScreenPausePress;

	tgb->parentscreen = &tguiScreenPrint;
	tgb->childscreen = NULL;

	// SETTINGS button
	tgb = &(tguiScrPrintButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrPrintButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->group_id = 0;
	tgb->button_id = 0;
	
	tgb->position = {334, 99, 468, 156};

	tgb->textposition = {334, 99, 468, 156};
	
	tgb->bgimagename_en = FNAME_BTN_PRINT_SETT_EN;
	tgb->bgimagename_press = FNAME_BTN_PRINT_SETT_PRESS;
	tgb->bgimagename_act = NULL;
	tgb->bgimagename_dis = FNAME_BTN_PRINT_SETT_EN;

	tgb->funcs._call_press = _tgui_PrnparamsScreenShow;

	// STOP button
	tgb = &(tguiScrPrintButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrPrintButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {334, 160, 468, 217};

	tgb->textposition = {334, 160, 468, 217};
	
	tgb->bgimagename_en = FNAME_BTN_PRINT_STOP_EN;
	tgb->bgimagename_press = FNAME_BTN_PRINT_STOP_PRESS;
	tgb->bgimagename_dis = FNAME_BTN_PRINT_STOP_EN;

	tgb->funcs._call_press = _tgui_PrintScreenStopPress;

	// PREVIEW image
	tgb = &(tguiScrPrintButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrPrintButtons[bi-2]), sizeof(TG_BUTTON));

	tgb->button_id = TG_SCR_PRINT_PREVIEW_ID;

	tgb->position = {7, 39, 326, 218};

	tgb->textposition = {7, 39, 326, 218};
	
	tgb->backcolor_en = LCDUI_RGB(0x000000);

	tgb->options.bgpaint = BGP_NONE;
	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	
	tgb->funcs._call_paint = _tgui_PrintScreenPreviewPaint;
	tgb->funcs._call_process = NULL;
	tgb->funcs._call_press = NULL;
	
	// PRINTINFO region
	tgb = &(tguiScrPrintButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrPrintButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_PRINT_INFO_ID;

	tgb->position = {7, 221, 471, 312};

	tgb->textposition = {7, 221, 471, 312};
	
	tgb->backcolor_en = LCDUI_RGB(0xDDDDDD);

	tgb->funcs._call_paint = _tgui_PrintScreenProgressPaint;
	
	// LOCK region
	tgb = &(tguiScrPrintButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrPrintButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_PRINT_LOCK_BTN_ID;

	tgb->position = {200, 4, 475, 30};

	tgb->textposition = {455, 6, 473, 27};
	
	tgb->options.bgpaint = BGP_NONE;
	tgb->bgimagename_en = FNAME_BTN_PRINT_LOCK_EN;
	tgb->bgimagename_act = FNAME_BTN_PRINT_LOCK_ACT;
	
	tgb->funcs._call_paint = _tgui_PrintScreenLockPaint;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	tgb->funcs._call_longpress = _tgui_PrintScreenLockLPress;


	// PRINT SCREEN
	tgs = &tguiScreenPrint;
	memset((void*)tgs, 0, sizeof(TG_SCREEN));
	
	tgs->bgimagename = FNAME_BKGR_PRINT;
	tgs->prevscreen = &tguiScreenService;

	tgs->name = LSTR_PRINTING_CAPTION;
	tgs->nameposition = {205, 3, 475, 30};
	tgs->nameoptions.textalign_h = HTA_CENTER;
	tgs->nameoptions.textalign_v = VTA_CENTER;

	tgs->btns_count = TG_BTN_CNT_SCREEN_PRINT;
	tgs->buttons = tguiScrPrintButtons;

	tgs->font = tgc->scrfont;
	tgs->namefont = tgc->scrnamefont;
	tgs->textcolor = tgc->scrtextcolor;
	tgs->nametextcolor = tgc->scrnametextcolor;
	tgs->backcolor = tgc->scrbackcolor;

	tgs->funcs._callpaint = _tgui_DefaultScreenPaint;
	tgs->funcs._process = _tgui_PrintScreenProcess;
	
}


	// -------------------- ClockSet Screen elements -----------------------
	id = 1;
{
	bi = 0;
	// BACK button
	tgb = &(tguiScrClockSetButtons[bi++]);
	memset((void*)tgb, 0, sizeof(TG_BUTTON));
	
	tgb->position = {4, 4, 167, 49};

	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	tgb->bgimagename_dis = NULL;

	tgb->text = LSTR_BACK;
	tgb->textposition = {54, 6, 165, 47};
	tgb->font = tgc->btnfont;
	tgb->textcolor_en = LCDUI_RGB(0x074B19);
	tgb->textcolor_press = tgc->btntextcolor_press;
	tgb->textcolor_dis = tgc->btntextcolor_dis;
	tgb->backcolor_en = tgc->btnbackcolor_en;
	tgb->backcolor_press = tgc->btnbackcolor_press;
	tgb->backcolor_dis = tgc->btnbackcolor_dis;
	
	tgb->options.disabled = 0;
	tgb->options.bgpaint = BGP_NONE;
	tgb->options.repaintonpress = 1;
	
	tgb->textoptions.textalign_h = HTA_CENTER;
	tgb->textoptions.textalign_v = VTA_CENTER;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = (pressfunc)BTNA_GOPREVSCR;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;

	tgb->parentscreen = &tguiScreenClockSet;
	tgb->childscreen = NULL;

	// OK button
	tgb = &(tguiScrClockSetButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrClockSetButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->group_id = 0;
	tgb->position = {386, 65, 469, 148};
	tgb->textposition = {386, 65, 469, 148};

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->text = LSTR____;
	
	tgb->options.bgpaint = BGP_IMAGE;
	tgb->bgimagename_en = FNAME_BTN_CLOCKSET_OK_EN;
	tgb->bgimagename_press = FNAME_BTN_CLOCKSET_OK_PRESS;
	
	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	tgb->funcs._call_press = _tgui_ClockSetOkButtonPress;
	
	// Hours UP button
	tgb = &(tguiScrClockSetButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrClockSetButtons[bi-2]), sizeof(TG_BUTTON));

	tgb->button_id = TG_SCR_CLOCKSET_HOURS_UP_BTN_ID;

	tgb->position = {82, 65, 153, 114};
	tgb->textposition = {82, 65, 153, 114};
	
	tgb->textcolor_en = LCDUI_RGB(0xD0D0D0);

	tgb->bgimagename_en = FNAME_BTN_CLOCKSET_UP_EN;
	tgb->bgimagename_press = FNAME_BTN_CLOCKSET_UP_PRESS;

	tgb->funcs._call_press = _tgui_ClockSetUpButtonPress;

	// Minutes UP button
	tgb = &(tguiScrClockSetButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrClockSetButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_CLOCKSET_MINUTES_UP_BTN_ID;

	tgb->position = {237, 65, 308, 114};
	tgb->textposition = {237, 65, 308, 114};
	
	tgb->funcs._call_press = _tgui_ClockSetUpButtonPress;

	// Day UP button
	tgb = &(tguiScrClockSetButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrClockSetButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_CLOCKSET_DAY_UP_BTN_ID;

	tgb->position = {82, 183, 153, 232};
	tgb->textposition = {82, 183, 153, 232};
	
	tgb->funcs._call_press = _tgui_ClockSetUpButtonPress;

	// Month UP button
	tgb = &(tguiScrClockSetButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrClockSetButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_CLOCKSET_MONTH_UP_BTN_ID;

	tgb->position = {237, 183, 308, 232};
	tgb->textposition = {237, 183, 308, 232};
	
	tgb->funcs._call_press = _tgui_ClockSetUpButtonPress;

	// Year UP button
	tgb = &(tguiScrClockSetButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrClockSetButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_CLOCKSET_YEAR_UP_BTN_ID;

	tgb->position = {398, 183, 469, 232};
	tgb->textposition = {398, 183, 469, 232};
	
	tgb->funcs._call_press = _tgui_ClockSetUpButtonPress;

	// Hours DOWN button
	tgb = &(tguiScrClockSetButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrClockSetButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {82, 119, 153, 168};
	tgb->textposition = {82, 119, 153, 168};
	
	tgb->button_id = TG_SCR_CLOCKSET_HOURS_DOWN_BTN_ID;

	tgb->bgimagename_en = FNAME_BTN_CLOCKSET_DOWN_EN;
	tgb->bgimagename_press = FNAME_BTN_CLOCKSET_DOWN_PRESS;

	tgb->funcs._call_press = _tgui_ClockSetDownButtonPress;

	// Minutes DOWN button
	tgb = &(tguiScrClockSetButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrClockSetButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_CLOCKSET_MINUTES_DOWN_BTN_ID;

	tgb->position = {237, 119, 308, 168};
	tgb->textposition = {237, 119, 308, 168};
	
	tgb->funcs._call_press = _tgui_ClockSetDownButtonPress;

	// Day DOWN button
	tgb = &(tguiScrClockSetButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrClockSetButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_CLOCKSET_DAY_DOWN_BTN_ID;

	tgb->position = {82, 237, 153, 286};
	tgb->textposition = {82, 237, 153, 286};
	
	tgb->funcs._call_press = _tgui_ClockSetDownButtonPress;

	// Month DOWN button
	tgb = &(tguiScrClockSetButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrClockSetButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_CLOCKSET_MONTH_DOWN_BTN_ID;

	tgb->position = {237, 237, 308, 286};
	tgb->textposition = {237, 237, 308, 286};
	
	tgb->funcs._call_press = _tgui_ClockSetDownButtonPress;

	// Year DOWN button
	tgb = &(tguiScrClockSetButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrClockSetButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_CLOCKSET_YEAR_DOWN_BTN_ID;

	tgb->position = {398, 237, 469, 286};
	tgb->textposition = {398, 237, 469, 286};
	
	tgb->funcs._call_press = _tgui_ClockSetDownButtonPress;

	// Hours field
	tgb = &(tguiScrClockSetButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrClockSetButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_CLOCKSET_HOURS_BTN_ID;
	
	tgb->position = {8, 90, 77, 143};
	tgb->textposition = {10, 92, 73, 141};

	tgb->textcolor_en = LCDUI_RGB(0x000000);
	tgb->backcolor_en = LCDUI_RGB(0xE9E9E9);
	tgb->textcolor_dis = LCDUI_RGB(0x000000);
	tgb->backcolor_dis = LCDUI_RGB(0x000000);
	
	tgb->font = LCDUI_FONT_H36NUM;

	tgb->text = LSTR____;
	
	tgb->options.disabled = 1;
	
	tgb->options.bgpaint = BGP_FILL;
	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	
	tgb->funcs._call_paint = _tgui_ClockSetItemPaint;
	tgb->funcs._call_process = NULL;
	tgb->funcs._call_press = NULL;

	// Minutes field
	tgb = &(tguiScrClockSetButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrClockSetButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_CLOCKSET_MINUTES_BTN_ID;
	
	tgb->position = {163, 90, 232, 143};
	tgb->textposition = {165, 92, 228, 141};

	tgb->funcs._call_paint = _tgui_ClockSetItemPaint;

	// Day field
	tgb = &(tguiScrClockSetButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrClockSetButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_CLOCKSET_DAY_BTN_ID;
	
	tgb->position = {8, 208, 77, 261};
	tgb->textposition = {10, 210, 73, 259};

	tgb->funcs._call_paint = _tgui_ClockSetItemPaint;

	// Month field
	tgb = &(tguiScrClockSetButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrClockSetButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_CLOCKSET_MONTH_BTN_ID;
	
	tgb->position = {163, 208, 232, 261};
	tgb->textposition = {165, 210, 228, 259};

	tgb->funcs._call_paint = _tgui_ClockSetItemPaint;

	// Day field
	tgb = &(tguiScrClockSetButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrClockSetButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_CLOCKSET_YEAR_BTN_ID;
	
	tgb->position = {324, 208, 393, 261};
	tgb->textposition = {326, 210, 389, 259};

	tgb->funcs._call_paint = _tgui_ClockSetItemPaint;


	// CLOCKSET SCREEN
	tgs = &tguiScreenClockSet;
	memset((void*)tgs, 0, sizeof(TG_SCREEN));
	
	tgs->bgimagename = FNAME_BKGR_CLOCKSET;
	tgs->prevscreen = &tguiScreenService;

	tgs->name = LSTR_CLOCKSET_CAPTION;
	tgs->nameposition = {205, 3, 475, 30};
	tgs->nameoptions.textalign_h = HTA_CENTER;
	tgs->nameoptions.textalign_v = VTA_CENTER;

	tgs->btns_count = TG_BTN_CNT_SCREEN_CLOCKSET;
	tgs->buttons = tguiScrClockSetButtons;

	tgs->font = LCDUI_FONT_H24;
	tgs->namefont = tgc->scrnamefont;
	tgs->textcolor = LCDUI_RGB(0xD0D0D0);
	tgs->nametextcolor = tgc->scrnametextcolor;
	tgs->backcolor = tgc->scrbackcolor;

	tgs->funcs._callpaint = _tgui_ClockSetScreenPaint;
	tgs->funcs._process = _tgui_DefaultScreenProcess;
	
}


	// -------------------- ScreenSaver elements -----------------------
	id = 1;
{
	bi = 0;
	// No buttons
	tgb = &(tguiScreenSaverButtons[bi++]);
	memset((void*)tgb, 0, sizeof(TG_BUTTON));


	// CLOCKSET SCREEN
	tgs = &tguiScreenSaver;
	memset((void*)tgs, 0, sizeof(TG_SCREEN));
	
	tgs->bgimagename = NULL;
	tgs->prevscreen = NULL;

	tgs->name = LSTR____;

	tgs->btns_count = TG_BTN_CNT_SCREENSAVER;
	tgs->buttons = tguiScreenSaverButtons;

	tgs->font = LCDUI_FONT_H36;
	tgs->textcolor = LCDUI_RGB(0xD6D6D6);
	tgs->nametextcolor = LCDUI_RGB(0xD3BA89);
	tgs->backcolor = LCDUI_RGB(0x000000);

	tgs->funcs._callpaint = _tgui_ScreenSaverPaint;
	tgs->funcs._process = _tgui_ScreenSaverProcess;
	
}



	// -------------------- UV Test Screen elements -----------------------
{
	bi = 0;
	// BACK button
	tgb = &(tguiScrUVTestButtons[bi++]);
	memset((void*)tgb, 0, sizeof(TG_BUTTON));
	
	tgb->position = {4, 4, 167, 49};

	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	tgb->bgimagename_dis = NULL;

	tgb->text = LSTR_BACK;
	tgb->textposition = {54, 6, 165, 47};
	tgb->font = tgc->btnfont;
	tgb->textcolor_en = LCDUI_RGB(0x074B19);
	tgb->textcolor_press = tgc->btntextcolor_press;
	tgb->textcolor_dis = tgc->btntextcolor_dis;
	tgb->backcolor_en = tgc->btnbackcolor_en;
	tgb->backcolor_press = tgc->btnbackcolor_press;
	tgb->backcolor_dis = tgc->btnbackcolor_dis;
	
	tgb->options.disabled = 0;
	tgb->options.bgpaint = BGP_NONE;
	tgb->options.repaintonpress = 1;
	
	tgb->textoptions.textalign_h = HTA_CENTER;
	tgb->textoptions.textalign_v = VTA_CENTER;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = _tgui_UVTestBackButtonPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;

	tgb->parentscreen = &tguiScreenUVTest;
	tgb->childscreen = NULL;

	// Image1 button
	tgb = &(tguiScrUVTestButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrUVTestButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->group_id = TG_SCR_UVTEST_IMG_GROUP_ID;
	tgb->button_id = TG_SCR_UVTEST_IMG1_BTN_ID;
	
	tgb->position = {10, 60, 159, 139};

	tgb->text = LSTR____;
	tgb->textposition = {10, 60, 159, 139};

	tgb->options.active = 1;

	tgb->options.bgpaint = BGP_IMAGE;
	tgb->bgimagename_en = FNAME_BTN_UVTEST_IMAGE1_EN;
	tgb->bgimagename_press = FNAME_BTN_UVTEST_IMAGE1_PRESS;
	tgb->bgimagename_act = FNAME_BTN_UVTEST_IMAGE1_PRESS;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = _tgui_UVTestImgSelectButtonPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;

	// Image2 button
	tgb = &(tguiScrUVTestButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrUVTestButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_UVTEST_IMG2_BTN_ID;
	
	tgb->position = {165, 60, 314, 139};

	tgb->textposition = {165, 60, 314, 139};

	tgb->options.active = 0;

	tgb->bgimagename_en = FNAME_BTN_UVTEST_IMAGE2_EN;
	tgb->bgimagename_press = FNAME_BTN_UVTEST_IMAGE2_PRESS;
	tgb->bgimagename_act = FNAME_BTN_UVTEST_IMAGE2_PRESS;

	// Image3 button
	tgb = &(tguiScrUVTestButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrUVTestButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_UVTEST_IMG3_BTN_ID;

	tgb->position = {320, 60, 469, 139};

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->textposition = {320, 60, 469, 139};

	tgb->bgimagename_en = FNAME_BTN_UVTEST_IMAGE3_EN;
	tgb->bgimagename_press = FNAME_BTN_UVTEST_IMAGE3_PRESS;
	tgb->bgimagename_act = FNAME_BTN_UVTEST_IMAGE3_PRESS;

	// LIGHT button
	tgb = &(tguiScrUVTestButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrUVTestButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->group_id = 0;
	tgb->button_id = 0;
	
	tgb->position = {75, 167, 394, 221};

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->text = LSTR____;
	tgb->textposition = {155, 167, 394, 221};
	
	tgb->options.bgpaint = BGP_IMAGE;
	tgb->bgimagename_en = FNAME_BTN_UVTEST_LIGHT_EN;
	tgb->bgimagename_press = FNAME_BTN_UVTEST_LIGHT_PRESS;

	tgb->funcs._call_paint = _tgui_UVTestLightButtonPaint;
	tgb->funcs._call_press = _tgui_UVTestLightButtonPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	
	// DISP button
	tgb = &(tguiScrUVTestButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrUVTestButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {75, 231, 394, 285};

	tgb->textposition = {155, 231, 394, 285};
	
	tgb->bgimagename_en = FNAME_BTN_UVTEST_DISP_EN;
	tgb->bgimagename_press = FNAME_BTN_UVTEST_DISP_PRESS;

	tgb->funcs._call_paint = _tgui_UVTestDispButtonPaint;
	tgb->funcs._call_press = _tgui_UVTestDispButtonPress;
	
	
	// UVTEST SCREEN
	tgs = &tguiScreenUVTest;
	memset((void*)tgs, 0, sizeof(TG_SCREEN));
	
	tgs->bgimagename = FNAME_BKGR_UVTEST;
	tgs->prevscreen = &tguiScreenService;

	tgs->name = LSTR_UVTEST;
	tgs->nameposition = {205, 3, 475, 30};
	tgs->nameoptions.textalign_h = HTA_CENTER;
	tgs->nameoptions.textalign_v = VTA_CENTER;

	tgs->btns_count = TG_BTN_CNT_SCREEN_UVTEST;
	tgs->buttons = tguiScrUVTestButtons;

	tgs->font = tgc->scrfont;
	tgs->namefont = tgc->scrnamefont;
	tgs->textcolor = tgc->scrtextcolor;
	tgs->nametextcolor = tgc->scrnametextcolor;
	tgs->backcolor = tgc->scrbackcolor;

	tgs->funcs._callpaint = _tgui_DefaultScreenPaint;
	tgs->funcs._process = _tgui_DefaultScreenProcess;
//	tgs->funcs._process = _tgui_MovezScreenProcess;
	
}



	// -------------------- Printing parameters Screen elements -----------------------
	id = 1;
{
	bi = 0;
	// BACK button
	tgb = &(tguiScrPrnparamsButtons[bi++]);
	memset((void*)tgb, 0, sizeof(TG_BUTTON));
	
	tgb->position = {4, 4, 167, 49};

	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	tgb->bgimagename_dis = NULL;

	tgb->text = LSTR_BACK;
	tgb->textposition = {54, 6, 165, 47};
	tgb->font = tgc->btnfont;
	tgb->textcolor_en = LCDUI_RGB(0x074B19);
	tgb->textcolor_press = tgc->btntextcolor_press;
	tgb->textcolor_dis = tgc->btntextcolor_dis;
	tgb->backcolor_en = tgc->btnbackcolor_en;
	tgb->backcolor_press = tgc->btnbackcolor_press;
	tgb->backcolor_dis = tgc->btnbackcolor_dis;
	
	tgb->options.disabled = 0;
	tgb->options.bgpaint = BGP_NONE;
	tgb->options.repaintonpress = 1;
	
	tgb->textoptions.textalign_h = HTA_CENTER;
	tgb->textoptions.textalign_v = VTA_CENTER;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = (pressfunc)BTNA_GOPREVSCR;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;

	tgb->parentscreen = &tguiScreenPrnparams;
	tgb->childscreen = NULL;

	// UP button
	tgb = &(tguiScrPrnparamsButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrPrnparamsButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->group_id = 0;
	tgb->position = {393, 183, 472, 243};

	tgb->button_id = TG_SCR_PRNPARAMS_UP_ID;

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->text = LSTR____;
	tgb->textposition = {393, 183, 472, 243};
	
	tgb->options.bgpaint = BGP_IMAGE;
	tgb->bgimagename_en = FNAME_BTN_PRNPARAMS_UP_EN;
	tgb->bgimagename_press = FNAME_BTN_PRNPARAMS_UP_PRESS;
	tgb->bgimagename_dis = FNAME_BTN_PRNPARAMS_UP_DIS;
	
	tgb->options.disabled = 1;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = _tgui_PrnparamsUpButtonPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	
	// DOWN button
	tgb = &(tguiScrPrnparamsButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrPrnparamsButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {393, 248, 472, 308};

	tgb->button_id = TG_SCR_PRNPARAMS_DOWN_ID;

	tgb->textposition = {393, 248, 472, 308};
	
	tgb->bgimagename_en = FNAME_BTN_PRNPARAMS_DOWN_EN;
	tgb->bgimagename_press = FNAME_BTN_PRNPARAMS_DOWN_PRESS;
	tgb->bgimagename_dis = FNAME_BTN_PRNPARAMS_DOWN_DIS;
	
	tgb->options.disabled = 0;

	tgb->funcs._call_press = _tgui_PrnparamsDownButtonPress;

	// SAVE button
	tgb = &(tguiScrPrnparamsButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrPrnparamsButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {393, 118, 472, 178};

	tgb->button_id = 0;

	tgb->textposition = {393, 118, 472, 178};
	
	tgb->bgimagename_en = FNAME_BTN_PRNPARAMS_SAVE_EN;
	tgb->bgimagename_press = FNAME_BTN_PRNPARAMS_SAVE_PRESS;
	tgb->bgimagename_dis = FNAME_BTN_PRNPARAMS_SAVE_DIS;

	tgb->options.disabled = 1;

	tgb->funcs._call_press = NULL;

	// OK button
	tgb = &(tguiScrPrnparamsButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrPrnparamsButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {393, 44, 472, 104};

	tgb->button_id = 0;

	tgb->textposition = {393, 44, 472, 104};
	
	tgb->bgimagename_en = FNAME_BTN_PRNPARAMS_OK_EN;
	tgb->bgimagename_press = FNAME_BTN_PRNPARAMS_OK_PRESS;

	tgb->options.disabled = 0;

	tgb->funcs._call_press = _tgui_PrnparamsOkButtonPress;


	// Bottom light
	by = 51;
	tgb = &(tguiScrPrnparamsButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrPrnparamsButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {8, 57, 387, 104};

	tgb->button_id = TG_SCR_PRNPARAMS_LIGHTBOTTOM_ID;

	tgb->font = LCDUI_FONT_H24;

	tgb->textcolor_en = tgc->btntextcolor_en;
	tgb->textcolor_act = tgc->btntextcolor_en;
	tgb->textcolor_dis = LCDUI_RGB(0x727272);
	tgb->textposition = {13, 57, 382, 104};
	tgb->text = LSTR_PP_LIGHT_BOTTOM;

	tgb->options.bgpaint = BGP_IMAGE;
	tgb->bgimagename_en = FNAME_BTN_PRNPARAMS_ITEM_EN;
	tgb->bgimagename_press = FNAME_BTN_PRNPARAMS_ITEM_PRESS;
	tgb->bgimagename_dis = FNAME_BTN_PRNPARAMS_ITEM_DIS;

	tgb->options.disabled = 0;

	tgb->funcs._call_paint = _tgui_PrnparamsItemButtonPaint;
	tgb->funcs._call_press = _tgui_PrnparamsItemButtonPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;

	// Light
	tgb = &(tguiScrPrnparamsButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrPrnparamsButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {8, (int16_t)(tguiScrPrnparamsButtons[bi-2].position.top + by), 387, (int16_t)(tguiScrPrnparamsButtons[bi-2].position.bottom + by)};

	tgb->button_id = TG_SCR_PRNPARAMS_LIGHT_ID;

	tgb->textcolor_en = tgc->btntextcolor_en;
	tgb->textcolor_act = tgc->btntextcolor_en;
	tgb->textposition = {13, (int16_t)(tguiScrPrnparamsButtons[bi-2].position.top + by), 382, (int16_t)(tguiScrPrnparamsButtons[bi-2].position.bottom + by)};
	tgb->text = LSTR_PP_LIGHT;

	tgb->funcs._call_press = _tgui_PrnparamsItemButtonPress;

	// Lift height
	tgb = &(tguiScrPrnparamsButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrPrnparamsButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {8, (int16_t)(tguiScrPrnparamsButtons[bi-2].position.top + by), 387, (int16_t)(tguiScrPrnparamsButtons[bi-2].position.bottom + by)};

	tgb->button_id = TG_SCR_PRNPARAMS_LIFTHEIGHT_ID;

	tgb->textcolor_en = tgc->btntextcolor_en;
	tgb->textcolor_act = tgc->btntextcolor_en;
	tgb->textposition = {13, (int16_t)(tguiScrPrnparamsButtons[bi-2].position.top + by), 382, (int16_t)(tguiScrPrnparamsButtons[bi-2].position.bottom + by)};
	tgb->text = LSTR_PP_LIFT_HEIGHT;

	tgb->funcs._call_press = _tgui_PrnparamsItemButtonPress;

	// Lift speed
	tgb = &(tguiScrPrnparamsButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrPrnparamsButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {8, (int16_t)(tguiScrPrnparamsButtons[bi-2].position.top + by), 387, (int16_t)(tguiScrPrnparamsButtons[bi-2].position.bottom + by)};

	tgb->button_id = TG_SCR_PRNPARAMS_LIFTSPEED_ID;

	tgb->textcolor_en = tgc->btntextcolor_en;
	tgb->textcolor_act = tgc->btntextcolor_en;
	tgb->textposition = {13, (int16_t)(tguiScrPrnparamsButtons[bi-2].position.top + by), 382, (int16_t)(tguiScrPrnparamsButtons[bi-2].position.bottom + by)};
	tgb->text = LSTR_PP_LIFT_SPEED;

	tgb->funcs._call_press = _tgui_PrnparamsItemButtonPress;

	// Drop speed
	tgb = &(tguiScrPrnparamsButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrPrnparamsButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {8, (int16_t)(tguiScrPrnparamsButtons[bi-2].position.top + by), 387, (int16_t)(tguiScrPrnparamsButtons[bi-2].position.bottom + by)};

	tgb->button_id = TG_SCR_PRNPARAMS_DROPSPEED_ID;

	tgb->textcolor_en = tgc->btntextcolor_en;
	tgb->textcolor_act = tgc->btntextcolor_en;
	tgb->textposition = {13, (int16_t)(tguiScrPrnparamsButtons[bi-2].position.top + by), 382, (int16_t)(tguiScrPrnparamsButtons[bi-2].position.bottom + by)};
	tgb->text = LSTR_PP_DROP_SPEED;

	tgb->funcs._call_press = _tgui_PrnparamsItemButtonPress;

	// Light pause
	tgb = &(tguiScrPrnparamsButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrPrnparamsButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {8, 57, 387, 104};

	tgb->button_id = TG_SCR_PRNPARAMS_LIGHTPAUSE_ID;

	tgb->textcolor_en = tgc->btntextcolor_en;
	tgb->textcolor_act = tgc->btntextcolor_en;
	tgb->textposition = {13, 57, 382, 104};
	tgb->text = LSTR_PP_LIGHT_PAUSE;

	tgb->options.disabled = 1;

	tgb->funcs._call_press = _tgui_PrnparamsItemButtonPress;

	// Antialiasing
	tgb = &(tguiScrPrnparamsButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrPrnparamsButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {8, (int16_t)(tguiScrPrnparamsButtons[bi-2].position.top + by), 387, (int16_t)(tguiScrPrnparamsButtons[bi-2].position.bottom + by)};

	tgb->button_id = TG_SCR_PRNPARAMS_AA_ID;

	tgb->textcolor_en = tgc->btntextcolor_en;
	tgb->textcolor_act = tgc->btntextcolor_en;
	tgb->textposition = {13, (int16_t)(tguiScrPrnparamsButtons[bi-2].position.top + by), 382, (int16_t)(tguiScrPrnparamsButtons[bi-2].position.bottom + by)};
	tgb->text = LSTR_PP_AA;

	tgb->funcs._call_press = _tgui_PrnparamsItemButtonPress;

	// Individual layers parameters
	tgb = &(tguiScrPrnparamsButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrPrnparamsButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {8, (int16_t)(tguiScrPrnparamsButtons[bi-2].position.top + by), 387, (int16_t)(tguiScrPrnparamsButtons[bi-2].position.bottom + by)};

	tgb->button_id = TG_SCR_PRNPARAMS_INDPARAMS_ID;

	tgb->textcolor_en = tgc->btntextcolor_en;
	tgb->textcolor_act = tgc->btntextcolor_en;
	tgb->textposition = {13, (int16_t)(tguiScrPrnparamsButtons[bi-2].position.top + by), 382, (int16_t)(tguiScrPrnparamsButtons[bi-2].position.bottom + by)};
	tgb->text = LSTR_PP_IND_SETTINGS;

	tgb->funcs._call_press = _tgui_PrnparamsItemButtonPress;



	// PRNPARAMS SCREEN
	tgs = &tguiScreenPrnparams;
	memset((void*)tgs, 0, sizeof(TG_SCREEN));
	
	tgs->bgimagename = FNAME_BKGR_EMPTY;
	tgs->prevscreen = &tguiScreenPrint;

	tgs->name = LSTR_PP_PRINTING_PARAMETERS;
	tgs->nameposition = {205, 3, 475, 30};
	tgs->nameoptions.textalign_h = HTA_CENTER;
	tgs->nameoptions.textalign_v = VTA_CENTER;

	tgs->btns_count = TG_BTN_CNT_SCREEN_PRNPARAMS;
	tgs->buttons = tguiScrPrnparamsButtons;

	tgs->font = tgc->scrfont;
	tgs->namefont = tgc->scrnamefont;
	tgs->textcolor = tgc->scrtextcolor;
	tgs->nametextcolor = tgc->scrnametextcolor;
	tgs->backcolor = tgc->scrbackcolor;

	tgs->funcs._callpaint = _tgui_PrnparamsScreenPaint;
	tgs->funcs._process = _tgui_DefaultScreenProcess;
	
}


	// -------------------- Touch calibration elements -----------------------
	id = 1;
{
	bi = 0;
	// No buttons
	tgb = &(tguiScrCalibButtons[bi++]);
	memset((void*)tgb, 0, sizeof(TG_BUTTON));
	tgb->position = {0, 0, 479, 319};

	tgb->funcs._call_longpress = _tgui_CalibButtonLongPress;
	tgb->funcs._call_press = _tgui_CalibButtonPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;


	// TOUCH CALIBRATION
	tgs = &tguiScreenCalib;
	memset((void*)tgs, 0, sizeof(TG_SCREEN));
	
	tgs->bgimagename = NULL;
	tgs->prevscreen = &tguiScreenService;

	tgs->name = LSTR____;

	tgs->btns_count = TG_BTN_CNT_SCREEN_CALIB;
	tgs->buttons = tguiScrCalibButtons;

	tgs->font = LCDUI_FONT_H18;
	tgs->textcolor = LCDUI_RGB(0xA6BFCB);
	tgs->backcolor = LCDUI_RGB(0x000000);

	tgs->funcs._callpaint = _tgui_CalibScreenPaint;
	tgs->funcs._process = _tgui_DefaultScreenProcess;
	
}



}
//==============================================================================



void		TGUI_DrawLogo()
{
	_tgui_DrawFileCimgBackground(FNAME_LOGO);
}
//==============================================================================




// Forced repaint current screen
void		TGUI_ForceRepaint()
{
	if (tguiActiveScreen == (TG_SCREEN*)&tguiMsgBox)
		_tgui_MsgBoxPaint();
	else if (tguiActiveScreen->funcs._callpaint != NULL)
		tguiActiveScreen->funcs._callpaint(tguiActiveScreen, NULL);
}
//==============================================================================




// Current screen periodical process handling (check for changes, touch pressed, etc)
void		TGUI_Process()
{
	if (tguiActiveScreen == (TG_SCREEN*)&tguiMsgBox)
		_tgui_MsgBoxProcess();
	else if (tguiActiveScreen->funcs._process != NULL)
		tguiActiveScreen->funcs._process(tguiActiveScreen, NULL);
}
//==============================================================================




// Current screen periodical process handling (check for changes, touch pressed, etc)
void		TGUI_USBStateChanged()
{
	if (tguiScreenFiles.funcs._process != NULL && tguiActiveScreen != &tguiScreenFiles && tguiActiveScreen != &tguiScreenFilesList)
		tguiScreenFiles.funcs._process((void*)&tguiScreenFiles, NULL);
}
//==============================================================================




void		TGUI_MessageBoxOk(char *caption, char *text, pressfunc func_ok)
{
	_tgui_MsgBoxShow(MSGBOX_OK, caption, text, func_ok, NULL);
}
//==============================================================================




void		TGUI_MessageBoxWait(char *caption, char *text)
{
	_tgui_MsgBoxShow(MSGBOX_WAIT, caption, text, NULL, NULL);
}
//==============================================================================




void		TGUI_MessageBoxYesNo(char *caption, char *text, pressfunc func_ok, pressfunc func_cancel)
{
	_tgui_MsgBoxShow(MSGBOX_YESNO, caption, text, func_ok, func_cancel);
}
//==============================================================================




void		TGUI_MessageBoxOkCancel(char *caption, char *text, pressfunc func_ok, pressfunc func_cancel)
{
	_tgui_MsgBoxShow(MSGBOX_OKCANCEL, caption, text, func_ok, func_cancel);
}
//==============================================================================




