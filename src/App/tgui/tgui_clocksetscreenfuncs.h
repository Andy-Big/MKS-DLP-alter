#ifndef __tgui_clocksetscreenfuncs_H
#define __tgui_clocksetscreenfuncs_H


#include "tgui.h"




void		TGUI_ClockSetScreenShow();

void		_tgui_ClockSetUpdateItem(uint32_t item_id);

void		_tgui_ClockSetScreenPaint(void *tguiobj, void *param);
void		_tgui_ClockSetItemPaint(void *tguiobj, void *param);

void		_tgui_ClockSetUpButtonPress(void *tguiobj, void *param);
void		_tgui_ClockSetDownButtonPress(void *tguiobj, void *param);
void		_tgui_ClockSetOkButtonPress(void *tguiobj, void *param);
void		_tgui_ClockSetClose(void *tguiobj, void *param);



#endif /*__tgui_clocksetscreenfuncs_H */

