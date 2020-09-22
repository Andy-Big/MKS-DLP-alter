#ifndef __tgui_screensaverfuncs_H
#define __tgui_screensaverfuncs_H


#include "tgui.h"




void		TGUI_ScreenSaverShow();

void		_tgui_ScreenSaverPaint(void *tguiobj, void *param);
void		_tgui_ScreenSaverTimePaint();
void		_tgui_ScreenSaverDatePaint();
void		_tgui_ScreenSaverProgressPaint();
void		_tgui_ScreenSaverProcess(void *tguiobj, void *param);


#endif /*__tgui_screensaverfuncs_H */

