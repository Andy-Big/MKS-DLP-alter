#ifndef __tgui_fileviewscreenfuncs_H
#define __tgui_fileviewscreenfuncs_H


#include "tgui.h"
#include "prtfiles.h"
#include "files_pws.h"



void		TGUI_FileviewScreenShow(char *fname, char *dname, FILES_TYPE ftype);

void		_tgui_FileviewFileinfoPaint(void *tguiobj, void *param);
void		_tgui_FileviewPrintinfoPaint(void *tguiobj, void *param);
void		_tgui_FileviewPreviewPaint(void *tguiobj, void *param);
/*
void		_tgui_NumenterOKPress(void *tguiobj, void *param);
void		_tgui_NumenterDigiScreenPaint(void *tguiobj, void *param);
void		_tgui_NumenterNumbersPress(void *tguiobj, void *param);
void		_tgui_NumenterDelPress(void *tguiobj, void *param);
void		_tgui_NumenterDotPress(void *tguiobj, void *param);
void		_tgui_NumenterACPress(void *tguiobj, void *param);
*/




#endif /*__tgui_fileviewscreenfuncs_H */

