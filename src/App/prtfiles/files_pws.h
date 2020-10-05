#ifndef __files_pws_H
#define __files_pws_H

#include "includes.h"
#include "main.h"
#include "prtfiles.h"
#include "ff.h"
#include "tgui.h"



#pragma pack(1)
typedef struct
{
	char		mark[12];
	uint32_t	version;
	uint32_t	areanum;
	uint32_t	info_offset;
	uint32_t	undef_1;
	uint32_t	preview_offset;
	uint32_t	undef_2;
	uint32_t	layersdef_offset;
	uint32_t	undef_3;
	uint32_t	layersimg_offset;
} FPWS_HEADER;

typedef struct
{
	char		mark[12];
	uint32_t	header_length;
	float		pixel_size;
	float		layers_thickness;
	float		exp_time;
	float		lightoff_time;
	float		expbottom_time;
	float		bottom_layers;
	float		lift_height;
	float		lift_speed;
	float		down_speed;
	float		resin_volume;
	uint32_t	aa_grade;
	uint32_t	res_x;
	uint32_t	res_y;
	float		resin_weight;
	float		resin_price;
	uint32_t	resin_type;
	uint32_t	use_layer_params;
} FPWS_INFO;

typedef struct
{
	char		mark[12];
	uint32_t	data_length;
	uint32_t	width;
	uint32_t	resolution;
	uint32_t	height;
} FPWS_PREVIEW;

typedef struct
{
	char		mark[12];
	uint32_t	data_length;
	uint32_t	total_layers;
} FPWS_LAYERSDEF;

typedef struct
{
	uint32_t	data_point;
	uint32_t	data_length;
	float		lift_height;
	float		lift_speed;
	float		light_time;
	float		backup[3];
} FPWS_LAYERSINFO;

#pragma pack()




uint8_t		FPWS_ReadFileInfo(FIL *file);
uint8_t		FPWS_SetPointerToPreview(FIL *file);
uint32_t	FPWS_GetPreviewDataOffset();
uint32_t	FPWS_GetPreviewWidth();
uint32_t	FPWS_GetPreviewHeight();
uint32_t	FPWS_GetPreviewSize();
uint8_t		FPWS_DrawPreview(FIL *file, TG_RECT *rect);

uint32_t	FPWS_GetTotalLayers();
uint32_t	FPWS_GetBottomLayers();
float		FPWS_GetLayerThickness();
uint32_t	FPWS_GetAntialiasing();
float		FPWS_GetLightLayer();
void		FPWS_SetLightLayer(float val);
float		FPWS_GetLightBottom();
void		FPWS_SetLightBottom(float val);
float		FPWS_GetLightPause();
void		FPWS_SetLightPause(float val);
float		FPWS_GetLiftHeight();
void		FPWS_SetLiftHeight(float val);
float		FPWS_GetLiftSpeed();
void		FPWS_SetLiftSpeed(float val);
float		FPWS_GetDropSpeed();
void		FPWS_SetDropSpeed(float val);
float		FPWS_GetResinVolume();
uint32_t	FPWS_GetIndLayerSettings();
uint32_t	FPWS_GetResolutionX();
uint32_t	FPWS_GetResolutionY();

uint8_t		FPWS_GetLayerInfo(uint32_t layer_num, LAYER_INFO *layer_info);




#endif /*__files_pws_H */

