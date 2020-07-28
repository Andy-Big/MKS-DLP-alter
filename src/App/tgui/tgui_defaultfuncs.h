#ifndef __tgui_defaultfuncs_H
#define __tgui_defaultfuncs_H


#include "tgui.h"



void		_tgui_DrawStringInRect(char *str, TGUI_RECT *rect, TGUI_TEXTOPTIONS *opt);
void		_tgui_DrawFileBmpBackground(char* file);
void		_tgui_DrawFileCimgBackground(char* file);

void		_tgui_DefaultButtonPaint(void *tguiobj, void *param);
void		_tgui_DefaultScreenPaint(void *tguiobj, void *param);
void		_tgui_DefaultScreenProcess(void *tguiobj, void *param);





#endif /*__tgui_defaultfuncs_H */

