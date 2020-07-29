#ifndef __tgui_H
#define __tgui_H


#include "includes.h"
#include "main.h"
#include "lcd_ui.h"
#include "fatfs.h"
#include "touch.h"



typedef void (*paintfunc)(void *tguiobj, void* param);
typedef void (*processfunc)(void *tguiobj, void* param);
typedef void (*pressfunc)(void *tguiobj, void* *pt);


// Method of background repainting for buttons and texts
typedef enum
{
	BGP_NONE = 0,		// backgrount not paints
	BGP_SCREEN = 1,		// background paint from parent background screen
	BGP_FILL = 2,		// backgriunt filled with background color
	BGP_IMAGE = 3,		// backgriunt paint from self image file
} BGPAINT_TYPE;


// Text align in buttons and texts
typedef enum
{
	// horizontal
	HTA_LEFT = 0,
	HTA_CENTER = 1,
	HTA_RIGHT = 2,
} TEXT_ALIGN_H;


typedef enum
{
	// vertical
	VTA_TOP = 0,
	VTA_CENTER = 1,
	VTA_BOTTOM = 2,
} TEXT_ALIGN_V;


typedef struct
{
	TEXT_ALIGN_H	textalign_h:4;
	TEXT_ALIGN_V	textalign_v:4;
} TGUI_TEXTOPTIONS;


typedef enum
{
	BTNA_GOCHILDSCR = 0xFFFFFF00,
	BTNA_GOPREVSCR = 0xFFFFFF01,
	
} TGUI_BTNACTIONS;

typedef struct
{
	uint16_t			scrnametextcolor;
	uint16_t			scrtextcolor;
	uint16_t			scrbackcolor;
	LCDUI_FONT_TYPE		scrfont;
	LCDUI_FONT_TYPE		scrnamefont;
	

	uint16_t			btntextcolor_en;
	uint16_t			btntextcolor_press;
	uint16_t			btntextcolor_dis;
	uint16_t			btnbackcolor_en;
	uint16_t			btnbackcolor_press;
	uint16_t			btnbackcolor_dis;
	LCDUI_FONT_TYPE		btnfont;
	
	char				*imagesdir;
} TGUI_CONFIG;



typedef struct
{
	uint16_t	x;
	uint16_t	y;
} TGUI_POINT;



typedef struct
{
	uint16_t	left;
	uint16_t	top;
	uint16_t	right;
	uint16_t	bottom;
} TGUI_RECT;



typedef struct
{
	void				*addparameter;
	
	TGUI_RECT			position;
	
	void				*parentscreen;
	void				*childscreen;

	char				*bgimagename_en;
	char				*bgimagename_press;
	char				*bgimagename_dis;

	char				*text;
	TGUI_RECT			textposition;
	LCDUI_FONT_TYPE		font;
	uint16_t			textcolor_en;
	uint16_t			textcolor_press;
	uint16_t			textcolor_dis;
	uint16_t			backcolor_en;
	uint16_t			backcolor_press;
	uint16_t			backcolor_dis;
	
	struct {
		uint8_t				pressed:1;
		uint8_t				disabled:1;
		uint8_t				repaintonpress:1;		// repaint or not when pressed - for indicate pressed state
		BGPAINT_TYPE		bgpaint:2;
	} options;
	
	TGUI_TEXTOPTIONS	textoptions;

	struct {
		paintfunc		_call_paint;	// repaint button
		pressfunc		_call_press;	// touch events handling
	} funcs;
} TGUI_BUTTON;



typedef struct
{
	char				*bgimagename;
	
	void				*prevscreen;
	
	char				*name;
	TGUI_RECT			nameposition;
	TGUI_TEXTOPTIONS	nameoptions;
	
	uint8_t				btns_count;
	TGUI_BUTTON			*buttons;
	
	LCDUI_FONT_TYPE		font;
	LCDUI_FONT_TYPE		namefont;
	uint16_t			textcolor;
	uint16_t			nametextcolor;
	uint16_t			backcolor;

	struct {
		paintfunc		_callpaint;	// repaint screen
		processfunc		_process;	// screen process handling (check for changes, touch pressed, etc)
	} funcs;
} TGUI_SCREEN;



#define	FNAME_LOGO				(char*)"logo.cimg"
#define	FNAME_BKGR_MAIN			(char*)"scr_main.cimg"
#define	FNAME_BKGR_SERVICE		(char*)"scr_service.cimg"


#define UIDBUFF_SIZE		4096
extern uint8_t 			tguiDBuff[UIDBUFF_SIZE];
#define UIFBUFF_SIZE		512
extern uint8_t 			tguiFBuff[UIFBUFF_SIZE];

extern FIL				tguiFile @ "CCMRAM";
extern TCHAR			tfname[512] @ "CCMRAM";


extern TGUI_CONFIG		tguiDefaultConfig;

#define		TGUI_BTN_CNT_MAINSCREEN			3
extern TGUI_BUTTON		tguiScrButtonsMain[TGUI_BTN_CNT_MAINSCREEN];
extern TGUI_SCREEN		tguiScreenMain;

#define		TGUI_BTN_CNT_SERVICESCREEN		4
extern TGUI_BUTTON		tguiScrButtonsService[TGUI_BTN_CNT_SERVICESCREEN];
extern TGUI_SCREEN		tguiScreenService;


extern TGUI_SCREEN		*tguiActiveScreen;


uint8_t		TGUI_PointInRect(TOUCH_POINT *pt, TGUI_RECT *rc);


void		TGUI_Init();
void		TGUI_DrawLogo();
// Forced repaint current screen
void		TGUI_ForceRepaint();
// Current screen periodical process handling (check for changes, touch pressed, etc)
void		TGUI_Process();





#endif /*__tgui_H */

