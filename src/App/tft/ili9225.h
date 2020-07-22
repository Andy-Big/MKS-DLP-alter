#ifndef LCDUI_H
#define LCDUI_H



// ************* settings ****************
//#define	TFT_ENABLE_PARSER
//#define		TFT_ENABLE_2B_FONTS
//#define	TFT_ENABLE_GRADIENT_BACKGROUND

// *********** end settings ***************





// ****************** fonts definitions *******************
#ifdef TFT_ENABLE_2B_FONTS
  #define	TFT_FONT_2B12			0
  #define	TFT_FONT_2B24SYMBOLS	1
  #define	TFT_FONT_2B62SPLASH		8
#endif
#define	TFT_FONT_1B12				10
#define	TFT_FONT_1B18				11
#define	TFT_FONT_1B62SPLASH			20
#define	TFT_FONT_1B22SYMBOLS		21
#define	TFT_FONT_1B22DIGITS			22






#define TFT_WIDTH	176
#define TFT_HEIGHT	220

#define ILI9225_DRIVER_OUTPUT_CTRL      (0x01u)  // Driver Output Control
#define ILI9225_LCD_AC_DRIVING_CTRL     (0x02u)  // LCD AC Driving Control
#define ILI9225_ENTRY_MODE            	(0x03u)  // Entry Mode
#define ILI9225_DISP_CTRL1          	(0x07u)  // Display Control 1
#define ILI9225_DISP_CTRL2				(0x08u)  // Blank Period Control
#define ILI9225_FRAME_CYCLE_CTRL        (0x0Bu)  // Frame Cycle Control
#define ILI9225_INTERFACE_CTRL          (0x0Cu)  // Interface Control
#define ILI9225_OSC_CTRL             	(0x0Fu)  // Osc Control
#define ILI9225_POWER_CTRL1            	(0x10u)  // Power Control 1
#define ILI9225_POWER_CTRL2           	(0x11u)  // Power Control 2
#define ILI9225_POWER_CTRL3            	(0x12u)  // Power Control 3
#define ILI9225_POWER_CTRL4            	(0x13u)  // Power Control 4
#define ILI9225_POWER_CTRL5            	(0x14u)  // Power Control 5
#define ILI9225_VCI_RECYCLING          	(0x15u)  // VCI Recycling
#define ILI9225_RAM_ADDR_SET1           (0x20u)  // Horizontal GRAM Address Set
#define ILI9225_RAM_ADDR_SET2           (0x21u)  // Vertical GRAM Address Set
#define ILI9225_GRAM_DATA_REG           (0x22u)  // GRAM Data Register
#define ILI9225_GATE_SCAN_CTRL          (0x30u)  // Gate Scan Control Register
#define ILI9225_VERTICAL_SCROLL_CTRL1   (0x31u)  // Vertical Scroll Control 1 Register
#define ILI9225_VERTICAL_SCROLL_CTRL2   (0x32u)  // Vertical Scroll Control 2 Register
#define ILI9225_VERTICAL_SCROLL_CTRL3   (0x33u)  // Vertical Scroll Control 3 Register
#define ILI9225_PARTIAL_DRIVING_POS1    (0x34u)  // Partial Driving Position 1 Register
#define ILI9225_PARTIAL_DRIVING_POS2    (0x35u)  // Partial Driving Position 2 Register
#define ILI9225_HORIZONTAL_WINDOW_ADDR1 (0x36u)  // Horizontal Address Start Position
#define ILI9225_HORIZONTAL_WINDOW_ADDR2	(0x37u)  // Horizontal Address End Position
#define ILI9225_VERTICAL_WINDOW_ADDR1   (0x38u)  // Vertical Address Start Position
#define ILI9225_VERTICAL_WINDOW_ADDR2   (0x39u)  // Vertical Address End Position
#define ILI9225_GAMMA_CTRL1            	(0x50u)  // Gamma Control 1
#define ILI9225_GAMMA_CTRL2             (0x51u)  // Gamma Control 2
#define ILI9225_GAMMA_CTRL3            	(0x52u)  // Gamma Control 3
#define ILI9225_GAMMA_CTRL4            	(0x53u)  // Gamma Control 4
#define ILI9225_GAMMA_CTRL5            	(0x54u)  // Gamma Control 5
#define ILI9225_GAMMA_CTRL6            	(0x55u)  // Gamma Control 6
#define ILI9225_GAMMA_CTRL7            	(0x56u)  // Gamma Control 7
#define ILI9225_GAMMA_CTRL8            	(0x57u)  // Gamma Control 8
#define ILI9225_GAMMA_CTRL9             (0x58u)  // Gamma Control 9
#define ILI9225_GAMMA_CTRL10            (0x59u)  // Gamma Control 10

