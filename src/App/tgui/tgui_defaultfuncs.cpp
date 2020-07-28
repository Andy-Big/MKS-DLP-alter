/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "tgui_defaultfuncs.h"




void		_tgui_DrawStringInRect(char *str, TGUI_RECT *rect, TGUI_TEXTOPTIONS *opt)
{
	if (str == NULL)
		return;
	int16_t tx = rect->left;
	int16_t ty = rect->top;
	int16_t tw = 0;
	int16_t th = 0;
	if (opt->textalign_h == HTA_CENTER)
	{
		tw = LCDUI_GetTextWidthUTF(str);
		tx += ((rect->right - tx) - tw) / 2;
	}
	else
	{
		if (opt->textalign_h == HTA_RIGHT)
		{
			tw = LCDUI_GetTextWidthUTF(str);
			tx = (rect->right - tw);
		}
	}
	if (opt->textalign_v == VTA_CENTER)
	{
		th = LCDUI_GetCurrentFontHeight();
		ty += ((rect->bottom - ty) - th) / 2 + th / 8;
	}
	else
	{
		if (opt->textalign_v == VTA_BOTTOM)
		{
			th = LCDUI_GetTextWidthUTF(str);
			ty = (rect->bottom - th) + th / 8;
		}
	}
	LCDUI_DrawTextUTF(str, LCDUI_TEXT_TRANSBACK, tx, ty);
}
//==============================================================================




void		_tgui_DrawFileBmpBackground(char* file)
{
	DWORD		readed = 0;
	uint32_t	bwidth = 0;
	uint32_t	bheight = 0;
	uint8_t		fliprows = 0;
	uint32_t	lsize = 0;
	uint32_t	imgbase;
	uint16_t	bufflines = UIDBUFF_SIZE / lsize;
	uint16_t	lpainted = 0;
	uint16_t	ltoread = 0;
	uint32_t	btoread = 0;

	tstrcpy(tfname, SpiflPath);
	tstrcat_utf(tfname, file);
	if (f_open(&tguiFile, tfname, FA_OPEN_EXISTING | FA_READ) != FR_OK)
	{
		return;
	}

	// Reading BMP header
	if (f_read(&tguiFile, tguiDBuff, 32, &readed) != FR_OK || readed < 32)
	{
		goto closeexit;
	}
	// Image width
	bwidth = *(uint32_t*)(tguiDBuff+18);
	// Image height
	bheight = *(uint32_t*)(tguiDBuff+22);
	if (bheight & 0x80000000)
	{
		// If standart row order
		bheight = 0xFFFFFFFF - bheight + 1;
	}
	else
	{
		// If flip row order
		fliprows = 1;
		LCD_WriteCmd(0x0036);
		LCD_WriteRAM(0x00F8);
	}
	
	
	// Image data offset
	imgbase = *(uint32_t *)(tguiDBuff + 10);
	// Line lenght in bytes (16 bpp aligned of DWORD)
	lsize = bwidth * 2;
	while (lsize % 4)
	{
		lsize++;
	}

	// Capacity of buffer in image lines
	bufflines = UIDBUFF_SIZE / lsize;
	
	LCD_SetWindows(0, 0, bwidth, bheight);
	LCD_WriteRAM_Prepare();
	
	if (f_lseek(&tguiFile, imgbase) != FR_OK)
	{
		goto flipcloseexit;
	}
	
	while (lpainted < bheight)
	{
		ltoread = bheight - lpainted;
		if (ltoread > bufflines)
			ltoread = bufflines;
		btoread = ltoread * lsize;
		if (btoread == 0)
			return;
		if (f_read(&tguiFile, tguiDBuff, btoread, &readed) != FR_OK || readed != btoread)
		{
			goto flipcloseexit;
		}

		LCD_WriteRAM_DMA((uint16_t*)tguiDBuff, btoread/2);

/*
		pbuf = (uint16_t*)tguiDBuff;
		for (uint16_t i = 0; i < ltoread; i++)
		{
			for (uint16_t k = 0; k < bwidth; k++)
			{
				LCD_WriteRAM(*pbuf);
				pbuf++;
			}
		}
*/
		lpainted += ltoread;
	}


flipcloseexit:
	if (fliprows)
	{
		LCD_WriteCmd(0x0036);
		LCD_WriteRAM(0x00B8);
	}
closeexit:
	f_close(&tguiFile);
	return;
}
//==============================================================================




