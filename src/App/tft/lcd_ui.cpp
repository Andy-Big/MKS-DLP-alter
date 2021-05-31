#include "includes.h"
#include "lcd_ui.h"
#include "config.h"



extern uint16_t			LCD_WIDTH;
extern uint16_t			LCD_HEIGHT;


extern LCDUI_FONT		font_fnt11;
extern LCDUI_FONT		font_fnt11bold;
extern LCDUI_FONT		font_fnt12;
extern LCDUI_FONT		font_fnt12bold;
extern LCDUI_FONT		font_fnt14;
extern LCDUI_FONT		font_fnt14bold;
extern LCDUI_FONT		font_fnt18;
extern LCDUI_FONT		font_fnt18bold;
extern LCDUI_FONT		font_fnt24;
extern LCDUI_FONT		font_fnt24bold;
extern LCDUI_FONT		font_fnt36;
extern LCDUI_FONT		font_fnt36num;
extern LCDUI_FONT		font_fnt170num_lcd;


//uint8_t 		lcdui_startH, lcdui_endH, lcdui_startV, lcdui_endV, lcdui_ramAddrOne, lcdui_ramAddrTwo;
int16_t			lcdui_cursor_x = 0, lcdui_cursor_y = 0;
LCDUI_FONT		*lcdui_current_font = &font_fnt18;
LCDUI_FONT_TYPE	lcdui_current_font_type = LCDUI_FONT_H18;
uint16_t		lcdui_bgcolor = LCDUI_RGB(0x000000), lcdui_color = LCDUI_RGB(0xFFFFFF);





void	LCDUI_Init()
{
	lcdui_cursor_x = 0;
	lcdui_cursor_y = 0;
	lcdui_current_font = &font_fnt18;
	lcdui_bgcolor = LCDUI_RGB(0x000000);
	lcdui_color = LCDUI_RGB(0xFFFFFF);
}
//==============================================================================




void	_lcdui_DrawCircle_helper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername)
{
	int16_t	f     = r;
	int16_t	ddF_x = 1;
	int16_t	ddF_y = -2 * r;
	int16_t	x     = 0;
	int16_t	y     = r;

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		if (cornername & 0x4)
		{
			LCDUI_DrawPixel(x0 + x, y0 + y);
			LCDUI_DrawPixel(x0 + y, y0 + x);
		}
		if (cornername & 0x2)
		{
			LCDUI_DrawPixel(x0 + x, y0 - y);
			LCDUI_DrawPixel(x0 + y, y0 - x);
		}
		if (cornername & 0x8)
		{
			LCDUI_DrawPixel(x0 - y, y0 + x);
			LCDUI_DrawPixel(x0 - x, y0 + y);
		}
		if (cornername & 0x1)
		{
			LCDUI_DrawPixel(x0 - y, y0 - x);
			LCDUI_DrawPixel(x0 - x, y0 - y);
		}
	}
}
//==============================================================================





void	_lcdui_FillCircle_helper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta)
{
	int16_t 	f     = r;
	int16_t 	ddF_x = 1;
	int16_t 	ddF_y = -2 * r;
	int16_t 	x     = 0;
	int16_t 	y     = r;

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		if (cornername & 0x1)
		{
			LCDUI_DrawFastVLine(x0+x, y0-y, 2*y+1+delta);
			LCDUI_DrawFastVLine(x0+y, y0-x, 2*x+1+delta);
		}
		if (cornername & 0x2)
		{
			LCDUI_DrawFastVLine(x0-x, y0-y, 2*y+1+delta);
			LCDUI_DrawFastVLine(x0-y, y0-x, 2*x+1+delta);
		}
	}
}
//==============================================================================





uint16_t	LCDUI_SetColor(uint16_t color)
{
	uint16_t oldcolor = lcdui_color;
	lcdui_color = color;
	return oldcolor;
}
//==============================================================================





uint16_t	LCDUI_SetBackColor(uint16_t color)
{
	uint16_t oldcolor = lcdui_bgcolor;
	lcdui_bgcolor = color;
	return oldcolor;
}
//==============================================================================





