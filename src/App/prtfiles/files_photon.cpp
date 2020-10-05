#include "files_photon.h"
#include "config.h"



extern __no_init uint8_t 		fbuff[16384] @ "CCMRAM";
extern __no_init FIL			ufile @ "CCMRAM";
extern __no_init FIL			sfile @ "CCMRAM";


FPHOTON_HEADER			fphoton_header;
FPHOTON_INFO			fphoton_info;
FPHOTON_PREVIEW			fphoton_preview_big;
FPHOTON_PREVIEW			fphoton_preview_small;



uint8_t		FPHOTON_ReadFileInfo(FIL *file)
{
	uint32_t	readed;
	
	memset(&fphoton_header, 0, sizeof(FPHOTON_HEADER));
	memset(&fphoton_info, 0, sizeof(FPHOTON_INFO));
	memset(&fphoton_preview_big, 0, sizeof(FPHOTON_PREVIEW));

	// header
	if (f_read(file, &fphoton_header, sizeof(FPHOTON_HEADER), &readed) != FR_OK || readed != sizeof(FPHOTON_HEADER))
		return 0;
	if (fphoton_header.header != 0x12FD0019 && fphoton_header.header != 0x12FD0086)
		return 0;
	// info
	if (fphoton_header.version == 1)
	{
		LAYER_INFO		l_info;
		
		FPHOTON_GetLayerInfo(0, &l_info);
		fphoton_info.lift_height_bottom = 5;
		fphoton_info.lift_speed_bottom = 120;
		fphoton_info.slicer_lightoff_time_bottom = l_info.lightoff_time;

		FPHOTON_GetLayerInfo(fphoton_header.bottom_layers, &l_info);
		fphoton_info.down_speed = 120;
		fphoton_info.lift_height = 5;
		fphoton_info.lift_speed = 120;
		fphoton_info.slicer_lightoff_time = l_info.lightoff_time;

		fphoton_header.aa_grade = 1;
		fphoton_info.bottom_layers = fphoton_header.bottom_layers;
		fphoton_info.resin_price = 0;
		fphoton_info.resin_volume = 0;
		fphoton_info.resin_weight = 0;
	}
	else
	{
		if (f_lseek(file, fphoton_header.print_parameters_offset) != FR_OK)
			return 0;
		if (f_read(file, &fphoton_info, sizeof(FPHOTON_INFO), &readed) != FR_OK || readed != sizeof(FPHOTON_INFO))
			return 0;
	}
	
	// preview big
	if (f_lseek(file, fphoton_header.preview_large_offset) != FR_OK)
		return 0;
	if (f_read(file, &fphoton_preview_big, sizeof(FPHOTON_PREVIEW), &readed) != FR_OK || readed != sizeof(FPHOTON_PREVIEW))
		return 0;

	// preview small
	if (f_lseek(file, fphoton_header.preview_small_offset) != FR_OK)
		return 0;
	if (f_read(file, &fphoton_preview_small, sizeof(FPHOTON_PREVIEW), &readed) != FR_OK || readed != sizeof(FPHOTON_PREVIEW))
		return 0;

	return 1;
}
//==============================================================================




uint8_t		FPHOTON_SetPointerToPreview(FIL *file, uint8_t small)
{
	if (fphoton_header.header != 0x12FD0019 && fphoton_header.header != 0x12FD0086)
		return 0;

	if (small > 0)
	{
		if (f_lseek(file, fphoton_header.preview_small_offset) != FR_OK)
			return 0;
	}
	else
	{
		if (f_lseek(file, fphoton_header.preview_large_offset) != FR_OK)
			return 0;
	}

	return 1;
}
//==============================================================================




uint32_t	FPHOTON_GetPreviewDataOffset(uint8_t small)
{
	if (fphoton_header.header != 0x12FD0019 && fphoton_header.header != 0x12FD0086)
		return 0;
	
	if (small > 0)
		return fphoton_header.preview_small_offset + sizeof(FPHOTON_PREVIEW);
	else
		return fphoton_header.preview_large_offset + sizeof(FPHOTON_PREVIEW);
}
//==============================================================================




uint32_t	FPHOTON_GetPreviewWidth(uint8_t small)
{
	if (small > 0)
		return fphoton_preview_small.width;
	else
		return fphoton_preview_big.width;
}
//==============================================================================




uint32_t	FPHOTON_GetPreviewHeight(uint8_t small)
{
	if (small > 0)
		return fphoton_preview_small.height;
	else
		return fphoton_preview_big.height;
}
//==============================================================================