void		_tgui_DrawFileCimgBackground(char* file)
{
	DWORD		freaded = 0;
	DWORD		readed = 0;
	uint32_t	bwidth = 0;
	uint32_t	bheight = 0;
	uint8_t		fliprows = 0;
	uint32_t	lsize = 0;
	uint32_t	imgbase;
	uint16_t	bufinpos = UIFBUFF_SIZE;
	uint16_t	bufoutpos = 0;
	uint16_t	toread = 0;
	uint8_t		len = 0;
	uint16_t	val = 0;
	
	tstrcpy(tfname, SpiflPath);
	tstrcat_utf(tfname, file);
	if (f_open(&tguiFile, tfname, FA_OPEN_EXISTING | FA_READ) != FR_OK)
	{
		return;
	}

	// Reading CIMG header
	if (f_read(&tguiFile, tguiDBuff, 4, &readed) != FR_OK || readed < 4)
	{
		goto closeexit;
	}
	freaded += readed;
	
	// Image width
	bwidth = *(uint16_t*)(tguiDBuff);
	// Image height
	bheight = *(uint16_t*)(tguiDBuff+2);
	if (bheight & 0x8000)
	{
		// If standart row order
		bheight &= 0x7FFF;
	}
	else
	{
		// If flip row order
		fliprows = 1;
		LCD_WriteCmd(0x0036);
		LCD_WriteRAM(0x00F8);
	}
	
	
	// Image data offset
	imgbase = 4;

	LCD_SetWindows(0, 0, bwidth, bheight);
	LCD_WriteRAM_Prepare();
	
	if (f_lseek(&tguiFile, imgbase) != FR_OK)
	{
		goto flipcloseexit;
	}
	freaded = imgbase;

	do
	{
		if (bufinpos == UIFBUFF_SIZE)
		{
			if (f_read(&tguiFile, tguiFBuff, UIFBUFF_SIZE, &readed) != FR_OK)
			{
				goto flipcloseexit;
			}
			bufinpos = 0;
		}
		
		len = tguiFBuff[bufinpos];
		if (len & 0x80)
		{
			len = (len & 0x7F) + 1;
/*			
			if ((bufoutpos + (len + 1) * 2) > UIDBUFF_SIZE )
			{
				LCD_WriteRAM_DMA((uint16_t*)tguiDBuff, bufoutpos / 2);
				bufoutpos = 0;
			}
*/
			if ((bufinpos + 4) > UIFBUFF_SIZE)
			{
				freaded += bufinpos;
				f_lseek(&tguiFile, freaded);
				bufinpos = UIFBUFF_SIZE;
				continue;
			}
			bufinpos++;
			val = *(uint16_t*)(tguiFBuff + bufinpos);
			for (uint8_t i = 0; i < len; i++)
			{
				LCD_WriteRAM(val);
/*
				*(uint16_t*)(tguiDBuff + bufoutpos) = val;
				bufoutpos += 2;
*/
			}
			bufinpos += 2;
		}
		else
		{
			len++;
/*
			if ((bufoutpos + (len + 1) * 2) > UIDBUFF_SIZE )
			{
				LCD_WriteRAM_DMA((uint16_t*)tguiDBuff, bufoutpos / 2);
				bufoutpos = 0;
			}
*/
			if ((bufinpos + (len + 1) * 2) > UIFBUFF_SIZE)
			{
				freaded += bufinpos;
				f_lseek(&tguiFile, freaded);
				bufinpos = UIFBUFF_SIZE;
				continue;
			}
			bufinpos++;
			for (uint8_t i = 0; i < len; i++)
			{
				LCD_WriteRAM(*(uint16_t*)(tguiFBuff + bufinpos));
/*
				*(uint16_t*)(tguiDBuff + bufoutpos) = *(uint16_t*)(tguiFBuff + bufinpos);
				bufoutpos += 2;
*/
				bufinpos += 2;
			}
			
		}
	} while (readed == UIFBUFF_SIZE || bufinpos < readed);

	if (bufoutpos > 0)
		LCD_WriteRAM_DMA((uint16_t*)tguiDBuff, bufoutpos / 2);

flipcloseexit:
	if (fliprows)
	{
		LCD_WriteCmd(0x0036);
		LCD_WriteRAM(0x00B8);
	}
closeexit:
	f_close(&tguiFile);
	return;
}
//==============================================================================