void	LCDUI_SetCursorCoord(int16_t x, int16_t y)
{
	if (x >= 0)
		lcdui_cursor_x = x;
	if (y >= 0)
		lcdui_cursor_y = y;
}
//==============================================================================





uint16_t	LCDUI_GetScreenWidth()
{
	return LCD_WIDTH;
}
//==============================================================================





uint16_t	LCDUI_GetScreenHeight()
{
	return LCD_HEIGHT;
}
//==============================================================================





uint16_t	LCDUI_GetCurrentCursorX()
{
	return lcdui_cursor_x;
}
//==============================================================================





uint16_t	LCDUI_GetCurrentCursorY()
{
	return lcdui_cursor_y;
}
//==============================================================================





uint16_t	LCDUI_GetCurrentColor()
{
	return lcdui_color;
}
//==============================================================================





uint16_t	LCDUI_GetCurrentBackColor()
{
	return lcdui_bgcolor;
}
//==============================================================================





void	LCDUI_Clear()
{
	LCD_Clear(lcdui_bgcolor);
	lcdui_cursor_y = 0;
	lcdui_cursor_x = 0;
}
//==============================================================================





void	LCDUI_DrawPixel(uint16_t x1, uint16_t y1)
{
	if((x1 >= LCD_WIDTH) || (y1 >= LCD_HEIGHT))
		return;

	LCD_SetCursor(x1, y1);
	LCD_WriteRAM_Prepare();
	LCD_WriteRAM(lcdui_color);

}
//==============================================================================




void	LCDUI_DrawFastVLine(int16_t x, int16_t y, int16_t h)
{
	if((x >= LCD_WIDTH) || (y >= LCD_HEIGHT))
		return;
	if((y+h) >= LCD_HEIGHT)
		h = LCD_HEIGHT-y;
	LCD_SetWindows(x, y, 1, h);
	LCD_WriteRAM_Prepare();
	while (h-- > 0)
		LCD_WriteRAM(lcdui_color);
//	LCD_SetWindows(0, 0, LCD_WIDTH-1, LCD_HEIGHT-1);
}
//==============================================================================





void	LCDUI_DrawFastHLine(int16_t x, int16_t y, int16_t w)
{
	if((x >= LCD_WIDTH) || (y >= LCD_HEIGHT))
		return;
	if((x+w) >= LCD_WIDTH)
		w = LCD_WIDTH-x;
	LCD_SetWindows(x, y, w, 1);
	LCD_WriteRAM_Prepare();
	while (w-- > 0)
		LCD_WriteRAM(lcdui_color);
//	LCD_SetWindows(0, 0, LCD_WIDTH-1, LCD_HEIGHT-1);
}
//==============================================================================




void	LCDUI_DrawRect(int16_t x, int16_t y, int16_t w, int16_t h)
{
	LCDUI_DrawFastHLine(x, y, w);
	LCDUI_DrawFastHLine(x, y+h, w);
	LCDUI_DrawFastVLine(x, y, h);
	LCDUI_DrawFastVLine(x+w, y, h);
}
//==============================================================================





void	LCDUI_FillRect(int16_t x, int16_t y, int16_t w, int16_t h)
{
	if((x >= LCD_WIDTH) || (y >= LCD_HEIGHT))
		return;
	if((x + w) >= LCD_WIDTH)
		w = LCD_WIDTH - x;
	if((y + h) >= LCD_HEIGHT)
		h = LCD_HEIGHT - y;

	LCD_SetWindows(x, y, w, h);
	LCD_WriteRAM_Prepare();
	int32_t cnt = h * w;
	while(cnt--)
	{
		LCD_WriteRAM(lcdui_color);
	}
//	LCDUI_set_window(0, 0, LCD_WIDTH-1, LCD_HEIGHT-1);
}
//==============================================================================