uint32_t	FPHOTON_GetPreviewSize(uint8_t small)
{
	if (small > 0)
		return fphoton_preview_small.data_length;
	else
		return fphoton_preview_big.data_length;
}
//==============================================================================




uint8_t		FPHOTON_DrawPreview(FIL *file, TG_RECT *rect, uint8_t small)
{
	if (fphoton_header.header != 0x12FD0019 && fphoton_header.header != 0x12FD0086)
		return 0;

	uint32_t		prev_width = 0, prev_height = 0, rect_width = 0, rect_height = 0, image_width = 0, image_height = 0, image_xcoord = 0, image_ycoord = 0;
	uint32_t		prev_datasize = 0;
	float			pscale = 0;
	uint16_t		columns_readed = 0, lines_readed = 0, read_col = 0, read_line = 0;
	uint16_t		paint_col = 0, paint_line = 0;
	uint32_t		readed = 0;
	uint16_t		*read_buff;
	uint32_t		doffset = FPHOTON_GetPreviewDataOffset(small);
	uint32_t		read_buff_pos = 0, btoread = 0, total_readed = 0;
	uint16_t		color = 0, repeat = 1;
	uint16_t		dest_buff[480];
	uint16_t		dest_buff_pos = 0;


	prev_width = FPHOTON_GetPreviewWidth(small);
	prev_height = FPHOTON_GetPreviewHeight(small);
	prev_datasize = FPHOTON_GetPreviewSize(small);
	
	btoread = 4096;
	if (btoread > prev_datasize)
		btoread = prev_datasize;

	if (prev_width == 0 || prev_height == 0)
		return 0;
	
	rect_width = rect->right - rect->left + 1;
	rect_height = rect->bottom - rect->top + 1;
	pscale = (float)prev_width / (float)rect_width;
	if ((float)prev_height / (float)rect_height > pscale)
		pscale = (float)prev_height / (float)rect_height;
	
	image_width = (uint32_t)((float)prev_width / pscale);
	image_height = (uint32_t)((float)prev_height / pscale);
	image_xcoord = rect->left + (rect_width - image_width) / 2;
	image_ycoord = rect->top + (rect_height - image_height) / 2;

	read_buff = (uint16_t*)fbuff;

	LCD_SetWindows(image_xcoord, image_ycoord, image_width, image_height);
	LCD_WriteRAM_Prepare();

	if (f_lseek(file, doffset) != FR_OK)
		return 0;
	if (f_read(file, fbuff, btoread, &readed) != FR_OK || readed != btoread)
		return 0;
	total_readed += btoread;

	while (1)
	{
		repeat = 1;
		color = read_buff[read_buff_pos++];
		if (read_buff_pos >= btoread / 2 && total_readed < prev_datasize)
		{
			btoread = 4096;
			if (btoread > prev_datasize - total_readed)
				btoread = prev_datasize - total_readed;
			if (f_read(file, fbuff, btoread, &readed) != FR_OK || readed != btoread)
				return 0;
			total_readed += btoread;
			read_buff_pos = 0;
		}
		
		if (color & 0x0020)
		{
			repeat += (read_buff[read_buff_pos++] & 0x0FFF);
			if (read_buff_pos >= btoread / 2 && total_readed < prev_datasize)
			{
				btoread = 4096;
				if (btoread > prev_datasize - total_readed)
					btoread = prev_datasize - total_readed;
				if (f_read(file, fbuff, btoread, &readed) != FR_OK || readed != btoread)
					return 0;
				total_readed += btoread;
				read_buff_pos = 0;
			}
		}
		
		for (uint32_t i = 0; i < repeat; i++)
		{
			while (columns_readed == read_col && lines_readed == read_line)
			{
				dest_buff[dest_buff_pos] = color;
				dest_buff_pos++;
				paint_col++;
				if (paint_col >= image_width)
				{
					uint16_t	next_read_line = (uint16_t)((float)paint_line * pscale);
					while (next_read_line == read_line)
					{
						LCD_WriteRAM_DMA(dest_buff, dest_buff_pos);
						paint_line++;
						next_read_line = (uint16_t)((float)paint_line * pscale);
					}
					paint_col = 0;
					read_line = next_read_line;
					dest_buff_pos = 0;
				}
				read_col = (uint16_t)((float)paint_col * pscale);
			}

			columns_readed++;
			if (columns_readed == prev_width)
			{
				columns_readed = 0;
				lines_readed++;
			}
		}
		if ((total_readed >= prev_datasize && read_buff_pos >= btoread / 2))
			break;
	}

	return 1;
}
//==============================================================================




uint32_t	FPHOTON_GetTotalLayers()
{
	return fphoton_header.total_layers;
}
//==============================================================================




