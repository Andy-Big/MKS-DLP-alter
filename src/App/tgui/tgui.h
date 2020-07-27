#ifndef __tgui_H
#define __tgui_H


#include "includes.h"
#include "main.h"
#include "lcd_ui.h"
#include "fatfs.h"



typedef void (*paintfunc)(void *tguiobj, void* param);
typedef void (*pressfunc)(void *tguiobj, void* *pt);


// Method of background repainting for buttons and texts
typedef enum
{
	BGP_NONE = 0,	// backgrount not paints
	BGP_SCREEN = 1,		// background paint from parent background screen
	BGP_FILL = 2,		// backgriunt filled with background color
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
	uint16_t			scrtextcolor;
	uint16_t			scrbackcolor;

	uint16_t			btntextcolor_en;
	uint16_t			btntextcolor_press;
	uint16_t			btntextcolor_dis;
	uint16_t			btnbackcolor_en;
	uint16_t			btnbackcolor_press;
	uint16_t			btnbackcolor_dis;
	
	LCDUI_FONT_TYPE		scrfont;
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
	TGUI_RECT			position;
	
	void				*parentscreen;

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
		uint8_t				disabled:1;
		uint8_t				repaintonpress:1;		// repaint or not when pressed - for indicate pressed state
		BGPAINT_TYPE		bgpainted:2;
		uint8_t				textalign_h:2;
		uint8_t				textalign_v:2;
	} options;

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
	
	uint8_t				btns_count;
	TGUI_BUTTON			*buttons;
	
	LCDUI_FONT_TYPE		font;
	uint16_t			textcolor;
	uint16_t			backcolor;

	struct {
		paintfunc		_callpaint;	// repaint screen
		pressfunc		_callpress;	// touch events handling
	} funcs;
} TGUI_SCREEN;





extern TGUI_CONFIG		tguiDefaultConfig;

#define		TGUI_BTN_CNT_MAINSCREEN			3
extern TGUI_BUTTON		tguiMainScrButtons[TGUI_BTN_CNT_MAINSCREEN];
extern TGUI_SCREEN		tguiScreenMain;


extern TGUI_SCREEN		*tguiActiveScreen;



void		TGUI_Init();






#endif /*__tgui_H */