void	LCDUI_DrawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r)
{
	// smarter version
	LCDUI_DrawFastHLine(x+r, y, w-2*r); // Top
	LCDUI_DrawFastHLine(x+r, y+h, w-2*r); // Bottom
	LCDUI_DrawFastVLine(x, y+r, h-2*r); // Left
	LCDUI_DrawFastVLine(x+w, y+r, h-2*r); // Right
	// draw four corners
	_lcdui_DrawCircle_helper(x+r, y+r, r, 1);
	_lcdui_DrawCircle_helper(x+w-r, y+r, r, 2);
	_lcdui_DrawCircle_helper(x+w-r, y+h-r, r, 4);
	_lcdui_DrawCircle_helper(x+r, y+h-r, r, 8);
}
//==============================================================================





void	LCDUI_FillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r)
{
	// smarter version
	LCDUI_FillRect(x+r, y, w-2*r, h);
	// draw four corners
	_lcdui_FillCircle_helper(x+w-r-1, y+r, r, 1, h-2*r);
	_lcdui_FillCircle_helper(x+r, y+r, r, 2, h-2*r);
}
//==============================================================================





void	LCDUI_FillCircle(int16_t x0, int16_t y0, int16_t r)
{
	LCDUI_DrawFastVLine(x0, y0-r, 2*r);
	_lcdui_FillCircle_helper(x0, y0, r, 3, 0);
}
//==============================================================================





void	LCDUI_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r)
{
	int16_t f = r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;
	LCDUI_DrawPixel(x0 - r, y0); // top left
	LCDUI_DrawPixel(x0, y0 + r); // top right
	LCDUI_DrawPixel(x0, y0-  r); // bottom right
	LCDUI_DrawPixel(x0 + r, y0); // bottom left

	while (x<y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		LCDUI_DrawPixel(x0 + x, y0 + y);
		LCDUI_DrawPixel(x0 - x, y0 + y);
		LCDUI_DrawPixel(x0 + x, y0 - y);
		LCDUI_DrawPixel(x0 - x, y0 - y);
		LCDUI_DrawPixel(x0 + y, y0 + x);
		LCDUI_DrawPixel(x0 - y, y0 + x);
		LCDUI_DrawPixel(x0 + y, y0 - x);
		LCDUI_DrawPixel(x0 - y, y0 - x);
	}
}
//==============================================================================




/*
void	LCDUI_FillRect(int16_t x, int16_t y, int16_t w, int16_t h)
{
	if((x >= LCD_WIDTH) || (y >= LCD_HEIGHT))
		return;
	if((x + w) >= LCD_WIDTH)
		w = LCD_WIDTH - x;
	if((y + h) >= LCD_HEIGHT)
		h = LCD_HEIGHT - y;

	LCDUI_set_window(x, y, x+w-1, y+h-1);
	LCDUI_write_reg_beg(ILI9225_GRAM_DATA_REG);
	h *= w;
	for(y = 0; y < h; y++)
	{
		LCDUI_write_data_cont(lcdui_color);
	}
	LCDUI_write_data_close();
	LCDUI_set_window(0, 0, LCD_WIDTH-1, LCD_HEIGHT-1);
}
//==============================================================================





void	LCDUI_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	if (y0 == y1)
	{
		if (x1 > x0)
			LCDUI_DrawFastHLine(x0, y0, x1 - x0);
		else
			if (x1 < x0)
				LCDUI_DrawFastHLine(x1, y0, x0 - x1);
			else
				LCDUI_DrawPixel(x0, y0);
		return;
	}
	else
		if (x0 == x1)
		{
			if (y1 > y0)
				LCDUI_DrawFastVLine(x0, y0, y1 - y0);
			else
				LCDUI_DrawFastVLine(x0, y1, y0 - y1);
			return;
		}
	bool steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep)
	{
//		swap(x0, y0);
		int16_t s = x0;
		x0 = y0;
		y0 = s;
//		swap(x1, y1);
		s = x1;
		x1 = y1;
		y1 = s;
	}
	if (x0 > x1)
	{
//		swap(x0, x1);
		int16_t s = x0;
		x0 = x1;
		x1 = s;
//		swap(y0, y1);
		y0 = y1;
		y1 = s;
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);
	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1)
		ystep = 1;
	else
		ystep = -1;

	int16_t xbegin = x0;
	if (steep)
	{
		for (; x0<=x1; x0++)
		{
			err -= dy;
			if (err < 0)
			{
				int16_t len = x0 - xbegin;
				if (len)
					LCDUI_DrawFastVLine(y0, xbegin, len + 1);
				else
					LCDUI_DrawPixel(y0, x0);
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1)
		LCDUI_DrawFastVLine(y0, xbegin, x0 - xbegin);
	}
	else
	{
		for (; x0<=x1; x0++)
		{
			err -= dy;
			if (err < 0)
			{
				int16_t len = x0 - xbegin;
				if (len)
					LCDUI_DrawFastHLine(xbegin, y0, len + 1);
				else
					LCDUI_DrawPixel(x0, y0);
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1)
			LCDUI_DrawFastHLine(xbegin, y0, x0 - xbegin);
	}
}
//==============================================================================





*/
uint16_t	_lcdui_GetCharWidth(uint8_t c)
{
	if (c < 32)
		return 0;
	if (c > 126)
		c -= 65;
	c -= 32;
	if (c >= lcdui_current_font->symcount)
		return 0;
	uint16_t res = lcdui_current_font->width[c];
	if (res & 0x8000)
		return lcdui_current_font->width[(res & 0x7FFF)];
	return res;
}
//==============================================================================





