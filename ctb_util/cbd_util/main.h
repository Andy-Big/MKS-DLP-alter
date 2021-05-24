#pragma once

#include <stdint.h>

#pragma pack(1)


typedef struct
{
	uint32_t	header;		// 0x12FD0086
	uint32_t	version;
	float		bed_x_mm;
	float		bed_y_mm;
	float		bed_z_mm;
	uint32_t	unknow1[2];
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
} FCTB_HEADER;

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
	uint32_t	unknow1[5];
} FCTB_INFO;

typedef struct
{
	uint32_t	unknown1[7];
	uint32_t	machine_type_offset;
	uint32_t	machine_type_len;
	uint32_t	encryption_mode;
	uint32_t	count_unknown;
	uint32_t	aa_grade;
	uint32_t	software_version;
	uint32_t	unknown2[3];
	uint32_t	transition_layers;
	uint32_t	unknowv3[3];
} FCTB_INFOEXT;

typedef struct
{
	uint32_t	width;
	uint32_t	height;
	uint32_t	data_offset;
	uint32_t	data_length;
	uint32_t	unknow1[4];
} FCTB_PREVIEW;

typedef struct
{
	float		layer_position;
	float		light_time;
	float		lightoff_time;
	uint32_t	data_offset;
	uint32_t	data_length;
	uint32_t	unknown1[4];
} FCTB_LAYERSINFO;


#pragma pack()



void		print_help();
