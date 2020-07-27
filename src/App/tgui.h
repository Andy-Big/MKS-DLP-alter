#ifndef __tgui_H
#define __tgui_H


#include "includes.h"
#include "main.h"

#include "lcd_ui.h"
#include "fatfs.h"




typedef void (*paintfunc)(void *tuiobj, void* param);
typedef void (*pressfunc)(void *tuiobj, void* *pt);



typedef enum
{
	BGP_NONE = 0,
	BGP_SCREEN,
	BGP_FILL,
} BGPAINT_TYPE;



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
	uint16_t	x_left;
	uint16_t	y_top;
	uint16_t	x_right;
	uint16_t	y_bottom;
} TGUI_RECT;



typedef struct
{
	TGUI_RECT			position;
	
	char				*bgimagename_en;
	char				*bgimagename_press;
	char				*bgimagename_dis;

	char				*text;
	LCDUI_FONT_TYPE		font;
	uint16_t			textcolor_en;
	uint16_t			textcolor_press;
	uint16_t			textcolor_dis;
	uint16_t			backcolor_en;
	uint16_t			backcolor_press;
	uint16_t			backcolor_dis;
	
	struct {
		uint8_t				disabled:1;
		BGPAINT_TYPE		bgpainted:3;
	} options;

	struct {
		paintfunc		*_call_paint;
		pressfunc		*_call_press;
	} funcs;
	
	void				*parentscreen;
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
		paintfunc		*_callpaint;
		pressfunc		*_callpress;
	} funcs;
} TGUI_SCREEN;


	 


#endif /*__tgui_H */

