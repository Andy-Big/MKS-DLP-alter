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



// Direct display writing - 65 ms in low optimize, 48 ms in high optimize
/*
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
			}
			bufinpos += 2;
		}
		else
		{
			len++;
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
				bufinpos += 2;
			}
			
		}
	} while (readed == UIFBUFF_SIZE || bufinpos < readed);

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
/**/


// DMA single buffer - 62 ms in low optimize, 47 ms in high optimize with bugs
/*
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
			if ((bufoutpos + (len + 1) * 2) > UIDBUFF_SIZE )
			{
				LCD_WriteRAM_DMA((uint16_t*)tguiDBuff, bufoutpos / 2);
				bufoutpos = 0;
			}
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
				*(uint16_t*)(tguiDBuff + bufoutpos) = val;
				bufoutpos += 2;
			}
			bufinpos += 2;
		}
		else
		{
			len++;
			if ((bufoutpos + (len + 1) * 2) > UIDBUFF_SIZE )
			{
				LCD_WriteRAM_DMA((uint16_t*)tguiDBuff, bufoutpos / 2);
				bufoutpos = 0;
			}
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
				*(uint16_t*)(tguiDBuff + bufoutpos) = *(uint16_t*)(tguiFBuff + bufinpos);
				bufoutpos += 2;
				bufinpos += 2;
			}
			
		}
	} while (readed == UIFBUFF_SIZE || bufinpos < readed);

	if (bufoutpos > 0)
	{
		LCD_WriteRAM_DMA((uint16_t*)tguiDBuff, bufoutpos / 2);
	}
	LCD_WaitDMAReady();

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
/**/


// DMA double buffer - 62 ms in low optimize, 47 ms in high optimize

void		_tgui_DrawFileCimgBackground(char* file)
{
	DWORD		freaded = 0;
	DWORD		readed = 0;
	uint32_t	bwidth = 0;
	uint32_t	bheight = 0;
	uint8_t		fliprows = 0;
	uint32_t	imgbase;
	uint16_t	bufinpos = UIFBUFF_SIZE;
	uint16_t	bufoutpos = 0;
	uint8_t		len = 0;
	uint16_t	val = 0;
	uint8_t		curbuffnum = 0;
	uint16_t	*curbuff = (uint16_t*)tguiDBuff;
	

/*
	tstrcpy(tfname, UsbPath);
	tstrcat_utf(tfname, (char*)"alterupd\\images");
	tstrcat_utf(tfname, (char*)"\\");
*/

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
			if ((bufoutpos + (len + 1)) > (UIDBUFF_SIZE / 4) )
			{
//				LCD_WaitDMAReady();
				LCD_WriteRAM_DMA(curbuff, bufoutpos);
				if ((uint32_t)curbuff == (uint32_t)tguiDBuff)
				{
					curbuff = (uint16_t*)(tguiDBuff + (UIDBUFF_SIZE / 2));
					curbuffnum = 1;
				}
				else
				{
					curbuff = (uint16_t*)(tguiDBuff);
					curbuffnum = 0;
				}
				bufoutpos = 0;
			}
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
				*(curbuff + bufoutpos) = val;
				bufoutpos++;
			}
			bufinpos += 2;
		}
		else
		{
			len++;
			if ((bufoutpos + (len + 1)) > (UIDBUFF_SIZE / 4))
			{
//				LCD_WaitDMAReady();
				LCD_WriteRAM_DMA(curbuff, bufoutpos);
				if (curbuffnum == 0)
				{
					curbuff = (uint16_t*)(tguiDBuff + (UIDBUFF_SIZE / 2));
					curbuffnum = 1;
				}
				else
				{
					curbuff = (uint16_t*)(tguiDBuff);
					curbuffnum = 0;
				}
				bufoutpos = 0;
			}
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
				*(curbuff + bufoutpos) = *(uint16_t*)(tguiFBuff + bufinpos);
				bufoutpos++;
				bufinpos += 2;
			}
			
		}
	} while (readed == UIFBUFF_SIZE || bufinpos < readed);

	if (bufoutpos > 0)
	{
		LCD_WaitDMAReady();
		LCD_WriteRAM_DMA(curbuff, bufoutpos);
	}
	LCD_WaitDMAReady();

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
/**/



