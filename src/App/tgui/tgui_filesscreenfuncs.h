#ifndef __tgui_filesscreenfuncs_H
#define __tgui_filesscreenfuncs_H


#include "tgui.h"
#include "ff.h"


typedef enum
{
	FTYPE_NONE = 0,
	FTYPE_DIR,
	FTYPE_PWS,
} FILES_TYPE;

typedef struct
{
	char		fname[256];
	FILES_TYPE	type;
} FILES_ITEM;



uint8_t		_tgui_FilesReadDir(char *fname);

void		_tgui_FilesScreenProcess(void *tguiobj, void *param);

void		_tgui_FilesScreenPaint(void *tguiobj, void *param);
void		_tgui_FilesFileButtonPaint(void *tguiobj, void *param);
void		_tgui_FilesDirNamePaint(void *tguiobj, void *param);

void		_tgui_FilesDownPress(void *tguiobj, void *param);





#endif /*__tgui_filesscreenfuncs_H */

