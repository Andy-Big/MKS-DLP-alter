#ifndef __tgui_H
#define __tgui_H


#include "includes.h"
#include "main.h"
#include "lcd_ui.h"
#include "fatfs.h"
#include "touch.h"
#include "languages.h"



typedef void (*paintfunc)(void *tguiobj, void* param);
typedef void (*processfunc)(void *tguiobj, void* param);
typedef void (*pressfunc)(void *tguiobj, void* param);


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
} TG_TEXTOPTIONS;


typedef enum
{
	BTNA_GOCHILDSCR = 0xFFFFFF00,
	BTNA_GOPREVSCR = 0xFFFFFF01,
	
} TG_BTNACTIONS;


typedef struct
{
	// screens properties
	uint16_t			scrnametextcolor;	// color for name text in header of screen
	uint16_t			scrtextcolor;
	uint16_t			scrbackcolor;
	LCDUI_FONT_TYPE		scrfont;
	LCDUI_FONT_TYPE		scrnamefont;		// fonr for name text in header of screen
	
	// buttons properties
	uint16_t			btntextcolor_en;
	uint16_t			btntextcolor_press;
	uint16_t			btntextcolor_dis;
	uint16_t			btntextcolor_act;
	uint16_t			btnbackcolor_en;
	uint16_t			btnbackcolor_press;
	uint16_t			btnbackcolor_dis;
	uint16_t			btnbackcolor_act;
	LCDUI_FONT_TYPE		btnfont;
	
	// message box properties
	LCDUI_FONT_TYPE		mb_text_font;
	LCDUI_FONT_TYPE		mb_capt_font;
	uint16_t			mb_text_color;
	uint16_t			mb_box_backcolor;
	uint16_t			mb_capt_textcolor;
	uint16_t			mb_capt_backcolor;
	
	char				*imagesdir;
} TG_CONFIG;



typedef struct
{
	uint16_t	x;
	uint16_t	y;
} TG_POINT;



typedef struct
{
	int16_t		left;
	int16_t		top;
	int16_t		right;
	int16_t		bottom;
} TG_RECT;



typedef struct
{
	void				*addparameter;
	
	uint8_t				button_id;
	

	int8_t				group_id;		// for swithed options buttons, >0 - single selection from group (select), <0 - multiple selection (switch)
	
	TG_RECT				position;
	
	void				*parentscreen;
	void				*childscreen;

	char				*bgimagename_en;
	char				*bgimagename_press;
	char				*bgimagename_dis;
	char				*bgimagename_act;	// for swithed options buttons

	LNG_STRING_ID		text;
	TG_RECT				textposition;
	LCDUI_FONT_TYPE		font;
	uint16_t			textcolor_en;
	uint16_t			textcolor_press;
	uint16_t			textcolor_dis;
	uint16_t			textcolor_act;	// for swithed options buttons
	uint16_t			backcolor_en;
	uint16_t			backcolor_press;
	uint16_t			backcolor_dis;
	uint16_t			backcolor_act;	// for swithed options buttons
	
	struct {
		uint8_t				active:1;		// for swithed options buttons
		uint8_t				needrepaint:1;
		uint8_t				pressed:1;
		uint8_t				disabled:1;
		uint8_t				repaintonpress:1;		// repaint or not when pressed - for indicate pressed state
		BGPAINT_TYPE		bgpaint:2;
	} options;
	
	TG_TEXTOPTIONS	textoptions;

	struct {
		paintfunc		_call_paint;	// repaint button
		pressfunc		_call_press;	// touch events handling
		processfunc		_call_process;	// periodical processing (for example text value refresh)
	} funcs;
} TG_BUTTON;



typedef struct
{
	void				*addparameter;

	char				*bgimagename;
	
	void				*prevscreen;
	
	LNG_STRING_ID		name;
	TG_RECT				nameposition;
	TG_TEXTOPTIONS		nameoptions;
	
	uint8_t				btns_count;
	TG_BUTTON			*buttons;
	
	LCDUI_FONT_TYPE		font;
	LCDUI_FONT_TYPE		namefont;
	uint16_t			textcolor;
	uint16_t			nametextcolor;
	uint16_t			backcolor;

	struct {
		paintfunc		_callpaint;	// repaint screen
		processfunc		_process;	// screen process handling (check for changes, touch pressed, etc)
	} funcs;
} TG_SCREEN;


