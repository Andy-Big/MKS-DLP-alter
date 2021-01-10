#ifndef __tgui_prnparamsextscreenfuncs_H
#define __tgui_prnparamsextscreenfuncs_H


#include "tgui.h"



typedef struct
{
	float		pause_lift;
	float		end_lift;
	float		accelerate;
} PRNPARAMSEXT_VALUES;


void		_tgui_PrnparamsExtScreenShow(void *tguiobj, void *param);

void		_tgui_PrnparamsExtScreenPaint(void *tguiobj, void *param);
void		_tgui_PrnparamsExtItemButtonPaint(void *tguiobj, void *param);
void		_tgui_PrnparamsExtItemSolidButtonPaint(void *tguiobj, void *param);

void		_tgui_PrnparamsExtDownButtonPress(void *tguiobj, void *param);
void		_tgui_PrnparamsExtUpButtonPress(void *tguiobj, void *param);

void		_tgui_PrnparamsExtItemButtonPress(void *tguiobj, void *param);

void		_tgui_PrnparamsExtOkButtonPress(void *tguiobj, void *param);

void		_tgui_PrnparamsExtSaveButtonPress(void *tguiobj, void *param);





#endif /*__tgui_prnparamsextscreenfuncs_H */

