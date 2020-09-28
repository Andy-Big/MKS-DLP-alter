#ifndef __tgui_fileviewscreenfuncs_H
#define __tgui_fileviewscreenfuncs_H


#include "tgui.h"
#include "prtfiles.h"
#include "files_pws.h"



void		TGUI_FileviewScreenShow(char *fname, char *dname, FILES_TYPE ftype);

void		_tgui_FileviewFileinfoPaint(void *tguiobj, void *param);
void		_tgui_FileviewPrintinfoPaint(void *tguiobj, void *param);
void		_tgui_FileviewPreviewPaint(void *tguiobj, void *param);

void		_tgui_FileviewPrintPress(void *tguiobj, void *param);
void		_tgui_FileviewPrintBegin(void *tguiobj, void *param);
void		_tgui_FileviewDeletePress(void *tguiobj, void *param);
void		_tgui_FileviewDeleteFile(void *tguiobj, void *param);
void		_tgui_FileviewFileDeleted(void *tguiobj, void *param);




#endif /*__tgui_fileviewscreenfuncs_H */