typedef enum
{
	MSGBOX_OK = 0,
	MSGBOX_OKCANCEL,
	MSGBOX_YESNO,
	
} MSGBOXTYPE;



#define		TG_BTN_CNT_MSGBOX				2
typedef struct
{
	MSGBOXTYPE			type;
	
	void				*prevscreen;
	
	char				caption[128];
	char				text[512];
	TG_RECT				boxpos;
	
	uint8_t				btns_count;
	TG_BUTTON			buttons[TG_BTN_CNT_MSGBOX];
	
	uint16_t			caption_height;
	
	LCDUI_FONT_TYPE		font_caption;
	LCDUI_FONT_TYPE		font_text;
	uint16_t			text_color;
	uint16_t			box_backcolor;
	uint16_t			capt_textcolor;
	uint16_t			capt_backcolor;
} TG_MSGBOX;



#define	FNAME_LOGO					(char*)"logo.cimg"
#define	FNAME_BKGR_MAIN				(char*)"scr_main.cimg"
#define	FNAME_BKGR_SERVICE			(char*)"scr_service.cimg"
#define	FNAME_BKGR_LANGUAGE			(char*)"scr_language.cimg"
#define	FNAME_BKGR_INFO				(char*)"scr_info.cimg"
#define	FNAME_BKGR_MOVEZ			(char*)"scr_movez.cimg"
#define	FNAME_BKGR_FILES			(char*)"scr_files.cimg"
#define	FNAME_BKGR_FILESLIST		(char*)"scr_files_list.cimg"

#define	FNAME_BTN_MOVEZ_UP_EN		(char*)"btn_movez_up_en.cimg"
#define	FNAME_BTN_MOVEZ_UP_PRESS	(char*)"btn_movez_up_press.cimg"
#define	FNAME_BTN_MOVEZ_DN_EN		(char*)"btn_movez_down_en.cimg"
#define	FNAME_BTN_MOVEZ_DN_PRESS	(char*)"btn_movez_down_press.cimg"
#define	FNAME_BTN_MOVEZ_HOMERDY_EN	(char*)"btn_movez_home_ready.cimg"
#define	FNAME_BTN_MOVEZ_HOMENRDY_EN	(char*)"btn_movez_home_noready.cimg"

#define	FNAME_BTN_FILES_UP_DIS		(char*)"btn_files_up_dis.cimg"
#define	FNAME_BTN_FILES_UP_EN		(char*)"btn_files_up_en.cimg"
#define	FNAME_BTN_FILES_UP_PRESS	(char*)"btn_files_up_press.cimg"
#define	FNAME_BTN_FILES_DN_DIS		(char*)"btn_files_down_dis.cimg"
#define	FNAME_BTN_FILES_DN_EN		(char*)"btn_files_down_en.cimg"
#define	FNAME_BTN_FILES_DN_PRESS	(char*)"btn_files_down_press.cimg"
#define	FNAME_BTN_FILES_PREV_EN		(char*)"btn_files_prev_en.cimg"
#define	FNAME_BTN_FILES_PREV_DIS	(char*)"btn_files_prev_dis.cimg"
#define	FNAME_BTN_FILES_PREV_PRESS	(char*)"btn_files_prev_press.cimg"
#define	FNAME_BTN_FILES_LIST_EN		(char*)"btn_files_list_en.cimg"
#define	FNAME_BTN_FILES_LIST_DIS	(char*)"btn_files_list_dis.cimg"
#define	FNAME_BTN_FILES_LIST_PRESS	(char*)"btn_files_list_press.cimg"
#define	FNAME_BTN_FILES_ICONS_EN	(char*)"btn_files_icons_en.cimg"
#define	FNAME_BTN_FILES_ICONS_DIS	(char*)"btn_files_icons_dis.cimg"
#define	FNAME_BTN_FILES_ICONS_PRESS	(char*)"btn_files_icons_press.cimg"
#define	FNAME_ICN_FILES_DIRECTORY	(char*)"icn_files_dir.cimg"
#define	FNAME_BTN_MSGBOX_EN			(char*)"btn_msgbox_en.cimg"
#define	FNAME_BTN_MSGBOX_PRESS		(char*)"btn_msgbox_press.cimg"



