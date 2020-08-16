#ifndef __tgui_messagebox_H
#define __tgui_messagebox_H


#include "tgui.h"


void		_tgui_MsgBoxShow(MSGBOXTYPE type, char *caption, char *text, pressfunc func_ok, pressfunc func_cancel);

void		_tgui_MsgBoxProcess();
void		_tgui_MsgBoxPaint();
void		_tgui_MsgBoxButtonPress(void *tguiobj, void *param);





#endif /*__tgui_messagebox_H */

