#ifndef __tgui_printscreenfuncs_H
#define __tgui_printscreenfuncs_H


#include "tgui.h"
#include "prtfiles.h"
#include "files_pws.h"
#include "config.h"
#include "usb_host.h"
#include "ff.h"



void		TGUI_PrintScreenShow(void *tguiobj, void *param);
void		TGUI_PrintScreenExit(void *tguiobj, void *param);

void		_tgui_PrintScreenProcess(void *tguiobj, void *param);

void		_tgui_PrintScreenPausePress(void *tguiobj, void *param);
void		_tgui_PrintScreenStopPress(void *tguiobj, void *param);
void		_tgui_PrintScreenStopping(void *tguiobj, void *param);
void		_tgui_PrintScreenLockLPress(void *tguiobj, void *param);
void		_tgui_PrintScreenInfoPress(void *tguiobj, void *param);

void		_tgui_PrintScreenProgressPaint(void *tguiobj, void *param);
void		_tgui_PrintScreenPreviewPaint(void *tguiobj, void *param);
void		_tgui_PrintScreenProgressUpdate(void *tguiobj, void *param);
void		_tgui_PrintScreenLockPaint(void *tguiobj, void *param);





#endif /*__tgui_fileviewscreenfuncs_H */

