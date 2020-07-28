/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "tgui.h"
#include "tgui_defaultfuncs.h"

__no_init uint8_t 		tguiDBuff[UIDBUFF_SIZE];
__no_init uint8_t		tguiFBuff[UIFBUFF_SIZE];
__no_init FIL			tguiFile @ "CCMRAM";
__no_init TCHAR			tfname[512] @ "CCMRAM";

TGUI_CONFIG				tguiDefaultConfig;

TGUI_BUTTON				tguiMainScrButtons[TGUI_BTN_CNT_MAINSCREEN];
TGUI_SCREEN				tguiScreenMain;


TGUI_SCREEN				*tguiActiveScreen;




void		TGUI_Init()
{
	TGUI_SCREEN		*tgs;
	TGUI_CONFIG		*tgc;
	TGUI_BUTTON		*tgb;
	
	tguiActiveScreen = &tguiScreenMain;
	
	// DEFAULT CONFIG
	tgc = &tguiDefaultConfig;
	memset((void*)tgc, 0, sizeof(TGUI_CONFIG));
	
	tgc->scrnametextcolor = LCDUI_RGB(0xBAC5D5);
	tgc->scrtextcolor = LCDUI_RGB(0x00272E);
	tgc->scrbackcolor = LCDUI_RGB(0x648E62);
	tgc->scrfont = LCDUI_FONT_H18;
	tgc->scrnamefont = LCDUI_FONT_H24BOLD;

	tgc->btntextcolor_en = LCDUI_RGB(0x00272E);
	tgc->btntextcolor_press = LCDUI_RGB(0x8080FF);
	tgc->btntextcolor_dis = LCDUI_RGB(0x404040);
	tgc->btnbackcolor_en = LCDUI_RGB(0xA6BFCB);
	tgc->btnbackcolor_press = LCDUI_RGB(0xA6BFCB);
	tgc->btnbackcolor_dis = LCDUI_RGB(0xCBCBCB);
	tgc->btnfont = LCDUI_FONT_H24BOLD;
	
	
	// -------------------- Main Screen elements -----------------------
{
	// PRINT button
	tgb = &(tguiMainScrButtons[0]);
	memset((void*)tgb, 0, sizeof(TGUI_BUTTON));
	
	tgb->position = {15, 100, 465, 165};

	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	tgb->bgimagename_dis = NULL;

	tgb->text = (char*)"ПЕЧАТЬ";
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
	tgb->textoptions.textalign_h = HTA_CENTER;
	tgb->textoptions.textalign_v = VTA_CENTER;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = NULL;

	tgb->parentscreen = &tguiScreenMain;

	// INFO button
	tgb = &(tguiMainScrButtons[1]);
	memset((void*)tgb, 0, sizeof(TGUI_BUTTON));
	
	tgb->position = {245, 185, 465, 250};

	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	tgb->bgimagename_dis = NULL;

	tgb->text = (char*)"ИНФО";
	tgb->textposition = {320, 188, 460, 247};
	tgb->font = tgc->btnfont;
	tgb->textcolor_en = tgc->btntextcolor_en;
	tgb->textcolor_press = tgc->btntextcolor_press;
	tgb->textcolor_dis = tgc->btntextcolor_dis;
	tgb->backcolor_en = tgc->btnbackcolor_en;
	tgb->backcolor_press = tgc->btnbackcolor_press;
	tgb->backcolor_dis = tgc->btnbackcolor_dis;
	
	tgb->options.disabled = 0;
	tgb->options.bgpaint = BGP_NONE;
	tgb->textoptions.textalign_h = HTA_CENTER;
	tgb->textoptions.textalign_v = VTA_CENTER;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = NULL;

	tgb->parentscreen = &tguiScreenMain;

	// SERVICE button
	tgb = &(tguiMainScrButtons[2]);
	memset((void*)tgb, 0, sizeof(TGUI_BUTTON));
	
	tgb->position = {15, 185, 235, 250};

	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	tgb->bgimagename_dis = NULL;

	tgb->text = (char*)"СЕРВИС";
	tgb->textposition = {85, 188, 230, 247};
	tgb->font = tgc->btnfont;
	tgb->textcolor_en = tgc->btntextcolor_en;
	tgb->textcolor_press = tgc->btntextcolor_press;
	tgb->textcolor_dis = tgc->btntextcolor_dis;
	tgb->backcolor_en = tgc->btnbackcolor_en;
	tgb->backcolor_press = tgc->btnbackcolor_press;
	tgb->backcolor_dis = tgc->btnbackcolor_dis;
	
	tgb->options.disabled = 0;
	tgb->options.bgpaint = BGP_NONE;
	tgb->textoptions.textalign_h = HTA_CENTER;
	tgb->textoptions.textalign_v = VTA_CENTER;

	tgb->funcs._call_paint = _tgui_DefaultButtonPaint;
	tgb->funcs._call_press = NULL;

	tgb->parentscreen = &tguiScreenMain;

	
	
	// MAIN SCREEN
	tgs = &tguiScreenMain;
	memset((void*)tgs, 0, sizeof(TGUI_SCREEN));
	
	tgs->bgimagename = FNAME_BKGR_MAIN;
	tgs->prevscreen = NULL;

	tgs->name = (char*)"ГЛАВНОЕ МЕНЮ";
	tgs->nameposition = {205, 3, 475, 30};
	tgs->nameoptions.textalign_h = HTA_CENTER;
	tgs->nameoptions.textalign_v = VTA_CENTER;

	tgs->btns_count = TGUI_BTN_CNT_MAINSCREEN;
	tgs->buttons = tguiMainScrButtons;

	tgs->font = tgc->scrfont;
	tgs->namefont = tgc->scrnamefont;
	tgs->textcolor = tgc->scrtextcolor;
	tgs->nametextcolor = tgc->scrnametextcolor;
	tgs->backcolor = tgc->scrbackcolor;

	tgs->funcs._callpaint = _tgui_DefaultScreenPaint;
	tgs->funcs._callprocess = NULL;
	
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
	if (tguiActiveScreen->funcs._callpaint != NULL)
		tguiActiveScreen->funcs._callpaint(tguiActiveScreen, NULL);
}
//==============================================================================




// Current screen periodical process handling (check for changes, touch pressed, etc)
void		TGUI_Process()
{
	if (tguiActiveScreen->funcs._callprocess != NULL)
		tguiActiveScreen->funcs._callprocess(tguiActiveScreen, NULL);
}
//==============================================================================




