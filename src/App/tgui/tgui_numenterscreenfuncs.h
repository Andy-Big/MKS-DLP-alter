#ifndef __tgui_numenterscreenfuncs_H
#define __tgui_numenterscreenfuncs_H


#include "tgui.h"



typedef enum
{
	NT_UINT = 0,
	NT_INT,
	NT_UFLOAT,
	NT_FLOAT
} NUMENTER_TYPEVAL;



void		TGUI_NumenterScreenShow(char *name, NUMENTER_TYPEVAL type, void *value);

void		_tgui_NumenterButtonPaint(void *tguiobj, void *param);
void		_tgui_NumenterOKPress(void *tguiobj, void *param);
void		_tgui_NumenterDigiScreenPaint(void *tguiobj, void *param);
void		_tgui_NumenterNumbersPress(void *tguiobj, void *param);
void		_tgui_NumenterDelPress(void *tguiobj, void *param);
void		_tgui_NumenterDotPress(void *tguiobj, void *param);
void		_tgui_NumenterACPress(void *tguiobj, void *param);





#endif /*__tgui_numenterscreenfuncs_H */

