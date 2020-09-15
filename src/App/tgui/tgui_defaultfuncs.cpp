/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "tgui_defaultfuncs.h"
#include "datetime.h"
#include "sys_timer.h"
#include "config.h"




// period in ms of timer for check to repaint time on main screen
#define		MS_SECTIMER_PERIOD			500


uint8_t		scrTimeTimer = INVALID_TIMER;



void		_tgui_ScreenTimeInit()
{
	if (scrTimeTimer == INVALID_TIMER)
		scrTimeTimer = SYSTIMER_NewCountDown(MS_SECTIMER_PERIOD);
	else
		SYSTIMER_SetCountDown(scrTimeTimer, MS_SECTIMER_PERIOD);
}
//==============================================================================




void		_tgui_ScreenTimeProcess(void *tguiobj, void *param)
{
	static uint8_t		minutes = 0;

	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;

	// if minutes changed
	if (SYSTIMER_GetCountDown(scrTimeTimer) == 0)
	{
		SYSTIMER_SetCountDown(scrTimeTimer, MS_SECTIMER_PERIOD);
		uint8_t newminutes = DTIME_GetMinutes();
		if (newminutes != minutes)
		{
			minutes = newminutes;
			thisbtn->options.needrepaint = 1;
		}
	}
}
//==============================================================================




void		_tgui_ScreenTimePaint(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	
	DATETIME_STRUCT dt;
	DTIME_GetCurrentDatetime(&dt);
	
	char *mshortname;
	switch (dt.month)
	{
		case 2:
			mshortname = LANG_GetString(LSTR_FEBRUARY);
			break;
		case 3:
			mshortname = LANG_GetString(LSTR_MARCH);
			break;
		case 4:
			mshortname = LANG_GetString(LSTR_APRIL);
			break;
		case 5:
			mshortname = LANG_GetString(LSTR_MAY);
			break;
		case 6:
			mshortname = LANG_GetString(LSTR_JUNE);
			break;
		case 7:
			mshortname = LANG_GetString(LSTR_JULY);
			break;
		case 8:
			mshortname = LANG_GetString(LSTR_AUGUST);
			break;
		case 9:
			mshortname = LANG_GetString(LSTR_SEPTEMBER);
			break;
		case 10:
			mshortname = LANG_GetString(LSTR_OCTOBER);
			break;
		case 11:
			mshortname = LANG_GetString(LSTR_NOVEMBER);
			break;
		case 12:
			mshortname = LANG_GetString(LSTR_DECEMBER);
			break;
		default:
			mshortname = LANG_GetString(LSTR_JANUARY);
			break;
	}
	
	char	msg[32];
	sprintf(msg, "%02u:%02u %u %s", dt.hours, dt.minutes, dt.date, mshortname);
	thisbtn->text = (LNG_STRING_ID)((DWORD)&msg);
	_tgui_DefaultButtonPaint(tguiobj, NULL);
	thisbtn->text = LSTR____;
}
//==============================================================================




void		_tgui_DrawStringInRect(char *str, TG_RECT *rect, TG_TEXTOPTIONS *opt)
{
	if (str == NULL)
		return;
	int16_t tx = rect->left;
	int16_t ty = rect->top;
	int16_t tw = 0;
	int16_t th = 0;
	if (opt->textalign_h == HTA_CENTER)
	{
		tw = LCDUI_GetTextWidth(str);
		tx += ((rect->right - tx) - tw) / 2;
	}
	else
	{
		if (opt->textalign_h == HTA_RIGHT)
		{
			tw = LCDUI_GetTextWidth(str);
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
			th = LCDUI_GetTextWidth(str);
			ty = (rect->bottom - th) + th / 8;
		}
	}
	LCDUI_DrawText(str, LCDUI_TEXT_TRANSBACK, tx, ty);
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
		if (cfgConfig.display_rotate == 0)
			LCD_WriteRAM(0x00F8);
		else
			LCD_WriteRAM(0x0038);
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
		if (cfgConfig.display_rotate == 0)
			LCD_WriteRAM(0x00B8);
		else
			LCD_WriteRAM(0x0078);
	}
