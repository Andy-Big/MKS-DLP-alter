#include "files_pws.h"



extern __no_init uint8_t 		fbuff[16384] @ "CCMRAM";
extern __no_init FIL			ufile @ "CCMRAM";
extern __no_init FIL			sfile @ "CCMRAM";


FPWS_HEADER			fpws_header;
FPWS_INFO			fpws_info;
FPWS_PREVIEW		fpws_preview;
FPWS_LAYERSDEF		fpws_layersdata;

uint8_t		FPWS_ReadFileInfo(FIL *file)
{
	uint32_t	rd;
	
	memset(&fpws_header, 0, sizeof(FPWS_HEADER));
	memset(&fpws_info, 0, sizeof(FPWS_INFO));
	memset(&fpws_preview, 0, sizeof(FPWS_PREVIEW));
	memset(&fpws_layersdata, 0, sizeof(FPWS_LAYERSDEF));

	// header
	if (f_read(file, &fpws_header, sizeof(FPWS_HEADER), &rd) != FR_OK || rd != sizeof(FPWS_HEADER))
		return 0;
	if (strcmp(fpws_header.mark, (char*)"ANYCUBIC") != 0)
		return 0;
	if (fpws_header.version != 1)
		return 0;
	
	// info
	if (f_lseek(file, fpws_header.info_offset) != FR_OK)
		return 0;
	if (f_read(file, &fpws_info, sizeof(FPWS_INFO), &rd) != FR_OK || rd != sizeof(FPWS_INFO))
		return 0;
	if (strcmp(fpws_info.mark, (char*)"HEADER") != 0)
		return 0;
	
	// preview
	if (f_lseek(file, fpws_header.preview_offset) != FR_OK)
		return 0;
	if (f_read(file, &fpws_preview, sizeof(FPWS_PREVIEW), &rd) != FR_OK || rd != sizeof(FPWS_PREVIEW))
		return 0;
	if (strcmp(fpws_preview.mark, (char*)"PREVIEW") != 0)
		return 0;

	// layers data
	if (f_lseek(file, fpws_header.layersdef_offset) != FR_OK)
		return 0;
	if (f_read(file, &fpws_layersdata, sizeof(FPWS_LAYERSDEF), &rd) != FR_OK || rd != sizeof(FPWS_LAYERSDEF))
		return 0;
	if (strcmp(fpws_layersdata.mark, (char*)"LAYERDEF") != 0)
		return 0;

	
	return 1;
}
//==============================================================================




uint8_t		FPWS_SetPointerToPreview(FIL *file)
{
	if (fpws_header.mark[0] == 0)
		return 0;
	
	if (f_lseek(file, fpws_header.preview_offset) != FR_OK)
		return 0;

	return 1;
}
//==============================================================================




uint32_t	FPWS_GetPreviewDataOffset()
{
	if (fpws_header.mark[0] == 0)
		return 0;
	
	return fpws_header.preview_offset + sizeof(FPWS_PREVIEW);
}
//==============================================================================




uint16_t	FPWS_GetPreviewWidth()
{
	return fpws_preview.width;
}
//==============================================================================




uint16_t	FPWS_GetPreviewHeight()
{
	return fpws_preview.height;
}
//==============================================================================




uint8_t		FPWS_DrawPreview(FIL *file, TG_RECT *rect)
{
	if (fpws_header.mark[0] == 0)
		return 0;

	uint16_t		pw = 0, ph = 0, rw = 0, rh = 0, iw = 0, ih = 0, ix = 0, iy = 0;
	float			pscale = 0, nextcol = 0, nextline = 0;
	uint32_t		cpainted = 0;
	uint32_t		lpainted = 0;
	uint32_t		rd = 0;
	uint16_t		*buff;
	uint32_t		doffset = FPWS_GetPreviewDataOffset();
	uint16_t		dbuff[480];
	uint32_t		bufpos = 0, oldbufpos = 0, oldline = 0, btoread = 0;


	pw = FPWS_GetPreviewWidth();
	ph = FPWS_GetPreviewHeight();
	// read by full lines of source preview image
	btoread = sizeof(fbuff) / ( pw * 2) * pw;

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

	LCD_SetWindows(ix, iy, iw, ih);
	LCD_WriteRAM_Prepare();

	if (f_lseek(file, doffset) != FR_OK)
		return 0;
	if (f_read(file, fbuff, btoread, &rd) != FR_OK || rd != btoread)
		return 0;

	bufpos = (uint32_t)(nextcol);
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




uint32_t	FPWS_GetTotalLayers()
{
	return fpws_layersdata.total_layers;
}
//==============================================================================




uint32_t	FPWS_GetBottomLayers()
{
	return (uint32_t)fpws_info.bottom_layers;
}
//==============================================================================




float		FPWS_GetLayerThickness()
{
	return fpws_info.layers_thickness;
}
//==============================================================================




uint32_t	FPWS_GetAntialiasing()
{
	return fpws_info.aa_grade;
}
//==============================================================================




float		FPWS_GetLightLayer()
{
	return fpws_info.exp_time;
}
//==============================================================================




float		FPWS_GetLightBottom()
{
	return fpws_info.expbottom_time;
}
//==============================================================================




float		FPWS_GetLightPause()
{
	return fpws_info.lightoff_time;
}
//==============================================================================




float		FPWS_GetLiftHeight()
{
	return fpws_info.lift_height;
}
//==============================================================================




float		FPWS_GetLiftSpeed()
{
	return fpws_info.lift_speed;
}
//==============================================================================




float		FPWS_GetDropSpeed()
{
	return fpws_info.down_speed;
}
//==============================================================================




float		FPWS_GetResinVolume()
{
	return fpws_info.resin_volume;
}
//==============================================================================




uint32_t	FPWS_GetIndLayerSettings()
{
	return fpws_info.use_layer_params;
}
//==============================================================================




uint32_t	FPWS_GetResolutionX()
{
	return fpws_info.res_x;
}
//==============================================================================




uint32_t	FPWS_GetResolutionY()
{
	return fpws_info.res_y;
}
//==============================================================================




uint8_t		FPWS_GetLayerInfo(uint32_t layer_num, FPWS_LAYERSINFO *layer_info)
{
	if (layer_num >= fpws_layersdata.total_layers)
		return 0;
	
	uint32_t	rd = 0;
	uint32_t	data_offset = fpws_header.layersdef_offset;
	data_offset += sizeof(FPWS_LAYERSDEF);
	data_offset += sizeof(FPWS_LAYERSINFO) * layer_num;

	if (f_lseek(&ufile, data_offset) != FR_OK)
		return 0;
	if (f_read(&ufile, layer_info, sizeof(FPWS_LAYERSINFO), &rd) != FR_OK || rd != sizeof(FPWS_LAYERSINFO))
		return 0;

	return 1;
}
//==============================================================================