uint32_t	FPHOTON_GetBottomLayers()
{
	return (uint32_t)fphoton_info.bottom_layers;
}
//==============================================================================




float		FPHOTON_GetLayerThickness()
{
	return fphoton_header.slicer_layers_thickness;
}
//==============================================================================




uint32_t	FPHOTON_GetAntialiasing()
{
	return fphoton_header.aa_grade;
}
//==============================================================================




float		FPHOTON_GetLightLayer()
{
	return fphoton_header.slicer_exp_time;
}
//==============================================================================




void		FPHOTON_SetLightLayer(float val)
{
	fphoton_header.slicer_exp_time = val;
}
//==============================================================================




float		FPHOTON_GetLightBottom()
{
	return fphoton_header.slicer_expbottom_time;
}
//==============================================================================




void		FPHOTON_SetLightBottom(float val)
{
	fphoton_header.slicer_expbottom_time = val;
}
//==============================================================================




float		FPHOTON_GetLightPause()
{
	return fphoton_header.slicer_lightoff_time;
}
//==============================================================================




void		FPHOTON_SetLightPause(float val)
{
	fphoton_header.slicer_lightoff_time = val;
}
//==============================================================================




float		FPHOTON_GetLiftHeight()
{
	return fphoton_info.lift_height;
}
//==============================================================================




void		FPHOTON_SetLiftHeight(float val)
{
	fphoton_info.lift_height = val;
	fphoton_info.lift_height_bottom = val;
}
//==============================================================================




float		FPHOTON_GetLiftSpeed()
{
	return fphoton_info.lift_speed / 60.0;
}
//==============================================================================




void		FPHOTON_SetLiftSpeed(float val)
{
	fphoton_info.lift_speed = val * 60.0;
	fphoton_info.lift_speed_bottom = val * 60;
}
//==============================================================================




float		FPHOTON_GetDropSpeed()
{
	return fphoton_info.down_speed / 60.0;
}
//==============================================================================




void		FPHOTON_SetDropSpeed(float val)
{
	fphoton_info.down_speed = val * 60.0;
}
//==============================================================================




float		FPHOTON_GetResinVolume()
{
	return fphoton_info.resin_volume;
}
//==============================================================================




uint32_t	FPHOTON_GetIndLayerSettings()
{
	return 1;
}
//==============================================================================




uint32_t	FPHOTON_GetResolutionX()
{
	return fphoton_header.res_x;
}
//==============================================================================




uint32_t	FPHOTON_GetResolutionY()
{
	return fphoton_header.res_y;
}
//==============================================================================




uint32_t	FPHOTON_GetVersion()
{
	return fphoton_header.version;
}
//==============================================================================




uint8_t		FPHOTON_GetLayerInfo(uint32_t layer_num, LAYER_INFO *layer_info)
{
	if (layer_num >= fphoton_header.total_layers)
		return 0;
	
	uint32_t			readed = 0;
	uint32_t			data_offset = fphoton_header.layersdef_offset;
	FPHOTON_LAYERSINFO	photon_linfo;
	
	data_offset += sizeof(FPHOTON_LAYERSINFO) * layer_num;

	if (f_lseek(&ufile, data_offset) != FR_OK)
		return 0;
	if (f_read(&ufile, &photon_linfo, sizeof(FPHOTON_LAYERSINFO), &readed) != FR_OK || readed != sizeof(FPHOTON_LAYERSINFO))
		return 0;

	layer_info->data_offset = photon_linfo.data_offset;
	layer_info->data_length = photon_linfo.data_length;
	layer_info->layer_position = photon_linfo.layer_position;
	layer_info->drop_speed = fphoton_info.down_speed;

	if (layer_num < fphoton_info.bottom_layers)
	{
		layer_info->lift_height = fphoton_info.lift_height_bottom;
		layer_info->lift_speed = fphoton_info.lift_speed_bottom;
	}
	else
	{
		layer_info->lift_height = fphoton_info.lift_height;
		layer_info->lift_speed = fphoton_info.lift_speed;
	}

	if (systemInfo.print_use_ind_params == 0)
	{
		layer_info->lightoff_time = fphoton_header.slicer_lightoff_time;
		if (layer_num < fphoton_info.bottom_layers)
		{
			layer_info->light_time = fphoton_header.slicer_expbottom_time;
		}
		else
		{
			layer_info->light_time = fphoton_header.slicer_exp_time;
		}
	}
	else
	{
		layer_info->lightoff_time = photon_linfo.lightoff_time;
		layer_info->light_time = photon_linfo.light_time;
	}

	return 1;
}
//==============================================================================