closeexit:
	f_close(&tguiFile);
	return;
}
//==============================================================================



void		_tgui_DrawFileRawImg(FIL *file, int16_t x, int16_t y, uint16_t bwidth, uint16_t bheight, uint32_t imgbase, uint8_t fliprows)
{
	DWORD		freaded = imgbase;
	DWORD		readed = 0;
	uint16_t	bufinpos = UIFBUFF_SIZE;
	uint16_t	bufoutpos = 0;
	uint8_t		len = 0;
	uint16_t	val = 0;
	uint8_t		curbuffnum = 0;
	uint16_t	*curbuff = (uint16_t*)tguiDBuff;
	
	if (fliprows)
	{
		LCD_WriteCmd(0x0036);
		if (cfgConfig.display_rotate == 0)
			LCD_WriteRAM(0x00F8);
		else
			LCD_WriteRAM(0x0038);
		y = LCD_HEIGHT - y - bheight;
	}

	LCD_SetWindows(x, y, bwidth, bheight);
	LCD_WriteRAM_Prepare();
	

	do
	{
		if (bufinpos == UIFBUFF_SIZE)
		{
			if (f_read(file, tguiFBuff, UIFBUFF_SIZE, &readed) != FR_OK)
			{
				break;
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
				f_lseek(file, freaded);
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
				f_lseek(file, freaded);
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

	if (fliprows)
	{
		LCD_WriteCmd(0x0036);
		if (cfgConfig.display_rotate == 0)
			LCD_WriteRAM(0x00B8);
		else
			LCD_WriteRAM(0x0078);
	}
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
		if (cfgConfig.display_rotate == 0)
			LCD_WriteRAM(0x00F8);
		else
			LCD_WriteRAM(0x0038);
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
		if (cfgConfig.display_rotate == 0)
			LCD_WriteRAM(0x00B8);
		else
			LCD_WriteRAM(0x0078);
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
		if (cfgConfig.display_rotate == 0)
			LCD_WriteRAM(0x00F8);
		else
			LCD_WriteRAM(0x0038);
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
		if (cfgConfig.display_rotate == 0)
			LCD_WriteRAM(0x00B8);
		else
			LCD_WriteRAM(0x0078);
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
	uint32_t	readed = 0;
	uint32_t	bwidth = 0;
	uint32_t	bheight = 0;
	uint8_t		fliprows = 0;
	uint32_t	imgbase;
	

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
	}
	
	
	// Image data offset
	imgbase = 4;

	if (f_lseek(&tguiFile, imgbase) != FR_OK)
	{
		goto closeexit;
	}

	_tgui_DrawFileRawImg(&tguiFile, 0, 0, bwidth, bheight, imgbase, fliprows);

closeexit:
	f_close(&tguiFile);
	return;
}
//==============================================================================
/**/



void		_tgui_DrawFileCimg(char* file, int16_t x, int16_t y)
{
	uint32_t	readed = 0;
	uint32_t	bwidth = 0;
	uint32_t	bheight = 0;
	uint8_t		fliprows = 0;
	uint32_t	imgbase;
	

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
	}
	
	
	// Image data offset
	imgbase = 4;

	if (f_lseek(&tguiFile, imgbase) != FR_OK)
	{
		goto closeexit;
	}

	_tgui_DrawFileRawImg(&tguiFile, x, y, bwidth, bheight, imgbase, fliprows);

closeexit:
	f_close(&tguiFile);
	return;
}
//==============================================================================



void		_tgui_DefaultButtonPaint(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	
	uint16_t oldcolor, newcolor = thisbtn->textcolor_en;
	uint16_t oldbackcolor, newbackcolor = thisbtn->backcolor_en;
	LCDUI_FONT_TYPE oldfont = LCDUI_SetFont(thisbtn->font);
	char *img = thisbtn->bgimagename_en;

	// colors
	if (thisbtn->options.disabled == 1)
	{
		newcolor = thisbtn->textcolor_dis;
		newbackcolor = thisbtn->backcolor_dis;
		img = thisbtn->bgimagename_dis;
	}
	else
	{
		if (thisbtn->options.pressed == 1 && thisbtn->options.repaintonpress == 1)
		{
			newcolor = thisbtn->textcolor_press;
			newbackcolor = thisbtn->backcolor_press;
			img = thisbtn->bgimagename_press;
		}
		else
		{
			if (thisbtn->options.active == 1)
			{
				newcolor = thisbtn->textcolor_act;
				newbackcolor = thisbtn->backcolor_act;
				img = thisbtn->bgimagename_act;
			}
		}
	}

	oldcolor = LCDUI_SetColor(newcolor);
	oldbackcolor = LCDUI_SetBackColor(newbackcolor);

	if (thisbtn->options.bgpaint == BGP_FILL)
	{
		LCDUI_SetColor(newbackcolor);
		LCDUI_FillRoundRect(thisbtn->position.left, thisbtn->position.top, thisbtn->position.right - thisbtn->position.left, thisbtn->position.bottom - thisbtn->position.top, 7);
		LCDUI_SetColor(newcolor);
	}
	else
	{
		if (thisbtn->options.bgpaint == BGP_SCREEN)
		{
		}
		else
		{
			if (thisbtn->options.bgpaint == BGP_IMAGE && img != NULL)
			{
				_tgui_DrawFileCimg(img, thisbtn->position.left, thisbtn->position.top);
			}
		}
	}
	
	if (thisbtn->text != NULL)
	{
		char		*txt;
		// if "text" field is a pointer to memory
		if (thisbtn->text & 0xFF000000)
			txt = (char*)thisbtn->text;
		else
			txt = LANG_GetString(thisbtn->text);
		_tgui_DrawStringInRect(txt, &(thisbtn->textposition), &(thisbtn->textoptions));
	}
	

	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================




void		_tgui_DefaultButtonProcess(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	TOUCH_STATES	*ts = (TOUCH_STATES*)param;;
	TOUCH_POINT		tp;
	
	if (*ts > TS_PREPRESSED)
	{
		Touch_GetCoords(&tp);
		switch (*ts)
		{
			case TS_SPRESSED:
				if (thisbtn->options.disabled == 0 && thisbtn->options.pressed == 0)
				{
					if (TGUI_PointInRect(&tp, &thisbtn->position) == 1)
					{
						BUZZ_TimerOn(cfgConfig.buzzer_touch);
						thisbtn->options.pressed = 1;
						if (thisbtn->options.repaintonpress == 1 && thisbtn->funcs._call_paint != NULL)
						{
							thisbtn->funcs._call_paint((void*)thisbtn, NULL);
							thisbtn->options.needrepaint = 0;
						}
						Touch_SetWorked(TS_SPRESSED);
					}
				}
				break;

			case TS_LPRESSED:
				Touch_SetWorked(TS_LPRESSED);
				break;

			case TS_SRELEASED:
				if (thisbtn->options.disabled == 0 && thisbtn->options.pressed == 1)
				{
					thisbtn->options.pressed = 0;
					// ------- group switches process --------
					// single selection from group (select)
					if (thisbtn->group_id > 0)
					{
						TG_SCREEN		*scr = (TG_SCREEN*)thisbtn->parentscreen;
						for (uint8_t i = 0; i < scr->btns_count; i++)
						{
							// search other active button fron this group and deactivate it
							if (scr->buttons[i].group_id == thisbtn->group_id && scr->buttons[i].options.active)
							{
								scr->buttons[i].options.active = 0;
								if (scr->buttons[i].options.disabled == 0 && scr->buttons[i].funcs._call_paint != NULL)
									scr->buttons[i].funcs._call_paint((void*)&(scr->buttons[i]), NULL);
							}
						}
						thisbtn->options.active = 1;
					}
					// miltiple selection (switch)
					if (thisbtn->group_id < 0)
					{
						thisbtn->options.active = !thisbtn->options.active;
					}
					// paint
					if (thisbtn->options.repaintonpress == 1 && thisbtn->funcs._call_paint != NULL)
					{
						thisbtn->funcs._call_paint((void*)thisbtn, NULL);
						thisbtn->options.needrepaint = 0;
					}
					// call linked function or predefined action
					if (thisbtn->funcs._call_press != NULL)
					{
						if (tguiActiveScreen == (TG_SCREEN*)&tguiMsgBox)
						{
							if (thisbtn->funcs._call_press == (pressfunc)BTNA_GOPREVSCR)
							{
								thisbtn->options.pressed = 0;
								tguiActiveScreen = (TG_SCREEN*)((TG_MSGBOX*)tguiActiveScreen)->prevscreen;
								TGUI_ForceRepaint();
							}
							else
							{
								// call linked function or predefined action
								thisbtn->funcs._call_press((void*)thisbtn, NULL);
							}
						}
						else
						{
							if (thisbtn->funcs._call_press == (pressfunc)BTNA_GOCHILDSCR && thisbtn->childscreen != NULL)
							{
								// go to child screen
								thisbtn->options.pressed = 0;
								TG_SCREEN		*curscreen = tguiActiveScreen;
								tguiActiveScreen = (TG_SCREEN*)thisbtn->childscreen;
								tguiActiveScreen->prevscreen = curscreen;
								TGUI_ForceRepaint();
							}
							else
							{
								// back to previous screen
								if (thisbtn->funcs._call_press == (pressfunc)BTNA_GOPREVSCR && tguiActiveScreen->prevscreen != NULL)
								{
									thisbtn->options.pressed = 0;
									tguiActiveScreen = (TG_SCREEN*)tguiActiveScreen->prevscreen;
									TGUI_ForceRepaint();
								}
								else
								{
									// call linked function or predefined action
									thisbtn->funcs._call_press((void*)thisbtn, NULL);
								}
							}
						}
					}
					
					Touch_SetWorked(TS_SRELEASED);
				}
				break;

			case TS_LRELEASED:
				if (thisbtn->options.disabled == 0 && thisbtn->options.pressed == 1)
				{
					thisbtn->options.pressed = 0;
					if (thisbtn->options.repaintonpress == 1 && thisbtn->funcs._call_paint != NULL)
					{
						thisbtn->funcs._call_paint((void*)thisbtn, NULL);
						thisbtn->options.needrepaint = 0;
					}
					Touch_SetWorked(TS_LRELEASED);
				}
				break;
		}
	}
}
//==============================================================================




void		_tgui_DefaultScreenPaint(void *tguiobj, void *param)
{
	TG_SCREEN		*thisscr = (TG_SCREEN*)tguiobj;
	
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
	char		*txt;
	// if "name" field is a pointer to memory
	if (thisscr->name & 0xFF000000)
		txt = (char*)thisscr->name;
	else
		txt = LANG_GetString(thisscr->name);
	_tgui_DrawStringInRect(txt, &(thisscr->nameposition), &(thisscr->nameoptions));

	for (uint8_t i = 0; i < thisscr->btns_count; i++)
	{
		if (thisscr->buttons[i].funcs._call_paint != NULL)
			thisscr->buttons[i].funcs._call_paint(&(thisscr->buttons[i]), 0);
		thisscr->buttons[i].options.needrepaint = 0;
	}

	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================




void		_tgui_DefaultScreenProcess(void *tguiobj, void *param)
{
	TG_SCREEN		*thisscr = (TG_SCREEN*)tguiobj;
	TOUCH_STATES	ts = Touch_GetState();
	
	for (uint8_t i = 0; i < thisscr->btns_count; i++)
	{
		if (thisscr->buttons[i].options.disabled == 0 && thisscr->buttons[i].funcs._call_process != NULL)
			thisscr->buttons[i].funcs._call_process(&(thisscr->buttons[i]), (void*)&ts);
		
		// if active screen has been not changed and item need to repaint
		if (tguiActiveScreen == thisscr)
		{
			if (thisscr->buttons[i].options.needrepaint)
			{
				if (thisscr->buttons[i].funcs._call_paint != NULL)
				{
					thisscr->buttons[i].funcs._call_paint(&(thisscr->buttons[i]), NULL);
					thisscr->buttons[i].options.needrepaint = 0;
				}
			}
		}
		else
		{
			// current screen has been changed, no more need to check buttons
			break;
		}
	}


}
//==============================================================================