uint8_t*	_lcdui_GetCharData(uint8_t c)
{
	if (c < 32)
		return 0;
	if (c > 126)
		c -= 65;
	c -= 32;
	if (c >= lcdui_current_font->symcount)
		return 0;
	uint16_t c1 = lcdui_current_font->width[c];
	if (c1 & 0x8000)
		c = (c1 & 0x7FFF);
	uint16_t ch = lcdui_current_font->height;
	int32_t i = 0, ptr = 0, bits = 0, line_bits = ch;
	for (i = 0; i < c; i++)
	{
		if (lcdui_current_font->width[i] & 0x8000)
			continue;
		bits = lcdui_current_font->width[i] * line_bits;
		ptr += bits >> 3;
		if (bits & 0x07)
			ptr++;
	}

	return &(lcdui_current_font->data[ptr]);
}
//==============================================================================




LCDUI_FONT_TYPE		LCDUI_SetFont(LCDUI_FONT_TYPE newfont)
{
	switch (newfont)
	{
		case LCDUI_FONT_H11:
			lcdui_current_font = &font_fnt11;
			break;

		case LCDUI_FONT_H11BOLD:
			lcdui_current_font = &font_fnt11bold;
			break;

		case LCDUI_FONT_H12:
			lcdui_current_font = &font_fnt12;
			break;

		case LCDUI_FONT_H12BOLD:
			lcdui_current_font = &font_fnt12bold;
			break;

		case LCDUI_FONT_H14:
			lcdui_current_font = &font_fnt14;
			break;

		case LCDUI_FONT_H14BOLD:
			lcdui_current_font = &font_fnt14bold;
			break;

		case LCDUI_FONT_H18:
			lcdui_current_font = &font_fnt18;
			break;

		case LCDUI_FONT_H18BOLD:
			lcdui_current_font = &font_fnt18bold;
			break;

		case LCDUI_FONT_H24:
			lcdui_current_font = &font_fnt24;
			break;

		case LCDUI_FONT_H24BOLD:
			lcdui_current_font = &font_fnt24bold;
			break;

		case LCDUI_FONT_H36:
			lcdui_current_font = &font_fnt36;
			break;

		case LCDUI_FONT_H36NUM:
			lcdui_current_font = &font_fnt36num;
			break;

		case LCDUI_FONT_H170NUM_LCD:
			lcdui_current_font = &font_fnt170num_lcd;
			break;

		default:
			return lcdui_current_font_type;
	}
	LCDUI_FONT_TYPE oldfont = lcdui_current_font_type;
	lcdui_current_font_type = newfont;
	return oldfont;
}
//==============================================================================





LCDUI_FONT_TYPE		LCDUI_GetCurrentFont()
{
	return lcdui_current_font_type;
}
//==============================================================================




/*
uint32_t	LCDUI_GetTextWidth(char *str)
{
	uint32_t i = 0, res = 0;
	char c;
	while(c = str[i])
	{
		if (c > 31)
			res += _lcdui_GetCharWidth(c);
		i++;
	}
	return res;
}
//==============================================================================
*/




