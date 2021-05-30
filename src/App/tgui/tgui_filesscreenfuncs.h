#ifndef __tgui_filesscreenfuncs_H
#define __tgui_filesscreenfuncs_H


#include "tgui.h"
#include "ff.h"
#include "prtfiles.h"


typedef struct
{
	char			fname[256];
	FILES_TYPE		type;
	uint16_t		date;
	uint16_t		time;
} FILES_ITEM;



void		_tgui_FilesDrawPreview(TG_RECT *rect, FILES_ITEM *fitem);

void		_tgui_FilesReinit();

uint8_t		_tgui_FilesReadDir();

void		_tgui_FilesScreenProcess(void *tguiobj, void *param);

void		_tgui_FilesScreenPaint(void *tguiobj, void *param);
void		_tgui_FilesFileButtonPaint(void *tguiobj, void *param);
void		_tgui_FilesListFileButtonPaint(void *tguiobj, void *param);
void		_tgui_FilesDirNamePaint(void *tguiobj, void *param);

void		_tgui_FilesDownButtonPress(void *tguiobj, void *param);
void		_tgui_FilesUpButtonPress(void *tguiobj, void *param);
void		_tgui_FilesBackButtonPress(void *tguiobj, void *param);
void		_tgui_FilesPrevButtonPress(void *tguiobj, void *param);
void		_tgui_FilesListButtonPress(void *tguiobj, void *param);

void		_tgui_FilesFileButtonPress(void *tguiobj, void *param);





#endif /*__tgui_filesscreenfuncs_H */