#define UIDBUFF_SIZE		4096
extern uint8_t 			tguiDBuff[UIDBUFF_SIZE];
#define UIFBUFF_SIZE		512
extern uint8_t 			tguiFBuff[UIFBUFF_SIZE];

extern FIL				tguiFile @ "CCMRAM";
extern TCHAR			tfname[512] @ "CCMRAM";


extern TG_CONFIG		tguiDefaultConfig;



#define		TG_BTN_MSGBOX_YESOK_ID			10
#define		TG_BTN_MSGBOX_NOCANCEL_ID		11

extern TG_MSGBOX		tguiMsgBox;

#define		TG_BTN_CNT_SCREEN_MAIN			4
#define		TG_TIMEBUTTON_ID				1
extern TG_BUTTON		tguiScrMainButtons[TG_BTN_CNT_SCREEN_MAIN];
extern TG_SCREEN		tguiScreenMain;

#define		TG_BTN_CNT_SCREEN_SERVICE		3
extern TG_BUTTON		tguiScrServiceButtons[TG_BTN_CNT_SCREEN_SERVICE];
extern TG_SCREEN		tguiScreenService;

#define		TG_BTN_CNT_SCREEN_LANGUAGE		LNG_LANGS_COUNT + 1
extern TG_BUTTON		tguiScrLanguageButtons[TG_BTN_CNT_SCREEN_LANGUAGE];
extern TG_SCREEN		tguiScreenLanguage;

#define		TG_BTN_CNT_SCREEN_INFO			11
extern TG_BUTTON		tguiScrInfoButtons[TG_BTN_CNT_SCREEN_INFO];
extern TG_SCREEN		tguiScreenInfo;

#define		TG_SCR_MOVEZ_STEP_GROUP_ID		1
#define		TG_SCR_MOVEZ_10_ID				20
#define		TG_SCR_MOVEZ_1_ID				21
#define		TG_SCR_MOVEZ_02_ID				22
#define		TG_SCR_MOVEZ_005_ID				23
#define		TG_SCR_MOVEZ_HOME_ID			30

#define		TG_BTN_CNT_SCREEN_MOVEZ			10
extern TG_BUTTON		tguiScrMovezButtons[TG_BTN_CNT_SCREEN_MOVEZ];
extern TG_SCREEN		tguiScreenMovez;

#define		TG_SCR_FILES_DIR_ID				20
#define		TG_SCR_FILES_PREV_ID			21
#define		TG_SCR_FILES_UP_ID				22
#define		TG_SCR_FILES_DOWN_ID			23
#define		TG_SCR_FILES_LIST_ID			24

#define		TG_BTN_CNT_SCREEN_FILES			10
extern TG_BUTTON		tguiScrFilesButtons[TG_BTN_CNT_SCREEN_FILES];
extern TG_SCREEN		tguiScreenFiles;
#define		TG_BTN_CNT_SCREEN_FILESICONS	14
extern TG_BUTTON		tguiScrFilesListButtons[TG_BTN_CNT_SCREEN_FILESICONS];
extern TG_SCREEN		tguiScreenFilesList;




extern TG_SCREEN		*tguiActiveScreen;


uint8_t		TGUI_PointInRect(TOUCH_POINT *pt, TG_RECT *rc);


void		TGUI_Init();
//void		TGUI_ChangeLanguage(uint8_t lang);
void		TGUI_DrawLogo();
// Forced repaint current screen
void		TGUI_ForceRepaint();
// Current screen periodical process handling (check for changes, touch pressed, etc)
void		TGUI_Process();

void		TGUI_USBStateChanged();

void		TGUI_MessageBoxOk(char *caption, char *text);




#endif /*__tgui_H */

