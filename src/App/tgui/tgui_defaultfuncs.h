#ifndef __tgui_defaultfuncs_H
#define __tgui_defaultfuncs_H


#include "tgui.h"


// functions for date-time on screen
void		_tgui_ScreenTimeInit();
void		_tgui_ScreenTimeProcess(void *tguiobj, void *param);
void		_tgui_ScreenTimePaint(void *tguiobj, void *param);

void		_tgui_DrawStringInRect(char *str, TG_RECT *rect, TG_TEXTOPTIONS *opt);
void		_tgui_DrawFileBmpBackground(char* file, uint8_t ui = 1);

void		_tgui_DrawFileRawImg(FIL *file, int16_t x, int16_t y, uint16_t bwidth, uint16_t bheight, uint32_t imgbase, uint8_t fliprows);
void		_tgui_DrawFileCimgBackground(char* file, uint8_t ui = 1);
void		_tgui_DrawFileCimg(char* file, int16_t x, int16_t y, uint8_t ui = 1);

void		_tgui_DefaultButtonPaint(void *tguiobj, void *param);
void		_tgui_DefaultButtonProcess(void *tguiobj, void *param);

void		_tgui_DefaultScreenPaint(void *tguiobj, void *param);
void		_tgui_DefaultScreenProcess(void *tguiobj, void *param);





#endif /*__tgui_defaultfuncs_H */