uint32_t	LCDUI_GetTextWidth(char *str)
{
	uint32_t i = 0, res = 0;
	char c;
	while(str[i])
	{
		c =  UTF8toANSI(str+i);
		if (c > 31)
			res += _lcdui_GetCharWidth(c);
		if (str[i] < 0x80)
			i++;
		else
			i += 2;
	}
	return res;
}
//==============================================================================





uint32_t	LCDUI_GetCurrentFontHeight()
{
	return lcdui_current_font->height;
}
//==============================================================================





void	LCDUI_DrawChar(char c,  uint16_t opt, int16_t x, int16_t y)
{
	if((x > LCD_WIDTH) || (y > LCD_HEIGHT) || c < 32)
	{
		return;
	}
	if (x >= 0)
		lcdui_cursor_x = x;
	if (y >= 0)
		lcdui_cursor_y = y;
	if((lcdui_cursor_x > LCD_WIDTH) || (lcdui_cursor_y > LCD_HEIGHT))
		return;

	uint16_t	cres[2];

	cres[1] = lcdui_color;
	cres[0] = lcdui_bgcolor;
	uint16_t	cw = 0, ch = lcdui_current_font->height;
	uint16_t	i = 0, ptr = 0;
	cw = _lcdui_GetCharWidth(c);
	if (cw == 0)
		return;
	uint8_t *data = _lcdui_GetCharData(c);
	if (data == 0)
		return;

	c = data[ptr++];
	uint8_t 	sh = 0, csh = 0;
	uint16_t	xc = lcdui_cursor_x, yc = lcdui_cursor_y;
	
//	LCD_SetCursor(lcdui_cursor_x, lcdui_cursor_y);
	LCD_SetWindows(lcdui_cursor_x, lcdui_cursor_y, cw, ch);
	LCD_WriteRAM_Prepare();
	
	if (opt & LCDUI_TEXT_TRANSBACK)
	{
		for (i = 0; i < cw*ch; i++)
		{
			csh = (c >> sh);
			if (csh&0x01)
			{
				LCD_SetCursor(xc, yc);
				LCD_WriteRAM_Prepare();
				LCD_WriteRAM(lcdui_color);
			}
			xc++;
			if (xc == lcdui_cursor_x+cw)
			{
				xc = lcdui_cursor_x;
				yc++;
			}
			
			sh ++;
			if (sh == 8)
			{
				c = data[ptr++];
				sh = 0;
			}
		}
	}
	else
	{
		for (i = 0; i < cw*ch; i++)
		{
			csh = (c >> sh);
			LCD_WriteRAM(cres[(csh&0x01)]);
			
			sh ++;
			if (sh == 8)
			{
				c = data[ptr++];
				sh = 0;
			}
		}
	}
/*
	if (opt & LCDUI_TEXT_UNDERLINE)
	{
		LCDUI_write_reg_beg(lcdui_ramAddrOne);
		LCDUI_write_data_last(lcdui_cursor_x);
		LCDUI_write_reg_beg(lcdui_ramAddrTwo);
		LCDUI_write_data_last(lcdui_cursor_y+ch-1);
		LCDUI_write_reg_beg(ILI9225_GRAM_DATA_REG);
		for (k = 0; k < cw; k++)
		{
			LCDUI_write_data_cont(lcdui_color);
		}
		LCDUI_write_data_close();
	}

	if (opt & LCDUI_TEXT_OVERLINE)
	{
		LCDUI_write_reg_beg(lcdui_ramAddrOne);
		LCDUI_write_data_last(lcdui_cursor_x);
		LCDUI_write_reg_beg(lcdui_ramAddrTwo);
		LCDUI_write_data_last(lcdui_cursor_y+ch/2);
		LCDUI_write_reg_beg(ILI9225_GRAM_DATA_REG);
		for (k = 0; k < cw; k++)
		{
			LCDUI_write_data_cont(lcdui_color);
		}
		LCDUI_write_data_close();
	}
*/
	lcdui_cursor_x += cw;
}
//==============================================================================





