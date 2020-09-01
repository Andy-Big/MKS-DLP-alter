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
#include "tgui_filesscreenfuncs.h"
#include "tgui_movezscreenfuncs.h"
#include "tgui_messagebox.h"
#include "tgui_settingsscreenfuncs.h"
#include "tgui_numenterscreenfuncs.h"


__no_init uint8_t 		tguiDBuff[UIDBUFF_SIZE];
__no_init uint8_t		tguiFBuff[UIFBUFF_SIZE];
__no_init FIL			tguiFile @ "CCMRAM";
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
	int16_t			bx, by, bw, bh;
	
	
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

	// LANGUAGE button
	tgb = &(tguiScrServiceButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrServiceButtons[0]), sizeof(TG_BUTTON));
	
	tgb->position = {15, 65, 235, 130};

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->text = LSTR_LANGUAGE;
	tgb->textposition = {80, 67, 230, 128};

	tgb->funcs._call_press = (pressfunc)BTNA_GOCHILDSCR;

	tgb->childscreen = &tguiScreenLanguage;
	
	
	// MOVEZ button
	tgb = &(tguiScrServiceButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrServiceButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {245, 65, 464, 130};

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->text = LSTR_MOVEZ;
	tgb->textposition = {316, 67, 462, 128};

	tgb->funcs._call_press = (pressfunc)BTNA_GOCHILDSCR;

	tgb->childscreen = &tguiScreenMovez;
	
	// UVTEST button
	tgb = &(tguiScrServiceButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrServiceButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {15, 139, 234, 204};

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->text = LSTR_UVTEST;
	tgb->textposition = {80, 142, 230, 201};

	tgb->funcs._call_press = NULL;

	tgb->childscreen = NULL;
	
	// SETTINGS button
	tgb = &(tguiScrServiceButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrServiceButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {245, 139, 464, 204};

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->text = LSTR_SETTINGS_SHORT;
	tgb->textposition = {316, 142, 462, 201};

	tgb->funcs._call_press = (pressfunc)BTNA_GOCHILDSCR;

	tgb->childscreen = &tguiScreenSettings;
	
	
	// SERVICE SCREEN
	tgs = &tguiScreenService;
	memset((void*)tgs, 0, sizeof(TG_SCREEN));
	
	tgs->bgimagename = FNAME_BKGR_SERVICE;
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

	
	bx = 25;
	by = 65;
	bw = 50;
	bh = 30;
	// VERSION_TEXT button
	tgb = &(tguiScrInfoButtons[bi++]);
	memset((void*)tgb, 0, sizeof(TG_BUTTON));
	
	tgb->position = {bx, by, (int16_t)(bx+bw), (int16_t)(by+bh)};

	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	tgb->bgimagename_dis = NULL;

	tgb->text = LSTR_VERSION;
	tgb->textposition = {(int16_t)(bx+10), by, (int16_t)(bx+bw), (int16_t)(by+bh)};
	tgb->font = LCDUI_FONT_H24;
	tgb->textcolor_en = LCDUI_RGB(0x00272E);
	tgb->textcolor_press = tgc->btntextcolor_press;
	tgb->textcolor_dis = tgc->btntextcolor_dis;
	tgb->backcolor_en = tgc->btnbackcolor_en;
	tgb->backcolor_press = tgc->btnbackcolor_press;
	tgb->backcolor_dis = tgc->btnbackcolor_dis;
	
	tgb->options.disabled = 0;
	tgb->options.bgpaint = BGP_NONE;
	tgb->options.repaintonpress = 0;
	
	tgb->textoptions.textalign_h = HTA_LEFT;
	tgb->textoptions.textalign_v = VTA_CENTER;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = NULL;

	tgb->parentscreen = &tguiScreenInfo;
	tgb->childscreen = NULL;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	by += bh;

	// LIGHTTIME_TEXT button
	tgb = &(tguiScrInfoButtons[bi++]);
	memcpy((void*)tgb, &(tguiScrInfoButtons[bi-2]), sizeof(TG_BUTTON));

	tgb->position = {bx, by, (int16_t)(bx+bw), (int16_t)(by+bh)};

	tgb->text = LSTR_LIGHTTIME;
	tgb->textposition = {(int16_t)(bx+10), by, (int16_t)(bx+bw), (int16_t)(by+bh)};
	by += bh;

	// FANSTIME_TEXT button
	tgb = &(tguiScrInfoButtons[bi++]);
	memcpy((void*)tgb, &(tguiScrInfoButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {bx, by, (int16_t)(bx+bw), (int16_t)(by+bh)};

	tgb->text = LSTR_FANSTIME;
	tgb->textposition = {(int16_t)(bx+10), by, (int16_t)(bx+bw), (int16_t)(by+bh)};
	by += bh;

	// URL_TEXT button
	tgb = &(tguiScrInfoButtons[bi++]);
	memcpy((void*)tgb, &(tguiScrInfoButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {bx, by, (int16_t)(bx+bw), (int16_t)(by+bh)};

	tgb->text = LSTR_URL;
	tgb->textposition = {(int16_t)(bx+10), by, (int16_t)(bx+bw), (int16_t)(by+bh)};
	by += bh;

	// UIAUTHOR_TEXT button
	tgb = &(tguiScrInfoButtons[bi++]);
	memcpy((void*)tgb, &(tguiScrInfoButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {bx, by, (int16_t)(bx+bw), (int16_t)(by+bh)};

	tgb->text = LSTR_UIAUTHOR;
	tgb->textposition = {(int16_t)(bx+10), by, (int16_t)(bx+bw), (int16_t)(by+bh)};

	bx = 76;
	by = 65;
	bw = 380;
	// VERSION_VAL button
	tgb = &(tguiScrInfoButtons[bi++]);
	memcpy((void*)tgb, &(tguiScrInfoButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {bx, by, (int16_t)(bx+bw), (int16_t)(by+bh)};

	tgb->text = LSTR____;
	tgb->textposition = {bx, by, (int16_t)(bx+bw-10), (int16_t)(by+bh)};
	tgb->textoptions.textalign_h = HTA_RIGHT;
	tgb->textcolor_en = LCDUI_RGB(0x0060A0);

	tgb->funcs._call_paint = _tgui_InfoScreenVersionPaint;
	by += bh;

	// LIGHTTIME_VAL button
	tgb = &(tguiScrInfoButtons[bi++]);
	memcpy((void*)tgb, &(tguiScrInfoButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {bx, by, (int16_t)(bx+bw), (int16_t)(by+bh)};

	tgb->text = LSTR____;
	tgb->textposition = {bx, by, (int16_t)(bx+bw-10), (int16_t)(by+bh)};

	tgb->funcs._call_paint = _tgui_InfoScreenLightTimePaint;
	by += bh;

	// FANSTIME_VAL button
	tgb = &(tguiScrInfoButtons[bi++]);
	memcpy((void*)tgb, &(tguiScrInfoButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {bx, by, (int16_t)(bx+bw), (int16_t)(by+bh)};

	tgb->text = LSTR____;
	tgb->textposition = {bx, by, (int16_t)(bx+bw-10), (int16_t)(by+bh)};

	tgb->funcs._call_paint = _tgui_InfoScreenFansTimePaint;
	by += bh;

	// URL_VAL button
	by += 4;
	tgb = &(tguiScrInfoButtons[bi++]);
	memcpy((void*)tgb, &(tguiScrInfoButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {bx, by, (int16_t)(bx+bw), (int16_t)(by+bh)};

	tgb->text = LSTR____;
	tgb->textposition = {bx, by, (int16_t)(bx+bw-10), (int16_t)(by+bh)};
	tgb->font = LCDUI_FONT_H18;

	tgb->funcs._call_paint = _tgui_InfoScreenURLPaint;
	by += bh;

	// UIAUTHOR_VAL button
	tgb = &(tguiScrInfoButtons[bi++]);
	memcpy((void*)tgb, &(tguiScrInfoButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {bx, by, (int16_t)(bx+bw), (int16_t)(by+bh)};

	tgb->text = LSTR____;
	tgb->textposition = {bx, by, (int16_t)(bx+bw-10), (int16_t)(by+bh)};

	tgb->funcs._call_paint = _tgui_InfoScreenUIAuthorPaint;


	
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

	// 10mm button
	tgb = &(tguiScrMovezButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrMovezButtons[0]), sizeof(TG_BUTTON));
	
	tgb->group_id = TG_SCR_MOVEZ_STEP_GROUP_ID;
	tgb->button_id = TG_SCR_MOVEZ_10_ID;
	
	tgb->position = {16, 65, 115, 114};

	tgb->textcolor_en = tgc->btntextcolor_en;
	tgb->textcolor_act = LCDUI_RGB(0xC80000);

	tgb->text = LSTR_10MM;
	tgb->textposition = {16, 65, 115, 114};

	tgb->funcs._call_press = _tgui_MovezStepSelectButtonPress;

	tgb->childscreen = NULL;

	// 1mm button
	tgb = &(tguiScrMovezButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrMovezButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_MOVEZ_1_ID;

	tgb->position = {132, 65, 231, 114};

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->text = LSTR_1MM;
	tgb->textposition = {132, 65, 231, 114};

	tgb->funcs._call_press = _tgui_MovezStepSelectButtonPress;

	tgb->childscreen = NULL;

	// 0.2mm button
	tgb = &(tguiScrMovezButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrMovezButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_MOVEZ_02_ID;
	tgb->options.active = 1;

	tgb->position = {248, 65, 347, 114};

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->text = LSTR_02MM;
	tgb->textposition = {248, 65, 347, 114};

	tgb->funcs._call_press = _tgui_MovezStepSelectButtonPress;

	tgb->childscreen = NULL;

	// 0.05mm button
	tgb = &(tguiScrMovezButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrMovezButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->button_id = TG_SCR_MOVEZ_005_ID;
	tgb->options.active = 0;

	tgb->position = {364, 65, 463, 114};

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->text = LSTR_005MM;
	tgb->textposition = {364, 65, 463, 114};

	tgb->funcs._call_press = _tgui_MovezStepSelectButtonPress;

	tgb->childscreen = NULL;

	// UP button
	tgb = &(tguiScrMovezButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrMovezButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->group_id = 0;
	tgb->position = {364, 125, 463, 204};

	tgb->textcolor_en = tgc->btntextcolor_en;

	tgb->text = LSTR____;
	tgb->textposition = {364, 125, 463, 204};
	
	tgb->options.bgpaint = BGP_IMAGE;
	tgb->bgimagename_en = FNAME_BTN_MOVEZ_UP_EN;
	tgb->bgimagename_press = FNAME_BTN_MOVEZ_UP_PRESS;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = _tgui_MovezUpButtonPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	
	// DOWN button
	tgb = &(tguiScrMovezButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrMovezButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {364, 230, 463, 309};

	tgb->textposition = {364, 230, 463, 309};
	
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
	tgb->childscreen = NULL;
	
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
		tgb->childscreen = NULL;
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

	// Item1 button - Lift on pause
	tgb = &(tguiScrSettingsButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrSettingsButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {8, 57, 387, 104};

	tgb->button_id = TG_SCR_SETTINGS_LIFTPAUSE_ID;

	tgb->textcolor_en = tgc->btntextcolor_en;
	tgb->textcolor_act = tgc->btntextcolor_en;
	tgb->textposition = {13, 57, 382, 104};
	tgb->text = LSTR_LIFT_ON_PAUSE;

	tgb->options.bgpaint = BGP_IMAGE;
	tgb->bgimagename_en = FNAME_BTN_SETTINGS_ITEM_EN;
	tgb->bgimagename_press = FNAME_BTN_SETTINGS_ITEM_PRESS;

	tgb->options.disabled = 0;

	tgb->funcs._call_paint = _tgui_SettingsItemButtonPaint;
	tgb->funcs._call_press = _tgui_SettingsPauseliftButtonPress;
	tgb->funcs._call_process = _tgui_DefaultButtonProcess;
	
	
	// Item2 button - Buzzer
	tgb = &(tguiScrSettingsButtons[bi++]);
	memcpy((void*)tgb, (void*)(&tguiScrSettingsButtons[bi-2]), sizeof(TG_BUTTON));
	
	tgb->position = {8, 108, 387, 155};

	tgb->button_id = TG_SCR_SETTINGS_BUZZER_ID;

	tgb->textcolor_en = tgc->btntextcolor_en;
	tgb->textcolor_act = tgc->btntextcolor_en;
	tgb->textposition = {13, 108, 382, 155};
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

	tgb->text = LCTR_AC;
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
	tgb->funcs._call_press = NULL;
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
	if (tguiScreenFiles.funcs._process != NULL && tguiActiveScreen != &tguiScreenFiles)
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




