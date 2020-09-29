#ifndef __files_photon_H
#define __files_photon_H

#include "includes.h"
#include "main.h"
#include "ff.h"
#include "tgui.h"



#pragma pack(1)
typedef struct
{
	uint32_t	header;		// 0x12FD0019
	uint32_t	version;
	float		bed_x_mm;
	float		bed_y_mm;
	float		bed_z_mm;
	float		unknow1[2];
	float		model_height_mm;
	float		slicer_layers_thickness;
	float		slicer_exp_time;
	float		slicer_expbottom_time;
	float		slicer_lightoff_time;
	uint32_t	bottom_layers;
	uint32_t	res_x;
	uint32_t	res_y;
	uint32_t	preview_large_offset;
	uint32_t	layersdef_offset;
	uint32_t	total_layers;
	uint32_t	preview_small_offset;
	uint32_t	print_time_seconds;
	uint32_t	mirror_show;
	uint32_t	print_parameters_offset;
	uint32_t	print_parameters_size;
	uint32_t	aa_grade;
	uint16_t	light_pwm;
	uint16_t	light_bottom_pwm;
	uint32_t	encryption_key;
	uint32_t	slicer_offset;
	uint32_t	slicer_size;
} FPHOTON_HEADER;

typedef struct
{
	float		lift_height_bottom;
	float		lift_speed_bottom;
	float		lift_height;
	float		lift_speed;
	float		down_speed;
	float		resin_volume;
	float		resin_weight;
	float		resin_price;
	float		slicer_lightoff_time_bottom;
	float		slicer_lightoff_time;
	uint32_t	bottom_layers;
	uint32_t	unknow1[4];
} FPHOTON_INFO;

typedef struct
{
	uint32_t	width;
	uint32_t	height;
	uint32_t	data_offset;
	uint32_t	data_length;
} FPHOTON_PREVIEW;

typedef struct
{
	float		layer_position;
	float		light_time;
	float		lightoff_time;
	uint32_t	data_offset;
	uint32_t	data_length;
	float		unknown1[4];
} FPHOTON_LAYERSINFO;

#pragma pack()




uint8_t		FPHOTON_ReadFileInfo(FIL *file);
uint8_t		FPHOTON_SetPointerToPreview(FIL *file);
uint32_t	FPHOTON_GetPreviewDataOffset();
uint16_t	FPHOTON_GetPreviewWidth();
uint16_t	FPHOTON_GetPreviewHeight();
uint8_t		FPHOTON_DrawPreview(FIL *file, TG_RECT *rect);

uint32_t	FPHOTON_GetTotalLayers();
uint32_t	FPHOTON_GetBottomLayers();
float		FPHOTON_GetLayerThickness();
uint32_t	FPHOTON_GetAntialiasing();
float		FPHOTON_GetLightLayer();
float		FPHOTON_GetLightBottom();
float		FPHOTON_GetLightPause();
float		FPHOTON_GetLiftHeight();
float		FPHOTON_GetLiftSpeed();
float		FPHOTON_GetDropSpeed();
float		FPHOTON_GetResinVolume();
uint32_t	FPHOTON_GetIndLayerSettings();
uint32_t	FPHOTON_GetResolutionX();
uint32_t	FPHOTON_GetResolutionY();

uint8_t		FPHOTON_GetLayerInfo(uint32_t layer_num, FPHOTON_LAYERSINFO *layer_info);




#endif /*__files_pws_H */

