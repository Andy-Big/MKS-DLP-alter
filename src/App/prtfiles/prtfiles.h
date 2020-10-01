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
	FTYPE_PHOTON,
	FTYPE_CTB,
	
} FILES_TYPE;

typedef struct
{
	float		layer_position;
	float		light_time;
	float		lightoff_time;
	float		lift_height;
	float		lift_speed;
	float		drop_speed;

	uint32_t	data_offset;
	uint32_t	data_length;
} LAYER_INFO;





uint8_t		PFILE_Init(FIL *file, FILES_TYPE ftype);
uint8_t		PFILE_IsInited();
uint8_t		PFILE_SetPointerToPreview(uint8_t small = 0);
uint16_t	PFILE_GetPreviewWidth(uint8_t small = 0);
uint16_t	PFILE_GetPreviewHeight(uint8_t small = 0);
uint8_t		PFILE_GetPreviewBpp();
uint8_t		PFILE_DrawPreview(FIL *file, TG_RECT *rect, uint8_t small = 0);

uint32_t	PFILE_GetPrintTime();

uint32_t	PFILE_GetTotalLayers();
uint32_t	PFILE_GetBottomLayers();
float		PFILE_GetLayerThickness();
uint32_t	PFILE_GetAntialiasing();
float		PFILE_GetLightLayer();
void		PFILE_SetLightLayer(float val);
float		PFILE_GetLightBottom();
void		PFILE_SetLightBottom(float val);
float		PFILE_GetLightPause();
void		PFILE_SetLightPause(float val);
float		PFILE_GetLiftHeight();
void		PFILE_SetLiftHeight(float val);
float		PFILE_GetLiftBottom();
float		PFILE_GetLiftSpeed();
void		PFILE_SetLiftSpeed(float val);
float		PFILE_GetLiftSpeedBottom();
float		PFILE_GetDropSpeed();
void		PFILE_SetDropSpeed(float val);
float		PFILE_GetResinVolume();
uint32_t	PFILE_GetIndLayerSettings();
uint32_t	PFILE_GetIndLayerSettings();
uint32_t	PFILE_GetResolutionX();
uint32_t	PFILE_GetResolutionY();



#endif /*__prtfiles_H */