void	LCDUI_DrawText(char *str, uint16_t opt, int16_t x1, int16_t y1, int16_t x2, int16_t y2, TSIZE *tsize)
{
	uint16_t i = 0, oldcolor = lcdui_color, oldbgcolor = lcdui_bgcolor, copt = opt;
	char c;
	if (x1 > LCD_WIDTH-1 || y1 > LCD_HEIGHT-1)
		return;
	if (x1 < 0)
		x1 = lcdui_cursor_x;
	if (y1 < 0)
		y1 = lcdui_cursor_y;
	if (x2 < 0)
		x2 = LCD_WIDTH-1;
	if (y2 < 0)
		y2 = LCD_HEIGHT-1;
	if ((opt & LCDUI_TEXT_GETSIZE) == 0)
	{
		lcdui_cursor_x = x1;
		lcdui_cursor_y = y1;
	}



	char *cp = (char*)str;
	int16_t sp = 0, cw = 0, sw = 0;

	while(1)
	{
		c = UTF8toANSI(cp+i);
		if (c == 0 || c == ' ')
		{
			sp = i;
			sw = cw;
		}
		cw += _lcdui_GetCharWidth(c);
		if (c == '\n' || cw > x2-x1 || c == 0)
		{
			if (cw > x2-x1)
				cw = sw;
			if (sp == 0)
				sp = i;
			if (c == '\n')
				sp = i;
			if (tsize != NULL && tsize->x_size < cw)
				tsize->x_size = cw;
			if (tsize != NULL && cw > 0)
				tsize->y_size += lcdui_current_font->height;
			
			if ((opt & LCDUI_TEXT_GETSIZE) == 0)
			{
				if (opt & LCDUI_TEXT_ALIGN_RIGHT)
					lcdui_cursor_x = x2 - cw;
				if (opt & LCDUI_TEXT_ALIGN_CENTER)
					lcdui_cursor_x = x1 + (x2 - x1 - cw) / 2;
				for (uint16_t j = 0; j < sp; )
				{
					LCDUI_DrawChar(UTF8toANSI(cp+j), copt);
					if ((*(cp+j) & 0x80)==0)
						j++;
					else
						j += 2;
				}
			}
			cw = 0;
			sw = 0;
			cp += sp;
			i = 0;
			sp = 0;
			while(UTF8toANSI(cp) == ' ')
			{
				if ((*cp & 0x80)==0)
					cp++;
				else
					cp += 2;
			}
			if (c != 0 && (opt & LCDUI_TEXT_GETSIZE) == 0)
			{
				lcdui_cursor_y += lcdui_current_font->height;
				lcdui_cursor_x = x1;
			}
			if (c == 0 || lcdui_cursor_y+lcdui_current_font->height > y2)
				break;
			if (c == '\n')
				i++;;
			continue;
		}
		if ((*(cp+i) & 0x80)==0)
			i++;
		else
			i += 2;
		if (c == '.' || c == ',' || c == ':' || c == ';' || c == '!' || c == '?' || c == '_' || c == '-' || c == '(' || c == ')' || c == '_')
		{
//			cw += _lcdui_GetCharWidth(c);
			sp = i;
			sw = cw;
		}

	}

	lcdui_bgcolor = oldbgcolor;
	lcdui_color = oldcolor;
}
//==============================================================================





void	LCDUI_DrawCharUTF(char *c,  uint16_t opt, int16_t x, int16_t y)
{
	char cc = UTF8toANSI(c);
	LCDUI_DrawChar(cc, opt, x, y);
}
//==============================================================================




