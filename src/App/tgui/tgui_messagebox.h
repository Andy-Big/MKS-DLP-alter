#ifndef __tgui_messagebox_H
#define __tgui_messagebox_H


#include "tgui.h"


void		_tgui_ShowMessageBox(MSGBOXTYPE type, char *caption, char *text, pressfunc func_ok, pressfunc func_cancel);

void		_tgui_MessageBoxButtonPress(void *tguiobj, void *param);





#endif /*__tgui_messagebox_H */