void		_tgui_DefaultButtonPaint(void *tguiobj, void *param)
{
	TGUI_BUTTON		*btn = (TGUI_BUTTON*)tguiobj;
//	if (btn->options.repaintonpress == BGP_NONE)
//		return;

	
	uint16_t oldcolor = LCDUI_SetColor(btn->backcolor_en);
	uint16_t oldbackcolor = LCDUI_SetBackColor(btn->backcolor_en);
	LCDUI_FONT_TYPE oldfont = LCDUI_SetFont(btn->font);

	if (btn->options.bgpaint == BGP_FILL)
	{
		LCDUI_FillRoundRect(btn->position.left, btn->position.top, btn->position.right - btn->position.left, btn->position.bottom - btn->position.top, 7);
	}
	else
	{
		if (btn->options.bgpaint == BGP_SCREEN)
		{
			//LCDUI_FillRoundRect(btn->position.left, btn->position.top, btn->position.right - btn->position.left, btn->position.bottom - btn->position.top, 7);
		}
	}
	
	if (btn->text != NULL)
	{
		LCDUI_SetColor(btn->textcolor_en);
		_tgui_DrawStringInRect(btn->text, &(btn->textposition), &(btn->textoptions));
	}
	

	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================




void		_tgui_DefaultScreenPaint(void *tguiobj, void *param)
{
	TGUI_SCREEN		*scr = (TGUI_SCREEN*)tguiobj;
	
	uint16_t oldcolor = LCDUI_SetColor(scr->nametextcolor);
	uint16_t oldbackcolor = LCDUI_SetBackColor(scr->backcolor);
	LCDUI_FONT_TYPE oldfont = LCDUI_SetFont(scr->namefont);
	
	if (scr->bgimagename != NULL)
	{
		_tgui_DrawFileCimgBackground(scr->bgimagename);
	}
	else
	{
		LCDUI_Clear();
	}
	_tgui_DrawStringInRect(scr->name, &(scr->nameposition), &(scr->nameoptions));

	for (uint8_t i = 0; i < scr->btns_count; i++)
	{
		if (scr->buttons[i].funcs._call_paint != NULL)
			scr->buttons[i].funcs._call_paint(&(scr->buttons[i]), 0);
	}

	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================




void		_tgui_DefaultScreenProcess(void *tguiobj, void *param)
{
	TGUI_SCREEN		*scr = (TGUI_SCREEN*)tguiobj;
	
	uint16_t oldcolor = LCDUI_SetColor(scr->nametextcolor);
	uint16_t oldbackcolor = LCDUI_SetBackColor(scr->backcolor);
	LCDUI_FONT_TYPE oldfont = LCDUI_SetFont(scr->namefont);
	
	if (scr->bgimagename != NULL)
	{
		_tgui_DrawFileBmpBackground(scr->bgimagename);
	}
	else
	{
		LCDUI_Clear();
	}
	_tgui_DrawStringInRect(scr->name, &(scr->nameposition), &(scr->nameoptions));

	for (uint8_t i = 0; i < scr->btns_count; i++)
	{
		if (scr->buttons[i].funcs._call_paint != NULL)
			scr->buttons[i].funcs._call_paint(&(scr->buttons[i]), 0);
	}

	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================




