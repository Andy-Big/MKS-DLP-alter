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
	FTYPE_CONFIG,
	FTYPE_IMAGE,
	
	FTYPE_PWS,
} FILES_TYPE;






uint8_t		PFILE_Init(FIL *file, FILES_TYPE ftype);
uint8_t		PFILE_IsInited();
uint8_t		PFILE_SetPointerToPreview();
uint16_t	PFILE_GetPreviewWidth();
uint16_t	PFILE_GetPreviewHeight();
uint8_t		PFILE_GetPreviewBpp();
uint8_t		PFILE_DrawPreview(FIL *file, TG_RECT *rect);

uint32_t	PFILE_GetPrintTime();

uint32_t	PFILE_GetTotalLayers();
uint32_t	PFILE_GetBottomLayers();
float		PFILE_GetLayerThickness();
uint32_t	PFILE_GetAntialiasing();
float		PFILE_GetLightLayer();
float		PFILE_GetLightBottom();
float		PFILE_GetLightPause();
float		PFILE_GetLiftHeight();
float		PFILE_GetLiftBottom();
float		PFILE_GetLiftSpeed();
float		PFILE_GetLiftSpeedBottom();
float		PFILE_GetDropSpeed();
float		PFILE_GetResinVolume();
uint32_t	PFILE_GetIndLayerSettings();



#endif /*__prtfiles_H */

