#ifndef __tgui_prnparamsscreenfuncs_H
#define __tgui_prnparamsscreenfuncs_H


#include "tgui.h"



typedef struct
{
	float		bottom_light;
	float		light;
	float		lift_height;
	float		lift_speed;
	float		drop_speed;
	float		light_pause;
	uint32_t	antialiasing;
	uint32_t	ind_params;
} PRNPARAMS_VALUES;


void		_tgui_PrnparamsScreenShow(void *tguiobj, void *param);

void		_tgui_PrnparamsScreenPaint(void *tguiobj, void *param);
void		_tgui_PrnparamsItemButtonPaint(void *tguiobj, void *param);
void		_tgui_PrnparamsItemSolidButtonPaint(void *tguiobj, void *param);

void		_tgui_PrnparamsDownButtonPress(void *tguiobj, void *param);
void		_tgui_PrnparamsUpButtonPress(void *tguiobj, void *param);

void		_tgui_PrnparamsItemButtonPress(void *tguiobj, void *param);

void		_tgui_PrnparamsOkButtonPress(void *tguiobj, void *param);

void		_tgui_PrnparamsSaveButtonPress(void *tguiobj, void *param);





#endif /*__tgui_settingsscreenfuncs_H */

