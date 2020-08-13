#ifndef __prtfiles_H
#define __prtfiles_H

#include "includes.h"
#include "main.h"
#include "ff.h"
#include "tgui.h"




typedef enum
{
	FTYPE_NONE = 0,
	FTYPE_DIR,
	FTYPE_PWS,
} FILES_TYPE;






uint8_t		PFILE_Init(FIL *file, FILES_TYPE ftype);
uint8_t		PFILE_SetPointerToPreview();
uint16_t	PFILE_GetPreviewWidth();
uint16_t	PFILE_GetPreviewHeight();
uint8_t		PFILE_GetPreviewBpp();
uint8_t		PFILE_DrawPreview(FIL *file, TG_RECT *rect);




#endif /*__prtfiles_H */