/*
void	LCDUI_DrawTextUTF(char *str, uint16_t opt, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	uint16_t i = 0, oldcolor = lcdui_color, oldbgcolor = lcdui_bgcolor, copt = opt;
	char c;
	if (x1 > LCD_WIDTH-1 || y1 > LCD_HEIGHT-1)
		return;
	if (x1 < 0)
		x1 = lcdui_cursor_x;
	if (y1 < 0)
		y1 = lcdui_cursor_y;
	if (x2 < 0)
		x2 = LCD_WIDTH-1;
	if (y2 < 0)
		y2 = LCD_HEIGHT-1;
	lcdui_cursor_x = x1;
	lcdui_cursor_y = y1;



	char *cp = (char*)str;
	int16_t sp = 0, cw = 0, sw = 0;

	while(1)
	{
		c = UTF8toANSI(cp+i);
		if (c == 0 || c == ' ')
		{
			sp = i;
			sw = cw;
		}
		cw += _lcdui_GetCharWidth(c);
		if (c == '\n' || cw > x2-x1 || c == 0)
		{
			if (cw > x2-x1)
				cw = sw;
			if (sp == 0)
				sp = i;
			if (c == '\n')
				sp = i;
			if (opt & LCDUI_TEXT_ALIGN_RIGHT)
				lcdui_cursor_x = x2 - cw;
			if (opt & LCDUI_TEXT_ALIGN_CENTER)
				lcdui_cursor_x = x1 + (x2 - x1 - cw) / 2;
			for (uint16_t j = 0; j < sp;)
			{
				LCDUI_DrawChar(UTF8toANSI(cp+j), copt);
				if (*(cp+j) < 0x80)
					j++;
				else
					j += 2;
			}
			cw = 0;
			sw = 0;
			cp += sp;
			i = 0;
			sp = 0;
			while(UTF8toANSI(cp) == ' ')
			{
				if (*cp < 0x80)
					cp++;
				else
					cp += 2;
			}
			if (c != 0)
			{
				lcdui_cursor_y += lcdui_current_font->height;
				lcdui_cursor_x = x1;
			}
			if (c == 0 || lcdui_cursor_y+lcdui_current_font->height > y2)
				break;
			if (c == '\n')
				i++;;
			continue;
		}
		if (*((uint8_t*)cp+i) < 0x80)
			i++;
		else
			i += 2;
		if (c == '.' || c == ',' || c == ':' || c == ';' || c == '!' || c == '?' || c == '_' || c == '-' || c == '(' || c == ')')
		{
//			cw += _lcdui_GetCharWidth(c);
			sp = i;
			sw = cw;
		}

	}

	lcdui_bgcolor = oldbgcolor;
	lcdui_color = oldcolor;
}
//==============================================================================
*/




void		LCDUI_DrawBitmap(uint16_t x, uint16_t y, uint8_t *bmp)
{
	if (x >= LCD_WIDTH || y >= LCD_HEIGHT || bmp == NULL)
		return;
	
	uint8_t 	*pbmp = bmp;
	uint32_t	bwidth = *(uint32_t*)(pbmp+18);
	uint32_t	bheight = *(uint32_t*)(pbmp+22);
	if (bheight & 0x80000000)
	{
		bheight = 0xFFFFFFFF - bheight + 1;
	}
	else
	{
		LCD_WriteCmd(0x0036);
		if (cfgConfig.display_rotate == 0)
			LCD_WriteRAM(0x00F8);
		else
			LCD_WriteRAM(0x0038);
	}
	uint32_t	linesize = bwidth * 2;
	while (linesize % 4)
	{
		linesize++;
	}
	
	uint16_t paintx = bwidth;
	if (paintx + x > LCD_WIDTH)
		paintx = LCD_WIDTH - x;
	uint16_t painty = bheight;
	if (paintx + x > LCD_HEIGHT)
		painty = LCD_HEIGHT - y;

	// image data offset
	uint32_t index = *(uint32_t *)(pbmp + 10);
	pbmp += index;

	LCD_SetWindows(x, y, x + paintx, y + painty);
	LCD_WriteRAM_Prepare();
	
	for (uint16_t iy = 0; iy < painty; iy++)
	{
		for (uint16_t ix = 0; ix < paintx; ix++)
		{
			LCD_WriteRAM(*(uint16_t*)(pbmp + ix*2));
		}
		pbmp += linesize;
	}
	if (!(bheight & 0x80000000))
	{
		LCD_WriteCmd(0x0036);
		if (cfgConfig.display_rotate == 0)
			LCD_WriteRAM(0x00B8);
		else
			LCD_WriteRAM(0x0078);
	}
}
//==============================================================================





