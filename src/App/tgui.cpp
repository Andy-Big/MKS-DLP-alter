/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "tgui.h"
#include "lcd_low.h"



TGUI_CONFIG		tguiDefaultConfig;

#define			TGUI_BTN_CNT_MAINSCREEN			3
TGUI_BUTTON		tguiMainScrButtons[TGUI_BTN_CNT_MAINSCREEN];
TGUI_SCREEN		tguiScreenMain;


TGUI_SCREEN		*tguiActiveScreen;




void		TGUI_Init()
{
	TGUI_SCREEN		*tgs;
	TGUI_CONFIG		*tgc;
	TGUI_BUTTON		*tgb;
	
	tguiActiveScreen = &tguiScreenMain;
	
	// DEFAULT CONFIG
	tgc = &tguiDefaultConfig;
	memset(tgc, 0, sizeof(TGUI_CONFIG));
	
	tgc->scrtextcolor = LCDUI_RGB(0x00FF00);
	tgc->scrbackcolor = LCDUI_RGB(0x000000);

	tgc->btntextcolor_en = LCDUI_RGB(0x00FF00);
	tgc->btntextcolor_dis = LCDUI_RGB(0x404040);
	tgc->btnbackcolor_en = LCDUI_RGB(0x002000);
	tgc->btnbackcolor_dis = LCDUI_RGB(0x101010);
	
	tgc->scrfont = LCDUI_FONT_H18;
	tgc->btnfont = LCDUI_FONT_H24BOLD;
	
	tgc->imagesdir = (char*)"tgui_images";
	
	
	
	// MAIN SCREEN BUTTON - PRINT
	tgb = &(tguiMainScrButtons[0]);
	memset(tgb, 0, sizeof(TGUI_BUTTON));
	
	tgb->position = {15, 100, 465, 165};

	tgb->bgimagename_en = NULL;
	tgb->bgimagename_press = NULL;
	tgb->bgimagename_dis = NULL;

	tgb->text = (char*)"ПЕЧАТЬ";
	tgb->font = tgc->btnfont;
	tgb->textcolor_en = tgc->btntextcolor_en;
	tgb->textcolor_press = tgc->btntextcolor_press;
	tgb->textcolor_dis = tgc->btntextcolor_dis;
	tgb->backcolor_en = tgc->btnbackcolor_en;
	tgb->backcolor_press = tgc->btnbackcolor_press;
	tgb->backcolor_dis = tgc->btnbackcolor_dis;
	
	tgb->options.disabled = 0;
	tgb->options.bgpainted = BGP_SCREEN;

	tgb->funcs._call_paint = NULL;
	tgb->funcs._call_press = NULL;
	tgb->parentscreen = &tguiScreenMain;

	
	
	// MAIN SCREEN
	tgs = &tguiScreenMain;
	memset(tgs, 0, sizeof(TGUI_SCREEN));
	
	tgs->bgimagename = (char*)"bgscreen_main.bmp";
	tgs->prevscreen = NULL;

	tgs->name = (char*)"ГЛАВНОЕ МЕНЮ";

	tgs->btns_count = TGUI_BTN_CNT_MAINSCREEN;
	tgs->buttons = tguiMainScrButtons;

	tgs->font = tgc->scrfont;
	tgs->textcolor = tgc->scrtextcolor;
	tgs->backcolor = tgc->scrbackcolor;

	tgs->funcs._callpaint = NULL;
	tgs->funcs._callpress = NULL;
}
//==============================================================================