void		_tgui_DefaultButtonPaint(void *tguiobj, void *param)
{
	TGUI_BUTTON		*thisbtn = (TGUI_BUTTON*)tguiobj;
//	if (thisbtn->options.repaintonpress == BGP_NONE)
//		return;

	
	uint16_t oldcolor = LCDUI_SetColor(thisbtn->backcolor_en);
	uint16_t oldbackcolor = LCDUI_SetBackColor(thisbtn->backcolor_en);
	LCDUI_FONT_TYPE oldfont = LCDUI_SetFont(thisbtn->font);

	if (thisbtn->options.bgpaint == BGP_FILL)
	{
		if (thisbtn->options.disabled == 1)
		{
			LCDUI_SetBackColor(thisbtn->backcolor_dis);
			LCDUI_SetColor(thisbtn->backcolor_dis);
		}
		else
		{
			if (thisbtn->options.pressed == 1 && thisbtn->options.repaintonpress == 1)
			{
				LCDUI_SetBackColor(thisbtn->backcolor_press);
				LCDUI_SetColor(thisbtn->backcolor_press);
			}
		}
		LCDUI_FillRoundRect(thisbtn->position.left, thisbtn->position.top, thisbtn->position.right - thisbtn->position.left, thisbtn->position.bottom - thisbtn->position.top, 7);
	}
	else
	{
		if (thisbtn->options.bgpaint == BGP_SCREEN)
		{
		}
	}
	
	LCDUI_SetColor(thisbtn->textcolor_en);
	if (thisbtn->text != NULL)
	{
		if (thisbtn->options.disabled == 1)
		{
			LCDUI_SetColor(thisbtn->textcolor_dis);
		}
		else
		{
			if (thisbtn->options.pressed == 1 && thisbtn->options.repaintonpress == 1)
			{
				LCDUI_SetColor(thisbtn->textcolor_press);
			}
		}
		_tgui_DrawStringInRect(thisbtn->text, &(thisbtn->textposition), &(thisbtn->textoptions));
	}
	

	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================




void		_tgui_DefaultScreenPaint(void *tguiobj, void *param)
{
	TGUI_SCREEN		*thisscr = (TGUI_SCREEN*)tguiobj;
	
	uint16_t oldcolor = LCDUI_SetColor(thisscr->nametextcolor);
	uint16_t oldbackcolor = LCDUI_SetBackColor(thisscr->backcolor);
	LCDUI_FONT_TYPE oldfont = LCDUI_SetFont(thisscr->namefont);
	
	if (thisscr->bgimagename != NULL)
	{
		_tgui_DrawFileCimgBackground(thisscr->bgimagename);
	}
	else
	{
		LCDUI_Clear();
	}
	_tgui_DrawStringInRect(thisscr->name, &(thisscr->nameposition), &(thisscr->nameoptions));

	for (uint8_t i = 0; i < thisscr->btns_count; i++)
	{
		if (thisscr->buttons[i].funcs._call_paint != NULL)
			thisscr->buttons[i].funcs._call_paint(&(thisscr->buttons[i]), 0);
	}

	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================




void		_tgui_DefaultScreenProcess(void *tguiobj, void *param)
{
	TGUI_SCREEN		*thisscr = (TGUI_SCREEN*)tguiobj;
	TOUCH_STATES	ts;
	TOUCH_POINT		tp;
	
	Touch_GetCoords(&tp);
	ts = Touch_GetState();
	switch (ts)
	{
		case TS_SPRESSED:
			for (uint8_t i = 0; i < thisscr->btns_count; i++)
			{
				if (thisscr->buttons[i].options.disabled == 0 && thisscr->buttons[i].options.pressed == 0)
				{
					if (TGUI_PointInRect(&tp, &thisscr->buttons[i].position) == 1)
					{
						thisscr->buttons[i].options.pressed = 1;
						if (thisscr->buttons[i].options.repaintonpress == 1 && thisscr->buttons[i].funcs._call_paint != NULL)
						{
							thisscr->buttons[i].funcs._call_paint(&(thisscr->buttons[i]), NULL);
						}
						break;
					}
				}
			}
			break;

		case TS_LPRESSED:
			break;

		case TS_SRELEASED:
			for (uint8_t i = 0; i < thisscr->btns_count; i++)
			{
				if (thisscr->buttons[i].options.disabled == 0 && thisscr->buttons[i].options.pressed == 1)
				{
					thisscr->buttons[i].options.pressed = 0;
					if (thisscr->buttons[i].options.repaintonpress == 1 && thisscr->buttons[i].funcs._call_paint != NULL)
					{
						thisscr->buttons[i].funcs._call_paint(&(thisscr->buttons[i]), NULL);
					}
					if (thisscr->buttons[i].funcs._call_press != NULL)
					{
						if (thisscr->buttons[i].funcs._call_press == (pressfunc)BTNA_GOCHILDSCR && thisscr->buttons[i].childscreen != NULL)
						{
							TGUI_SCREEN		*curscreen = tguiActiveScreen;
							tguiActiveScreen = (TGUI_SCREEN*)thisscr->buttons[i].childscreen;
							tguiActiveScreen->prevscreen = curscreen;
							TGUI_ForceRepaint();
						}
						else
						{
							if (thisscr->buttons[i].funcs._call_press == (pressfunc)BTNA_GOPREVSCR && tguiActiveScreen->prevscreen != NULL)
							{
								tguiActiveScreen = (TGUI_SCREEN*)tguiActiveScreen->prevscreen;
								TGUI_ForceRepaint();
							}
							else
							{
								thisscr->buttons[i].funcs._call_press(&(thisscr->buttons[i]), NULL);
							}
						}
					}
				}

			}
			Touch_SetState(TS_WORKED);
			break;

		case TS_LRELEASED:
			for (uint8_t i = 0; i < thisscr->btns_count; i++)
			{
				if (thisscr->buttons[i].options.disabled == 0 && thisscr->buttons[i].options.pressed == 1)
				{
					thisscr->buttons[i].options.pressed = 0;
					if (thisscr->buttons[i].options.repaintonpress == 1 && thisscr->buttons[i].funcs._call_paint != NULL)
					{
						thisscr->buttons[i].funcs._call_paint(&(thisscr->buttons[i]), NULL);
					}
				}

			}
			Touch_SetState(TS_WORKED);
			break;
	}

	Touch_GetCoords(&tp);
	
}
//==============================================================================




