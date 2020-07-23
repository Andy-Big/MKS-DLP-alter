#ifndef LCDUI_H
#define LCDUI_H

#include "includes.h"



// ****************** fonts definitions *******************
typedef enum
{
	LCDUI_FONT_H18 = 0,
	LCDUI_FONT_H18BOLD,
	LCDUI_FONT_H24,
	LCDUI_FONT_H24BOLD,
} LCDUI_FONT_TYPE;






/* RGB 24-bits color table definition (RGB888). */
#define LCDUI_RGB(color) ((((color) >> 19) & 0x1f) << 11) | ((((color) >> 10) & 0x3f) << 5) | (((color) >> 3) & 0x1f)
#define LCDUI_R_G_B(r, g, b) ((((r) >> 3) & 0x1f) << 11) | ((((g) >> 2) & 0x3f) << 5) | (((b) >> 3) & 0x1f)

#define COLOR_BLACK          LCDUI_RGB(0x000000u)
#define COLOR_WHITE          LCDUI_RGB(0xFFFFFFu)
#define COLOR_BLUE           LCDUI_RGB(0x0000FFu)
#define COLOR_GREEN          LCDUI_RGB(0x00FF00u)
#define COLOR_RED            LCDUI_RGB(0xFF0000u)
#define COLOR_NAVY           LCDUI_RGB(0x000080u)
#define COLOR_DARKBLUE       LCDUI_RGB(0x00008Bu)
#define COLOR_DARKGREEN      LCDUI_RGB(0x006400u)
#define COLOR_DARKCYAN       LCDUI_RGB(0x008B8Bu)
#define COLOR_CYAN           LCDUI_RGB(0x00FFFFu)
#define COLOR_TURQUOISE      LCDUI_RGB(0x40E0D0u)
#define COLOR_INDIGO         LCDUI_RGB(0x4B0082u)
#define COLOR_DARKRED        LCDUI_RGB(0x800000u)
#define COLOR_OLIVE          LCDUI_RGB(0x808000u)
#define COLOR_GRAY           LCDUI_RGB(0x808080u)
#define COLOR_SKYBLUE        LCDUI_RGB(0x87CEEBu)
#define COLOR_BLUEVIOLET     LCDUI_RGB(0x8A2BE2u)
#define COLOR_LIGHTGREEN     LCDUI_RGB(0x90EE90u)
#define COLOR_DARKVIOLET     LCDUI_RGB(0x9400D3u)
#define COLOR_YELLOWGREEN    LCDUI_RGB(0x9ACD32u)
#define COLOR_BROWN          LCDUI_RGB(0xA52A2Au)
#define COLOR_DARKGRAY       LCDUI_RGB(0xA9A9A9u)
#define COLOR_SIENNA         LCDUI_RGB(0xA0522Du)
#define COLOR_LIGHTBLUE      LCDUI_RGB(0xADD8E6u)
#define COLOR_GREENYELLOW    LCDUI_RGB(0xADFF2Fu)
#define COLOR_SILVER         LCDUI_RGB(0xC0C0C0u)
#define COLOR_LIGHTGREY      LCDUI_RGB(0xD3D3D3u)
#define COLOR_LIGHTCYAN      LCDUI_RGB(0xE0FFFFu)
#define COLOR_VIOLET         LCDUI_RGB(0xEE82EEu)
#define COLOR_AZUR           LCDUI_RGB(0xF0FFFFu)
#define COLOR_BEIGE          LCDUI_RGB(0xF5F5DCu)
#define COLOR_MAGENTA        LCDUI_RGB(0xFF00FFu)
#define COLOR_TOMATO         LCDUI_RGB(0xFF6347u)
#define COLOR_GOLD           LCDUI_RGB(0xFFD700u)
#define COLOR_ORANGE         LCDUI_RGB(0xFFA500u)
#define COLOR_SNOW           LCDUI_RGB(0xFFFAFAu)
#define COLOR_YELLOW         LCDUI_RGB(0xFFFF00u)


#define	LCDUI_TEXT_ALIGN_RIGHT		(uint16_t)0x0001
#define	LCDUI_TEXT_ALIGN_CENTER		(uint16_t)0x0002
#define	LCDUI_TEXT_UNDERLINE		(uint16_t)0x0004
#define	LCDUI_TEXT_OVERLINE			(uint16_t)0x0008
#define	LCDUI_TEXT_TRANSBACK		(uint16_t)0x0010




void		LCDUI_DrawCircle_helper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername);
void		LCDUI_FillCircle_helper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta);
uint16_t	LCDUI_getcharwidth(char c);
uint8_t*	LCDUI_getchardata(char c);




void		LCDUI_Init();

void		LCDUI_SetColor(uint16_t color);
void		LCDUI_SetBackColor(uint16_t color);
void		LCDUI_SetCursorCoord(int16_t x, int16_t y);

uint16_t	LCDUI_GetScreenWidth();
uint16_t	LCDUI_GetScreenHeight();
uint16_t	LCDUI_GetCurrentCursorX();
uint16_t	LCDUI_GetCurrentCursorY();

void		LCDUI_Clear();
void		LCDUI_DrawPixel(uint16_t x1, uint16_t y1);
void		LCDUI_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r);
void		LCDUI_DrawFastVLine(int16_t x, int16_t y, int16_t h);
void		LCDUI_DrawFastHLine(int16_t x, int16_t y, int16_t w);
void		LCDUI_FillRect(int16_t x, int16_t y, int16_t w, int16_t h);
void		LCDUI_FillCircle(int16_t x0, int16_t y0, int16_t r);
void		LCDUI_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void		LCDUI_DrawRect(int16_t x, int16_t y, int16_t w, int16_t h);
void		LCDUI_DrawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r);
void		LCDUI_FillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r);
	
void		LCDUI_SetFont(LCDUI_FONT_TYPE newfont);
uint32_t	LCDUI_GetTextWidth(const char *str);
uint32_t	LCDUI_GetCurrentFontHeight();
void		LCDUI_DrawChar(char c, uint16_t opt = 0, int16_t x = -1, int16_t y = -1);
void		LCDUI_DrawText(char *str, uint16_t opt=0, int16_t x1=-1, int16_t y=-1, int16_t x2=-1, int16_t y2=-1);
void		LCDUI_DrawCharUTF(char *c, uint16_t opt = 0, int16_t x = -1, int16_t y = -1);
void		LCDUI_DrawTextUTF(char *str, uint16_t opt=0, int16_t x1=-1, int16_t y=-1, int16_t x2=-1, int16_t y2=-1);

void		LCDUI_DrawBitmap(uint16_t x, uint16_t y, uint8_t *bmp);



#endif