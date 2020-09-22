#ifndef __tgui_uvtestscreenfuncs_H
#define __tgui_uvtestscreenfuncs_H


#include "tgui.h"


void		_tgui_UVTestEnableLight();
void		_tgui_UVTestDisableLight();
void		_tgui_UVTestEnableDisp();
void		_tgui_UVTestDisableDisp();

void		_tgui_UVTestReadImage(uint8_t imgnum);

void		_tgui_UVTestBackButtonPress(void *tguiobj, void *param);
void		_tgui_UVTestImgSelectButtonPress(void *tguiobj, void *param);
void		_tgui_UVTestLightButtonPress(void *tguiobj, void *param);
void		_tgui_UVTestDispButtonPress(void *tguiobj, void *param);

void		_tgui_UVTestLightButtonPaint(void *tguiobj, void *param);
void		_tgui_UVTestDispButtonPaint(void *tguiobj, void *param);




#endif /*__tgui_uvtestscreenfuncs_H */

