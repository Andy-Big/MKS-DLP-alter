#include "files_photon.h"



extern __no_init uint8_t 		fbuff[16384] @ "CCMRAM";
extern __no_init FIL			ufile @ "CCMRAM";
extern __no_init FIL			sfile @ "CCMRAM";


FPHOTON_HEADER			fphoton_header;
FPHOTON_INFO			fphoton_info;
FPHOTON_PREVIEW			fphoton_preview;



uint8_t		FPHOTON_ReadFileInfo(FIL *file)
{
	uint32_t	rd;
	
	memset(&fphoton_header, 0, sizeof(FPHOTON_HEADER));
	memset(&fphoton_info, 0, sizeof(FPHOTON_INFO));
	memset(&fphoton_preview, 0, sizeof(FPHOTON_PREVIEW));

	// header
	if (f_read(file, &fphoton_header, sizeof(FPHOTON_HEADER), &rd) != FR_OK || rd != sizeof(FPHOTON_HEADER))
		return 0;
	if (fphoton_header.header != 0x12FD0019)
		return 0;
	if (fphoton_header.version == 1)
		fphoton_header.aa_grade = 1;
	
	// info
	if (f_lseek(file, fphoton_header.print_parameters_offset) != FR_OK)
		return 0;
	if (f_read(file, &fphoton_info, sizeof(FPHOTON_INFO), &rd) != FR_OK || rd != sizeof(FPHOTON_INFO))
		return 0;
	
	// preview
	if (f_lseek(file, fphoton_header.preview_large_offset) != FR_OK)
		return 0;
	if (f_read(file, &fphoton_preview, sizeof(FPHOTON_PREVIEW), &rd) != FR_OK || rd != sizeof(FPHOTON_PREVIEW))
		return 0;

	return 1;
}
//==============================================================================




uint8_t		FPHOTON_SetPointerToPreview(FIL *file)
{
	if (fphoton_header.header != 0x12FD0019)
		return 0;
	
	if (f_lseek(file, fphoton_header.preview_large_offset) != FR_OK)
		return 0;

	return 1;
}
//==============================================================================




uint32_t	FPHOTON_GetPreviewDataOffset()
{
	if (fphoton_header.header != 0x12FD0019)
		return 0;
	
	return fphoton_header.preview_large_offset + sizeof(FPHOTON_PREVIEW);
}
//==============================================================================




uint16_t	FPHOTON_GetPreviewWidth()
{
	return fphoton_preview.width;
}
//==============================================================================




uint16_t	FPHOTON_GetPreviewHeight()
{
	return fphoton_preview.height;
}
//==============================================================================




uint8_t		FPHOTON_DrawPreview(FIL *file, TG_RECT *rect)
{
	if (fphoton_header.header != 0x12FD0019)
		return 0;

	uint16_t		pw = 0, ph = 0, rw = 0, rh = 0, iw = 0, ih = 0, ix = 0, iy = 0;
	float			pscale = 0, nextcol = 0, nextline = 0;
	uint32_t		cpainted = 0;
	uint32_t		lpainted = 0;
	uint32_t		rd = 0;
	uint16_t		*buff, *decbuff;
	uint32_t		doffset = FPHOTON_GetPreviewDataOffset();
	uint16_t		dbuff[480];
	uint32_t		bufpos = 0, decbufpos = 0, oldbufpos = 0, oldline = 0, btoread = 0;


	pw = fphoton_preview.width;
	ph = fphoton_preview.height;
	// read by full lines of source preview image
	btoread = 4096;

	if (pw == 0 || ph == 0)
		return 0;
	
	rw = rect->right - rect->left + 1;
	rh = rect->bottom - rect->top + 1;
	pscale = (float)pw / (float)rw;
	if ((float)ph / (float)rh > pscale)
		pscale = (float)ph / (float)rh;
//	if (pscale < 1)
//		pscale = 1;
	
	iw = (uint32_t)((float)pw / pscale);
	ih = (uint32_t)((float)ph / pscale);
	ix = rect->left + (rw - iw) / 2;
	iy = rect->top + (rh - ih) / 2;

	buff = (uint16_t*)fbuff;
	decbuff = (uint16_t*)(fbuff + 4096);

	LCD_SetWindows(ix, iy, iw, ih);
	LCD_WriteRAM_Prepare();

	if (f_lseek(file, doffset) != FR_OK)
		return 0;
	if (f_read(file, fbuff, btoread, &rd) != FR_OK || rd != btoread)
		return 0;

	bufpos = (uint32_t)(nextcol);
	uint16_t	repeat = 1;
	uint16_t	dot = buff[bufpos]
	while (lpainted < ih)
	{
		while (cpainted < iw)
		{
			if (bufpos >= btoread / 2)
			{
				if (f_read(file, fbuff, btoread, &rd) != FR_OK)
					return 0;
				bufpos = 0;
				oldbufpos = bufpos;
				nextcol = 0;
			}
			dbuff[cpainted] = buff[bufpos];
//			LCD_WriteRAM(buff[(uint32_t)(nextcol)]);
			nextcol += pscale;
			cpainted++;
			bufpos = oldbufpos + (uint32_t)(nextcol);
		}
		LCD_WriteRAM_DMA(dbuff, cpainted);
		nextcol = 0;
		nextline += pscale;
		cpainted = 0;
		lpainted++;
		if (((uint32_t)nextline - oldline) > 1)
			bufpos += ((uint32_t)nextline - oldline) * pw;
		if (((uint32_t)nextline - oldline) == 0)
			bufpos -= pw;
		if (bufpos % pw)
			bufpos -= (bufpos % pw);
		oldline = (uint32_t)nextline;
		oldbufpos = bufpos;
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




float		FPHOTON_GetLightBottom()
{
	return fphoton_header.slicer_expbottom_time;
}
//==============================================================================




float		FPHOTON_GetLightPause()
{
	return fphoton_header.slicer_lightoff_time;
}
//==============================================================================




float		FPHOTON_GetLiftHeight()
{
	return fphoton_info.lift_height;
}
//==============================================================================




float		FPHOTON_GetLiftSpeed()
{
	return fphoton_info.lift_speed;
}
//==============================================================================




float		FPHOTONFPHOTON_GetDropSpeed()
{
	return fphoton_info.down_speed;
}
//==============================================================================




float		FPHOTONFPHOTON_GetResinVolume()
{
	return fphoton_info.resin_volume;
}
//==============================================================================




uint32_t	FPHOTONFPHOTON_GetIndLayerSettings()
{
	return 1;
}
//==============================================================================




uint32_t	FPHOTONFPHOTON_GetResolutionX()
{
	return fphoton_header.res_x;
}
//==============================================================================




uint32_t	FPHOTONFPHOTON_GetResolutionY()
{
	return fphoton_header.res_y;
}
//==============================================================================




uint8_t		FPHOTON_GetLayerInfo(uint32_t layer_num, FPHOTON_LAYERSINFO *layer_info)
{
	if (layer_num >= fphoton_header.total_layers)
		return 0;
	
	uint32_t	rd = 0;
	uint32_t	data_offset = fphoton_header.layersdef_offset;
	data_offset += sizeof(FPHOTON_LAYERSINFO) * layer_num;

	if (f_lseek(&ufile, data_offset) != FR_OK)
		return 0;
	if (f_read(&ufile, layer_info, sizeof(FPHOTON_LAYERSINFO), &rd) != FR_OK || rd != sizeof(FPHOTON_LAYERSINFO))
		return 0;

	return 1;
}
//==============================================================================