/* RGB 24-bits color table definition (RGB888). */
#define TFT_RGB(color) ((((color) >> 19) & 0x1f) << 11) | ((((color) >> 10) & 0x3f) << 5) | (((color) >> 3) & 0x1f)
#define TFT_R_G_B(r, g, b) ((((r) >> 3) & 0x1f) << 11) | ((((g) >> 2) & 0x3f) << 5) | (((b) >> 3) & 0x1f)

#define COLOR_BLACK          TFT_RGB(0x000000u)
#define COLOR_WHITE          TFT_RGB(0xFFFFFFu)
#define COLOR_BLUE           TFT_RGB(0x0000FFu)
#define COLOR_GREEN          TFT_RGB(0x00FF00u)
#define COLOR_RED            TFT_RGB(0xFF0000u)
#define COLOR_NAVY           TFT_RGB(0x000080u)
#define COLOR_DARKBLUE       TFT_RGB(0x00008Bu)
#define COLOR_DARKGREEN      TFT_RGB(0x006400u)
#define COLOR_DARKCYAN       TFT_RGB(0x008B8Bu)
#define COLOR_CYAN           TFT_RGB(0x00FFFFu)
#define COLOR_TURQUOISE      TFT_RGB(0x40E0D0u)
#define COLOR_INDIGO         TFT_RGB(0x4B0082u)
#define COLOR_DARKRED        TFT_RGB(0x800000u)
#define COLOR_OLIVE          TFT_RGB(0x808000u)
#define COLOR_GRAY           TFT_RGB(0x808080u)
#define COLOR_SKYBLUE        TFT_RGB(0x87CEEBu)
#define COLOR_BLUEVIOLET     TFT_RGB(0x8A2BE2u)
#define COLOR_LIGHTGREEN     TFT_RGB(0x90EE90u)
#define COLOR_DARKVIOLET     TFT_RGB(0x9400D3u)
#define COLOR_YELLOWGREEN    TFT_RGB(0x9ACD32u)
#define COLOR_BROWN          TFT_RGB(0xA52A2Au)
#define COLOR_DARKGRAY       TFT_RGB(0xA9A9A9u)
#define COLOR_SIENNA         TFT_RGB(0xA0522Du)
#define COLOR_LIGHTBLUE      TFT_RGB(0xADD8E6u)
#define COLOR_GREENYELLOW    TFT_RGB(0xADFF2Fu)
#define COLOR_SILVER         TFT_RGB(0xC0C0C0u)
#define COLOR_LIGHTGREY      TFT_RGB(0xD3D3D3u)
#define COLOR_LIGHTCYAN      TFT_RGB(0xE0FFFFu)
#define COLOR_VIOLET         TFT_RGB(0xEE82EEu)
#define COLOR_AZUR           TFT_RGB(0xF0FFFFu)
#define COLOR_BEIGE          TFT_RGB(0xF5F5DCu)
#define COLOR_MAGENTA        TFT_RGB(0xFF00FFu)
#define COLOR_TOMATO         TFT_RGB(0xFF6347u)
#define COLOR_GOLD           TFT_RGB(0xFFD700u)
#define COLOR_ORANGE         TFT_RGB(0xFFA500u)
#define COLOR_SNOW           TFT_RGB(0xFFFAFAu)
#define COLOR_YELLOW         TFT_RGB(0xFFFF00u)



#define	TFT_ROTATION_0		(Int8U)0
#define	TFT_ROTATION_90		(Int8U)1
#define	TFT_ROTATION_180	(Int8U)2
#define	TFT_ROTATION_270	(Int8U)3

#define	TFT_TEXT_ALIGN_RIGHT	(Int16U)0x0001
#define	TFT_TEXT_ALIGN_CENTER	(Int16U)0x0002
#define	TFT_TEXT_UNDERLINE		(Int16U)0x0004
#define	TFT_TEXT_OVERLINE		(Int16U)0x0008



void	TFT_Init();
void	TFT_Reset();

void	TFT_write_reg(Int8U addr, Int16U data);
void	TFT_write_GRAM(Int16U data);
void	TFT_write_reg_beg(Int8U data);
void	TFT_write_data_cont(Int16U data);
void	TFT_write_data_last(Int16U data);
void	TFT_write_data_close();

void	TFT_set_window(Int16U x0, Int16U y0, Int16U x1, Int16U y1);
Int16U	TFT_text_getcharwidth(char c);
char*	TFT_text_getchardata(char c);
void	TFT_text1b_char(char c, Int16U opt=0, Int16S x=-1, Int16S y=-1);
#ifdef TFT_ENABLE_2B_FONTS
void	TFT_text2b_char(char c, Int16U opt=0, Int16S x=-1, Int16S y=-1);
#endif
void	TFT_draw_circle_helper(Int16S x0, Int16S y0, Int16S r, Int8U cornername);
void	TFT_fill_circle_helper(Int16S x0, Int16S y0, Int16S r, Int8U cornername, Int16S delta);



void	TFT_SetBacklight(Int8U bl);
void	TFT_SetColor(Int16U color);
void	TFT_SetBackColor(Int16U color);
#ifdef	TFT_ENABLE_GRADIENT_BACKGROUND
void	TFT_SetBackGradient(Int8U set);
#endif
void 	TFT_SetRotation(Int8U r, Int8U i);
void	TFT_SetCursorCoord(Int16S x, Int16S y);
void	TFT_SetFont(Int8U newfont);

void	TFT_Scroll1(Int16U x, Int16U y, Int16U z);

Int32U	TFT_GetTextWidth(const char *str);
Int32U	TFT_GetCurrentFontHeight();
Int16U	TFT_GetScreenWidth();
Int16U	TFT_GetScreenHeight();
Int16U	TFT_GetCurrentCursorX();
Int16U	TFT_GetCurrentCursorY();

void	TFT_Clear();
void	TFT_DrawPixel(Int16U x1, Int16U y1);
void	TFT_DrawCircle(Int16U x0, Int16U y0, Int16U r);
void	TFT_DrawFastVLine(Int16S x, Int16S y, Int16S h);
void	TFT_DrawFastHLine(Int16S x, Int16S y, Int16S w);
void	TFT_FillRect(Int16S x, Int16S y, Int16S w, Int16S h);
void	TFT_FillCircle(Int16S x0, Int16S y0, Int16S r);
void	TFT_DrawLine(Int16U x0, Int16U y0, Int16U x1, Int16U y1);
void	TFT_DrawRect(Int16S x, Int16S y, Int16S w, Int16S h);
void	TFT_DrawRoundRect(Int16S x, Int16S y, Int16S w, Int16S h, Int16S r);
void	TFT_FillRoundRect(Int16S x, Int16S y, Int16S w, Int16S h, Int16S r);

void	TFT_DrawText(const char *str, Int16U opt=0, Int16S x1=-1, Int16S y=-1, Int16S x2=-1, Int16S y2=-1);




#endif