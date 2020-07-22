#include "includes.h"
#include "ili9225.h"
#include "timer.h"


#include "tft_fonts.h"


// TFT SPI
//*****************************************
#define	TFTCS_PIN			GPIO_Pin_15
#define	TFTMOSI_PIN			GPIO_Pin_5
#define	TFTSCK_PIN			GPIO_Pin_3

#define	TFTCS_GPIO			GPIOA
#define	TFTSPI_GPIO			GPIOB

#define	TFTSPI				SPI1

#define	TFTSPI_RCC			RCC_APB2Periph_SPI1
#define	TFTSPIIO_RCC		RCC_AHBPeriph_GPIOB
#define	TFTCSIO_RCC			RCC_AHBPeriph_GPIOA
//*****************************************


// TFT Control
//*****************************************
#define	TFTRST_PIN			GPIO_Pin_12
#define	TFTRS_PIN			GPIO_Pin_11
#define	TFTLED_PIN			GPIO_Pin_9
#define	TFTLED_SRC			GPIO_PinSource9

#define	TFTRST_GPIO			GPIOA
#define	TFTRS_GPIO			GPIOA
#define	TFTLED_GPIO			GPIOA

#define	TFTRSTIO_RCC		RCC_AHBPeriph_GPIOA
#define	TFTRSIO_RCC			RCC_AHBPeriph_GPIOA
#define	TFTLEDIO_RCC		RCC_AHBPeriph_GPIOA
//*****************************************




#define TFT_CS_ON()			GPIO_WriteBit(TFTCS_GPIO, TFTCS_PIN, Bit_RESET)
#define TFT_CS_OFF()		GPIO_WriteBit(TFTCS_GPIO, TFTCS_PIN, Bit_SET)
#define TFT_RST_ON()		GPIO_WriteBit(TFTRST_GPIO, TFTRST_PIN, Bit_RESET)
#define TFT_RST_OFF()		GPIO_WriteBit(TFTRST_GPIO, TFTRST_PIN, Bit_SET)
#define TFT_RS_HIGH()		GPIO_WriteBit(TFTRS_GPIO, TFTRS_PIN, Bit_SET)
#define TFT_RS_LOW()		GPIO_WriteBit(TFTRS_GPIO, TFTRS_PIN, Bit_RESET)

#define SPI1_DR_Address    ((uint32_t)0x4001300C)



Int8U 		tft_startH, tft_endH, tft_startV, tft_endV, tft_ramAddrOne, tft_ramAddrTwo;
Int16S		tft_width, tft_height, tft_cursor_x, tft_cursor_y; // Display w/h as modified by current rotation
TFT_FONT	*tft_current_font;
Int16U		tft_bgcolor = TFT_R_G_B(0x00, 0x00, 0x00), tft_color = TFT_R_G_B(0xFF, 0xFF, 0xFF);
Int8U		tft_bggradient = 0;

//extern TFT_FONT_2B font_2b16h;
#ifdef TFT_ENABLE_2B_FONTS
  extern TFT_FONT	font_2b12h;
  extern TFT_FONT	font_2b24h_symbols;
  extern TFT_FONT	font_2b62h_splash;
#endif
//extern TFT_FONT_2B font_2b62h_splash;
extern TFT_FONT		font_1b12h;
extern TFT_FONT		font_1b18h;
extern TFT_FONT		font_1b62h_splash;
extern TFT_FONT		font_1b22h_digits;
extern TFT_FONT		font_1b22h_symbols;

//extern const Int16U gradback1[];


void	TFT_Init()
{
	SPI_InitTypeDef				SPI_InitStructure;
	SPI_StructInit(&SPI_InitStructure);
	GPIO_InitTypeDef 			GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_OCInitTypeDef			TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);


	// GPIO Init
	// Enable GPIO clock and release reset
	RCC_AHBPeriphClockCmd(TFTSPIIO_RCC, ENABLE);
	RCC_AHBPeriphResetCmd(TFTSPIIO_RCC, DISABLE);
	RCC_AHBPeriphClockCmd(TFTCSIO_RCC, ENABLE);
	RCC_AHBPeriphResetCmd(TFTCSIO_RCC, DISABLE);
	RCC_AHBPeriphClockCmd(TFTRSTIO_RCC, ENABLE);
	RCC_AHBPeriphResetCmd(TFTRSTIO_RCC, DISABLE);
	RCC_AHBPeriphClockCmd(TFTRSIO_RCC, ENABLE);
	RCC_AHBPeriphResetCmd(TFTRSIO_RCC, DISABLE);
	RCC_AHBPeriphClockCmd(TFTLEDIO_RCC, ENABLE);
	RCC_AHBPeriphResetCmd(TFTLEDIO_RCC, DISABLE);

	// (MOSI)
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Pin = TFTMOSI_PIN;
	GPIO_Init(TFTSPI_GPIO, &GPIO_InitStructure);

	// (SCK)
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = TFTSCK_PIN;
	GPIO_Init(TFTSPI_GPIO, &GPIO_InitStructure);

	// (LED)
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_2);

	// (CS)
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin = TFTCS_PIN;
	GPIO_Init(TFTCS_GPIO, &GPIO_InitStructure);
	TFT_CS_OFF();

	// (RST)
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = TFTRST_PIN;
	GPIO_Init(TFTRST_GPIO, &GPIO_InitStructure);
	TFT_RST_OFF();

	// (RS)
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = TFTRS_PIN;
	GPIO_Init(TFTRS_GPIO, &GPIO_InitStructure);
	TFT_RS_HIGH();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	TIM_DeInit(TIM1);

	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 10000;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	// G-led
	TIM_OCInitStructure.TIM_Pulse = 5000;
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);

	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	TIM_SelectOnePulseMode(TIM1, TIM_OPMode_Repetitive);

	TIM_BDTRInitTypeDef TIM_BDTRstruct;
	TIM_BDTRStructInit(&TIM_BDTRstruct);
	TIM_BDTRstruct.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
	TIM_BDTRConfig(TIM1, &TIM_BDTRstruct);

	TIM_Cmd(TIM1, ENABLE);

	/* TIM1 Main Output Enable */
	TIM_CtrlPWMOutputs(TIM1, ENABLE);


	// Release reset and enable clock
	SPI_I2S_DeInit(TFTSPI);
	RCC_APB2PeriphClockCmd(TFTSPI_RCC, ENABLE);

	SPI_InitStructure. SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(TFTSPI, &SPI_InitStructure);
	// Init SPI2
	SPI_Init(TFTSPI, &SPI_InitStructure);

	// Enable the SPI2
	SPI_CalculateCRC(TFTSPI, DISABLE);
	SPI_Cmd(TFTSPI, DISABLE);

}
//==============================================================================




void	TFT_Reset()
{
	tft_cursor_x = 0;
	tft_cursor_y = 0;
#ifdef TFT_ENABLE_2B_FONTS
	tft_current_font = &font_2b12h;
#else
	tft_current_font = &font_1b12h;
#endif
	tft_bgcolor = TFT_RGB(0x000000);
	tft_color = TFT_RGB(0xFFFFFF);

	// ************* Reset LCD Driver ****************
	Int8U	tmr = TIMER_NewCountDown(2);
	TFT_RST_OFF();
	while(TIMER_GetCountDown(tmr));

	TIMER_SetCountDown(tmr, 10);
	TFT_RST_ON();
	while(TIMER_GetCountDown(tmr));

	TIMER_SetCountDown(tmr, 50);
	TFT_RST_OFF();
	while(TIMER_GetCountDown(tmr));




/*
	// Power-on sequence
	TFT_write_reg(ILI9225_POWER_CTRL1,			0x0000); // Set SAP,DSTB,STB
	TFT_write_reg(ILI9225_POWER_CTRL2, 			0x0000); // Set APON,PON,AON,VCI1EN,VC
	TFT_write_reg(ILI9225_POWER_CTRL3, 			0x0000); // Set BT,DC1,DC2,DC3
	TFT_write_reg(ILI9225_POWER_CTRL4, 			0x0000); // Set GVDD
	TFT_write_reg(ILI9225_POWER_CTRL5, 			0x0000); // Set VCOMH/VCOML voltage

	TIMER_SetCountDown(tmr, 40);
	while(TIMER_GetCountDown(tmr));
	// Power-on sequence
	TFT_write_reg(ILI9225_POWER_CTRL2, 			0x0018); // Set APON,PON,AON,VCI1EN,VC
	TFT_write_reg(ILI9225_POWER_CTRL3, 			0x6121); // Set BT,DC1,DC2,DC3
	TFT_write_reg(ILI9225_POWER_CTRL4, 			0x006F); // Set GVDD   007F 0088
	TFT_write_reg(ILI9225_POWER_CTRL5, 			0x495F); // Set VCOMH/VCOML voltage
	TFT_write_reg(ILI9225_POWER_CTRL1, 			0x0F00); // Set SAP,DSTB,STB

	TIMER_SetCountDown(tmr, 10);
	while(TIMER_GetCountDown(tmr));

	TFT_write_reg(ILI9225_POWER_CTRL2, 			0x103B); // Set APON,PON,AON,VCI1EN,VC

	TIMER_SetCountDown(tmr, 50);
	while(TIMER_GetCountDown(tmr));

	TFT_write_reg(ILI9225_DRIVER_OUTPUT_CTRL, 	0x011C); // set the display line number and display direction
	TFT_write_reg(ILI9225_LCD_AC_DRIVING_CTRL, 	0x0100); // set 1 line inversion
	TFT_write_reg(ILI9225_ENTRY_MODE, 			0x01030); // set GRAM write direction and BGR=1.
	TFT_write_reg(ILI9225_DISP_CTRL1, 			0x0000); // Display off
	TFT_write_reg(ILI9225_DISP_CTRL2, 			0x0808); // set the back porch and front porch
	TFT_write_reg(ILI9225_FRAME_CYCLE_CTRL, 	0x1100); // set the clocks number per line
	TFT_write_reg(ILI9225_INTERFACE_CTRL, 		0x0000); // CPU interface
	TFT_write_reg(ILI9225_OSC_CTRL, 			0x0D01); // Set Osc  0e01
	TFT_write_reg(ILI9225_VCI_RECYCLING, 		0x0020); // Set VCI recycling
	TFT_write_reg(ILI9225_RAM_ADDR_SET1, 		0x0000); // RAM Address
	TFT_write_reg(ILI9225_RAM_ADDR_SET2, 		0x0000); // RAM Address
	// Set GRAM area
	TFT_write_reg(ILI9225_GATE_SCAN_CTRL, 		0x0000);
	TFT_write_reg(ILI9225_VERTICAL_SCROLL_CTRL1, 0x00DB);
	TFT_write_reg(ILI9225_VERTICAL_SCROLL_CTRL2, 0x0000);
	TFT_write_reg(ILI9225_VERTICAL_SCROLL_CTRL3, 0x0000);
	TFT_write_reg(ILI9225_PARTIAL_DRIVING_POS1, 0x00DB);
	TFT_write_reg(ILI9225_PARTIAL_DRIVING_POS2, 0x0000);
	TFT_write_reg(ILI9225_HORIZONTAL_WINDOW_ADDR1, 0x00AF);
	TFT_write_reg(ILI9225_HORIZONTAL_WINDOW_ADDR2, 0x0000);
	TFT_write_reg(ILI9225_VERTICAL_WINDOW_ADDR1, 0x00DB);
	TFT_write_reg(ILI9225_VERTICAL_WINDOW_ADDR2, 0x0000);
	// Set GAMMA curve
	TFT_write_reg(ILI9225_GAMMA_CTRL1, 			0x0000);
	TFT_write_reg(ILI9225_GAMMA_CTRL2, 			0x0808);
	TFT_write_reg(ILI9225_GAMMA_CTRL3, 			0x080A);
	TFT_write_reg(ILI9225_GAMMA_CTRL4, 			0x000A);
	TFT_write_reg(ILI9225_GAMMA_CTRL5, 			0x0A08);
	TFT_write_reg(ILI9225_GAMMA_CTRL6, 			0x0808);
	TFT_write_reg(ILI9225_GAMMA_CTRL7, 			0x0000);
	TFT_write_reg(ILI9225_GAMMA_CTRL8, 			0x0A00);
	TFT_write_reg(ILI9225_GAMMA_CTRL9, 			0x0710);
	TFT_write_reg(ILI9225_GAMMA_CTRL10,			0x0710);
	TFT_write_reg(ILI9225_DISP_CTRL1, 			0x0012);
	TIMER_SetCountDown(tmr, 50);
	while(TIMER_GetCountDown(tmr));
	TFT_write_reg(ILI9225_DISP_CTRL1, 			0x1017);

*/

	// ************* Start Initial Sequence **********
	TFT_write_reg(ILI9225_DRIVER_OUTPUT_CTRL, 	0x011C); // set SS and NL bit
	TFT_write_reg(ILI9225_LCD_AC_DRIVING_CTRL, 	0x0100); // set 1 line inversion
	TFT_write_reg(ILI9225_ENTRY_MODE, 			0x1030); // set GRAM write direction and BGR=1
	TFT_write_reg(ILI9225_DISP_CTRL1, 			0x0000);
	TFT_write_reg(ILI9225_DISP_CTRL2, 			0x0808); // set BP and FP
	TFT_write_reg(ILI9225_FRAME_CYCLE_CTRL, 	0x1100); // set the clocks number per line
	TFT_write_reg(ILI9225_INTERFACE_CTRL, 		0x0001); // RGB interface setting
	TFT_write_reg(ILI9225_OSC_CTRL, 			0x0C01); // Set frame rate
	TFT_write_reg(ILI9225_RAM_ADDR_SET1, 		0x0000); // Set GRAM Address
	TFT_write_reg(ILI9225_RAM_ADDR_SET2, 		0x0000); // Set GRAM Address

	// *************Power On sequence ****************
	TIMER_SetCountDown(tmr, 50);
	while(TIMER_GetCountDown(tmr));
	TFT_write_reg(ILI9225_POWER_CTRL1, 			0x0A00); // Set SAP,DSTB,STB
	TFT_write_reg(ILI9225_POWER_CTRL2, 			0x1038); // Set APON,PON,AON,VCI1EN,VC
	TIMER_SetCountDown(tmr, 50);
	while(TIMER_GetCountDown(tmr));
	TFT_write_reg(ILI9225_POWER_CTRL3, 			0x1121); // Internal reference voltage= Vci;
	TFT_write_reg(ILI9225_POWER_CTRL4, 			0x0088); // Set GVDD
	TFT_write_reg(ILI9225_POWER_CTRL5, 			0x5F60); // Set VCOMH/VCOML voltage
	//------------------------ Set GRAM area --------------------------------//
	TFT_write_reg(ILI9225_GATE_SCAN_CTRL, 		0x0000);
	TFT_write_reg(ILI9225_VERTICAL_SCROLL_CTRL1, 0x00DB);
	TFT_write_reg(ILI9225_VERTICAL_SCROLL_CTRL2, 0x0000);
	TFT_write_reg(ILI9225_VERTICAL_SCROLL_CTRL3, 0x0000);
	TFT_write_reg(ILI9225_PARTIAL_DRIVING_POS1, 	0x00DB);
	TFT_write_reg(ILI9225_PARTIAL_DRIVING_POS2, 	0x0000);
	TFT_write_reg(ILI9225_HORIZONTAL_WINDOW_ADDR1, 0x00AF);
	TFT_write_reg(ILI9225_HORIZONTAL_WINDOW_ADDR2, 0x0000);
	TFT_write_reg(ILI9225_VERTICAL_WINDOW_ADDR1, 0x00DB);
	TFT_write_reg(ILI9225_VERTICAL_WINDOW_ADDR2, 0x0000);
	// ----------- Adjust the Gamma Curve ----------//
	TFT_write_reg(ILI9225_GAMMA_CTRL1, 			0x0400);
	TFT_write_reg(ILI9225_GAMMA_CTRL2, 			0x060B);
	TFT_write_reg(ILI9225_GAMMA_CTRL3, 			0x0C0A);
	TFT_write_reg(ILI9225_GAMMA_CTRL4, 			0x0105);
	TFT_write_reg(ILI9225_GAMMA_CTRL5, 			0x0A0C);
	TFT_write_reg(ILI9225_GAMMA_CTRL6, 			0x0B06);
	TFT_write_reg(ILI9225_GAMMA_CTRL7, 			0x0004);
	TFT_write_reg(ILI9225_GAMMA_CTRL8, 			0x0500);
	TFT_write_reg(ILI9225_GAMMA_CTRL9, 			0x0E00);
	TFT_write_reg(ILI9225_GAMMA_CTRL10, 		0x000E);
	TIMER_SetCountDown(tmr, 50);
	while(TIMER_GetCountDown(tmr));
	TFT_write_reg(ILI9225_DISP_CTRL1, 			0x1017);
	TIMER_DeleteCountDown(tmr);
}
//==============================================================================





inline void	TFT_write_reg(Int8U addr, Int16U data)
{
	Int16U treg = TFTSPI->CR2 & (~SPI_DataSize_16b);
	TFTSPI->CR2 = treg;
	SPI_Cmd(TFTSPI, ENABLE);
	TFT_RS_LOW();
	TFT_CS_ON();
	// address

	*(Int8U*)&TFTSPI->DR = addr;
	while ((TFTSPI->SR & SPI_I2S_FLAG_TXE) == 0);
	while ((TFTSPI->SR & SPI_I2S_FLAG_BSY) > 0);
	// data
	SPI_Cmd(TFTSPI, DISABLE);
	treg = TFTSPI->CR2 | SPI_DataSize_16b;
	TFTSPI->CR2 = treg;
	SPI_Cmd(TFTSPI, ENABLE);
	TFT_RS_HIGH();
	TFTSPI->DR = data;
	while ((TFTSPI->SR & SPI_I2S_FLAG_TXE) == 0);
	while ((TFTSPI->SR & SPI_I2S_FLAG_BSY) > 0);
	TFT_CS_OFF();
	SPI_Cmd(TFTSPI, DISABLE);
}
//==============================================================================





void	TFT_write_GRAM(Int16U data)
{
	Int16U treg = TFTSPI->CR2 | SPI_DataSize_16b;
	TFTSPI->CR2 = treg;
	SPI_Cmd(TFTSPI, ENABLE);
	TFT_RS_HIGH();
	TFT_CS_ON();
	TFTSPI->DR = data;
	while ((TFTSPI->SR & SPI_I2S_FLAG_TXE) == 0);
	while ((TFTSPI->SR & SPI_I2S_FLAG_BSY) > 0);
	TFT_CS_OFF();
	SPI_Cmd(TFTSPI, DISABLE);
}
//==============================================================================





inline void	TFT_write_reg_beg(Int8U data)
{
	Int16U treg = TFTSPI->CR2 & (~SPI_DataSize_16b);
	TFTSPI->CR2 = treg;
	SPI_Cmd(TFTSPI, ENABLE);
	TFT_RS_LOW();
	TFT_CS_ON();
	// address
	*(Int8U*)&TFTSPI->DR = data;
	while ((TFTSPI->SR & SPI_I2S_FLAG_TXE) == 0);
	while ((TFTSPI->SR & SPI_I2S_FLAG_BSY) > 0);
	SPI_Cmd(TFTSPI, DISABLE);
	treg = TFTSPI->CR2 | SPI_DataSize_16b;
	TFTSPI->CR2 = treg;
	SPI_Cmd(TFTSPI, ENABLE);
	TFT_RS_HIGH();
}
//==============================================================================





inline void	TFT_write_data_cont(Int16U data)
{
	TFTSPI->DR = data;
	while (!(TFTSPI->SR & SPI_I2S_FLAG_TXE));
}
//==============================================================================





inline void	TFT_write_data_last(Int16U data)
{
	TFTSPI->DR = data;
	while ((TFTSPI->SR & SPI_I2S_FLAG_TXE) == 0);
	while ((TFTSPI->SR & SPI_I2S_FLAG_BSY) > 0);
	TFT_CS_OFF();
	SPI_Cmd(TFTSPI, DISABLE);
}
//==============================================================================





inline void	TFT_write_data_close()
{
	while ((TFTSPI->SR & SPI_I2S_FLAG_BSY) > 0);
	TFT_CS_OFF();
	SPI_Cmd(TFTSPI, DISABLE);
}
//==============================================================================





void	TFT_set_window(Int16U x0, Int16U y0, Int16U x1, Int16U y1)
{
	TFT_write_reg(tft_endH, x1);
	TFT_write_reg(tft_startH, x0);
	TFT_write_reg(tft_endV, y1);
	TFT_write_reg(tft_startV, y0);
	TFT_write_reg(tft_ramAddrOne, x0);
	TFT_write_reg(tft_ramAddrTwo, y0);
}
//==============================================================================





Int16U	TFT_text_getcharwidth(char c)
{
	if (c < 32)
		return 0;
	if (c > 126)
		c -= 65;
	c -= 32;
	if (c >= tft_current_font->symcount)
		return 0;
	Int16U res = tft_current_font->width[c];
	if (res & 0x8000)
		return tft_current_font->width[(res & 0x7FFF)];
	return res;
}
//==============================================================================





char*	TFT_text_getchardata(char c)
{
	if (c < 32)
		return 0;
	if (c > 126)
		c -= 65;
	c -= 32;
	if (c >= tft_current_font->symcount)
		return 0;
	Int16U c1 = tft_current_font->width[c];
	if (c1 & 0x8000)
		c = (c1 & 0x7FFF);
	Int16U ch = tft_current_font->height;
	Int32S i = 0, ptr = 0, bits = 0, line_bits = ch;
#ifdef TFT_ENABLE_2B_FONTS
	if (tft_current_font->type == TFT_FONTTYPE_2BITS)
		line_bits *= 2;
#endif
	for (i = 0; i < c; i++)
	{
		if (tft_current_font->width[i] & 0x8000)
			continue;
		bits = tft_current_font->width[i] * line_bits;
		ptr += bits >> 3;
		if (bits & 0x07)
			ptr++;
	}

	return &(tft_current_font->data[ptr]);
}
//==============================================================================




void	TFT_text1b_char(char c,  Int16U opt, Int16S x, Int16S y)
{
	if((x > tft_width) || (y > tft_height) || c < 32)
	{
		return;
	}
	if (x >= 0)
		tft_cursor_x = x;
	if (y >= 0)
		tft_cursor_y = y;
	if((tft_cursor_x > tft_width) || (tft_cursor_y > tft_height))
		return;

	Int16U	cres[2];
	if (tft_current_font->alpha[0])
	{
		cres[0] = tft_color;
		cres[1] = tft_bgcolor;
	}
	else
	{
		cres[1] = tft_color;
		cres[0] = tft_bgcolor;
	}
	Int16U cw = 0, ch = tft_current_font->height;
	Int16U i = 0, ptr = 0, k = 0, y1 = 0;
	cw = TFT_text_getcharwidth(c);
	if (cw == 0)
		return;
	char *data = TFT_text_getchardata(c);
	if (data == 0)
		return;

	c = data[ptr++];
	Int8U sh = 0, csh = 0;
	for (i = 0; i < ch; i++)
	{
		y1 = tft_cursor_y+i;
		TFT_write_reg_beg(tft_ramAddrOne);
		TFT_write_data_last(tft_cursor_x);
		TFT_write_reg_beg(tft_ramAddrTwo);
		TFT_write_data_last(y1);
		TFT_write_reg_beg(ILI9225_GRAM_DATA_REG);

		for (k = 0; k < cw; k++)
		{
			csh = (c >> sh);
			if (tft_cursor_x+k < tft_width)
			{
				TFT_write_data_cont(cres[(csh&0x01)]);
			}
			sh ++;
			if (sh == 8)
			{
				c = data[ptr++];
				sh = 0;
/*
				while (c == 0)
				{
				 	ce = 1;
					k += 4;
					if (k >= cw)
					{
						k -= cw;
						i++;
						if (i == ch)
						{
							k = cw;
							break;
						}
					}
					c = tft_current_font->data[ptr++];
				}
				if (ce)
				{
					ce = 0;
					TFT_write_data_close();
					if (k < cw && i < ch)
					{
						y1 = tft_cursor_y+i;
						TFT_write_reg_beg(tft_ramAddrOne);
						TFT_write_data_last(tft_cursor_x+k);
						TFT_write_reg_beg(tft_ramAddrTwo);
						TFT_write_data_last(y1);
						TFT_write_reg_beg(ILI9225_GRAM_DATA_REG);
					}
				}
*/
			}
		}
		TFT_write_data_close();
	}
	if (opt & TFT_TEXT_UNDERLINE)
	{
		TFT_write_reg_beg(tft_ramAddrOne);
		TFT_write_data_last(tft_cursor_x);
		TFT_write_reg_beg(tft_ramAddrTwo);
		TFT_write_data_last(tft_cursor_y+ch-1);
		TFT_write_reg_beg(ILI9225_GRAM_DATA_REG);
		for (k = 0; k < cw; k++)
		{
			TFT_write_data_cont(tft_color);
		}
		TFT_write_data_close();
	}

	if (opt & TFT_TEXT_OVERLINE)
	{
		TFT_write_reg_beg(tft_ramAddrOne);
		TFT_write_data_last(tft_cursor_x);
		TFT_write_reg_beg(tft_ramAddrTwo);
		TFT_write_data_last(tft_cursor_y+ch/2);
		TFT_write_reg_beg(ILI9225_GRAM_DATA_REG);
		for (k = 0; k < cw; k++)
		{
			TFT_write_data_cont(tft_color);
		}
		TFT_write_data_close();
	}

	tft_cursor_x += cw;
}
//==============================================================================





#ifdef TFT_FONTTYPE_2BITS
void	TFT_text2b_char(char c,  Int16U opt, Int16S x, Int16S y)
{
	if((x > tft_width) || (y > tft_height) || c < 32)
	{
		return;
	}
	if (x >= 0)
		tft_cursor_x = x;
	if (y >= 0)
		tft_cursor_y = y;
	if((tft_cursor_x > tft_width) || (tft_cursor_y > tft_height))
		return;

	Int8U	Rs = (tft_color >> 11), Gs = ((tft_color >> 5) & 0x3F), Bs = (tft_color & 0x1F);
	Int8U	Rd = (tft_bgcolor >> 11), Gd = ((tft_bgcolor >> 5) & 0x3F), Bd = (tft_bgcolor & 0x1F);
	Int16U	cres[4];
	for (Int8U i = 0; i < 4; i++)
		cres[i] = 	(((Rs*tft_current_font->alpha[i]+Rd*(3-tft_current_font->alpha[i]))/3) << 11) |
					(((Gs*tft_current_font->alpha[i]+Gd*(3-tft_current_font->alpha[i]))/3) << 5) |
					((Bs*tft_current_font->alpha[i]+Bd*(3-tft_current_font->alpha[i]))/3);



	Int16U cw = 0, ch = tft_current_font->height;
	Int16U i = 0, ptr = 0, k = 0, y1 = 0;
	cw = TFT_text_getcharwidth(c);
	if (cw == 0)
		return;
	char *data = TFT_text_getchardata(c);
	if (data == 0)
		return;

	c = data[ptr++];
	Int8U sh = 0, csh = 0;
	for (i = 0; i < ch; i++)
	{
		y1 = tft_cursor_y+i;
/*
		if (tft_bggradient)
		{
			Int8U	Rd = (gradback1[y1] >> 11), Gd = ((gradback1[y1] >> 5) & 0x3F), Bd = (gradback1[y1] & 0x1F);
			for (Int8U i = 0; i < 4; i++)
				cres[i] = 	(((Rs*tft_current_font->alpha[i]+Rd*(3-tft_current_font->alpha[i]))/3) << 11) |
							(((Gs*tft_current_font->alpha[i]+Gd*(3-tft_current_font->alpha[i]))/3) << 5) |
							((Bs*tft_current_font->alpha[i]+Bd*(3-tft_current_font->alpha[i]))/3);
		}
*/
		TFT_write_reg_beg(tft_ramAddrOne);
		TFT_write_data_last(tft_cursor_x);
		TFT_write_reg_beg(tft_ramAddrTwo);
		TFT_write_data_last(y1);
		TFT_write_reg_beg(ILI9225_GRAM_DATA_REG);

		for (k = 0; k < cw; k++)
		{
			csh = (c >> sh) & 0x03;
			if (tft_cursor_x+k < tft_width)
				TFT_write_data_cont(cres[csh]);
			sh += 2;
			if (sh == 8)
			{
				c = data[ptr++];
				sh = 0;
/*
				while (c == 0)
				{
				 	ce = 1;
					k += 4;
					if (k >= cw)
					{
						k -= cw;
						i++;
						if (i == ch)
						{
							k = cw;
							break;
						}
					}
					c = tft_current_font->data[ptr++];
				}
				if (ce)
				{
					ce = 0;
					TFT_write_data_close();
					if (k < cw && i < ch)
					{
						y1 = tft_cursor_y+i;
						TFT_write_reg_beg(tft_ramAddrOne);
						TFT_write_data_last(tft_cursor_x+k);
						TFT_write_reg_beg(tft_ramAddrTwo);
						TFT_write_data_last(y1);
						TFT_write_reg_beg(ILI9225_GRAM_DATA_REG);
					}
				}
*/
			}
		}
		TFT_write_data_close();
	}
	if (opt & TFT_TEXT_UNDERLINE)
	{
		TFT_write_reg_beg(tft_ramAddrOne);
		TFT_write_data_last(tft_cursor_x);
		TFT_write_reg_beg(tft_ramAddrTwo);
		TFT_write_data_last(tft_cursor_y+ch-1);
		TFT_write_reg_beg(ILI9225_GRAM_DATA_REG);
		for (k = 0; k < cw; k++)
		{
			TFT_write_data_cont(tft_color);
		}
		TFT_write_data_close();
	}

	if (opt & TFT_TEXT_OVERLINE)
	{
		TFT_write_reg_beg(tft_ramAddrOne);
		TFT_write_data_last(tft_cursor_x);
		TFT_write_reg_beg(tft_ramAddrTwo);
		TFT_write_data_last(tft_cursor_y+ch/2);
		TFT_write_reg_beg(ILI9225_GRAM_DATA_REG);
		for (k = 0; k < cw; k++)
		{
			TFT_write_data_cont(tft_color);
		}
		TFT_write_data_close();
	}

	tft_cursor_x += cw;
}
//==============================================================================
#endif




void	TFT_draw_circle_helper(Int16S x0, Int16S y0, Int16S r, Int8U cornername)
{
	Int16S	f     = r;
	Int16S	ddF_x = 1;
	Int16S	ddF_y = -2 * r;
	Int16S	x     = 0;
	Int16S	y     = r;

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
			TFT_DrawPixel(x0 + x, y0 + y);
			TFT_DrawPixel(x0 + y, y0 + x);
		}
		if (cornername & 0x2)
		{
			TFT_DrawPixel(x0 + x, y0 - y);
			TFT_DrawPixel(x0 + y, y0 - x);
		}
		if (cornername & 0x8)
		{
			TFT_DrawPixel(x0 - y, y0 + x);
			TFT_DrawPixel(x0 - x, y0 + y);
		}
		if (cornername & 0x1)
		{
			TFT_DrawPixel(x0 - y, y0 - x);
			TFT_DrawPixel(x0 - x, y0 - y);
		}
	}
}
//==============================================================================





void	TFT_fill_circle_helper(Int16S x0, Int16S y0, Int16S r, Int8U cornername, Int16S delta)
{
	Int16S 	f     = r;
	Int16S 	ddF_x = 1;
	Int16S 	ddF_y = -2 * r;
	Int16S 	x     = 0;
	Int16S 	y     = r;

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
			TFT_DrawFastVLine(x0+x, y0-y, 2*y+1+delta);
			TFT_DrawFastVLine(x0+y, y0-x, 2*x+1+delta);
		}
		if (cornername & 0x2)
		{
			TFT_DrawFastVLine(x0-x, y0-y, 2*y+1+delta);
			TFT_DrawFastVLine(x0-y, y0-x, 2*x+1+delta);
		}
	}
}
//==============================================================================





void	TFT_SetBacklight(Int8U bl)
{
	if (bl > 100)
		bl = 100;
	TIM_SetCompare2(TIM1, bl*bl);
}
//==============================================================================





void	TFT_SetColor(Int16U color)
{
	tft_color = color;
}
//==============================================================================





void	TFT_SetBackColor(Int16U color)
{
	tft_bgcolor = color;
}
//==============================================================================





void	TFT_SetBackGradient(Int8U set)
{
	tft_bggradient = set;
}
//==============================================================================





void	TFT_SetRotation(Int8U r, Int8U i)
{
	Int8U rotation = r % 4; // can't be higher than 3
	i = i % 2; // can't be higher than 1
	TFT_write_reg_beg(ILI9225_DRIVER_OUTPUT_CTRL);

	if(!rotation)
	{
		TFT_write_data_last(i ? 0x001C:0x011C);
		TFT_write_reg_beg(ILI9225_ENTRY_MODE);
		TFT_write_data_last(0x1030);
		tft_startH = ILI9225_HORIZONTAL_WINDOW_ADDR2;
		tft_endH = ILI9225_HORIZONTAL_WINDOW_ADDR1;
		tft_startV = ILI9225_VERTICAL_WINDOW_ADDR2;
		tft_endV = ILI9225_VERTICAL_WINDOW_ADDR1;
		tft_ramAddrOne = ILI9225_RAM_ADDR_SET1;
		tft_ramAddrTwo = ILI9225_RAM_ADDR_SET2;
		tft_width = TFT_WIDTH;
		tft_height = TFT_HEIGHT;
	}
	else if(rotation == 1)
	{
		TFT_write_data_last(i ? 0x021C:0x001C);
		TFT_write_reg_beg(ILI9225_ENTRY_MODE);
		TFT_write_data_last(0x1038);
		tft_startH = ILI9225_VERTICAL_WINDOW_ADDR2;
		tft_endH = ILI9225_VERTICAL_WINDOW_ADDR1;
		tft_startV = ILI9225_HORIZONTAL_WINDOW_ADDR2;
		tft_endV = ILI9225_HORIZONTAL_WINDOW_ADDR1;
		tft_ramAddrOne = ILI9225_RAM_ADDR_SET2;
		tft_ramAddrTwo = ILI9225_RAM_ADDR_SET1;
		tft_width = TFT_HEIGHT;
		tft_height = TFT_WIDTH;
	}
	else if(rotation == 2)
	{
		TFT_write_data_last(i ? 0x031C:0x021C);
		TFT_write_reg_beg(ILI9225_ENTRY_MODE);
		TFT_write_data_last(0x1030);
		tft_startH = ILI9225_HORIZONTAL_WINDOW_ADDR2;
		tft_endH = ILI9225_HORIZONTAL_WINDOW_ADDR1;
		tft_startV = ILI9225_VERTICAL_WINDOW_ADDR2;
		tft_endV = ILI9225_VERTICAL_WINDOW_ADDR1;
		tft_ramAddrOne = ILI9225_RAM_ADDR_SET1;
		tft_ramAddrTwo = ILI9225_RAM_ADDR_SET2;
		tft_width = TFT_WIDTH;
		tft_height = TFT_HEIGHT;
	}
	else if(rotation == 3)
	{
		TFT_write_data_last(i ? 0x011C:0x031C);
		TFT_write_reg_beg(ILI9225_ENTRY_MODE);
		TFT_write_data_last(0x1038);
		tft_startH = ILI9225_VERTICAL_WINDOW_ADDR2;
		tft_endH = ILI9225_VERTICAL_WINDOW_ADDR1;
		tft_startV = ILI9225_HORIZONTAL_WINDOW_ADDR2;
		tft_endV = ILI9225_HORIZONTAL_WINDOW_ADDR1;
		tft_ramAddrOne = ILI9225_RAM_ADDR_SET2;
		tft_ramAddrTwo = ILI9225_RAM_ADDR_SET1;
		tft_width = TFT_HEIGHT;
		tft_height = TFT_WIDTH;
	}
}
//==============================================================================





void	TFT_SetCursorCoord(Int16S x, Int16S y)
{
	tft_cursor_x = x;
	tft_cursor_y = y;
}
//==============================================================================





void	TFT_SetFont(Int8U newfont)
{

	switch (newfont)
	{
#ifdef TFT_ENABLE_2B_FONTS
		case TFT_FONT_2B12:
			tft_current_font = &font_2b12h;
			break;

		case TFT_FONT_2B24SYMBOLS:
			tft_current_font = &font_2b24h_symbols;
			break;

		case TFT_FONT_2B62SPLASH:
			tft_current_font = &font_2b62h_splash;
			break;
#endif

		case TFT_FONT_1B12:
			tft_current_font = &font_1b12h;
			break;

		case TFT_FONT_1B18:
			tft_current_font = &font_1b18h;
			break;

		case TFT_FONT_1B22DIGITS:
			tft_current_font = &font_1b22h_digits;
			break;

		case TFT_FONT_1B62SPLASH:
			tft_current_font = &font_1b62h_splash;
			break;

		case TFT_FONT_1B22SYMBOLS:
			tft_current_font = &font_1b22h_symbols;
			break;

	}
}
//==============================================================================





void	TFT_Scroll1(Int16U x, Int16U y, Int16U z)
{
//ILI9225_PARTIAL_DRIVING_POS1
	TFT_write_reg(ILI9225_PARTIAL_DRIVING_POS1, x+20);
	TFT_write_reg(ILI9225_PARTIAL_DRIVING_POS2, y-20);
	TFT_write_reg(ILI9225_VERTICAL_SCROLL_CTRL1, x);
	TFT_write_reg(ILI9225_VERTICAL_SCROLL_CTRL2, y);
	TFT_write_reg(ILI9225_VERTICAL_SCROLL_CTRL3, z);
}
//==============================================================================





Int32U	TFT_GetTextWidth(const char *str)
{
	Int32U i = 0, res = 0;
	char c;
	while(c = str[i])
	{
#ifdef TFT_ENABLE_PARSER
// ******** parser **********
		while (c == '[')
		{
			char c1 = str[i+1];
			// not code
			if (c1 == '[')
			{
				i++;
				c = c1;
				break;
			}

			// color, [C112233]        ([C112233]xxxxx[/C])
			if (c1 == 'C')
			{
				Int32S j = 0;
				for(j = 0; j < 8; j++)
				{
					c1 = str[i+2+j];
					if (
						!(c1 > 47 && c1 < 58) &&	// 0-9
						!(c1 > 64 && c1 < 71) &&	// A-F
						!(c1 > 96 && c1 < 103)		// a-f
						)
							break;
				}
				if (j < 8 && j > 0 && c1 == ']')
				{
					i += 3 + j;
					c = str[i];
					continue;
				}
				break;
			}
			// background, [F112233]        ([F112233]xxxxx[/F])
			if (c1 == 'F')
			{
				Int32S j = 0;
				for(j = 0; j < 8; j++)
				{
					c1 = str[i+2+j];
					if (
						!(c1 > 47 && c1 < 58) &&	// 0-9
						!(c1 > 64 && c1 < 71) &&	// A-F
						!(c1 > 96 && c1 < 103)		// a-f
						)
							break;
				}
				if (j < 8 && j > 0 && c1 == ']')
				{
					i += 3 + j;
					c = str[i];
					continue;
				}
				break;
			}
			// underline, [U]        ([U]xxxxx[/U])
			if (c1 == 'U')
			{
				if (str[i+2] == ']')
				{
					i += 3;
					c = str[i];
					continue;
				}
				break;
			}
			// overline, [O]        ([O]xxxxx[/O])
			if (c1 == 'O')
			{
				if (str[i+2] == ']')
				{
					i += 3;
					c = str[i];
					continue;
				}
				break;
			}
			// end code
			if (c1 == '/')
			{
				// end color, [/C]      ([C112233]xxxxx[/C])
				if (str[i+2] == 'C' && str[i+3] == ']')
				{
					i += 4;
					c = str[i];
					continue;
				}
				// end background, [/F]      ([F112233]xxxxx[/F])
				if (str[i+2] == 'F' && str[i+3] == ']')
				{
					i += 4;
					c = str[i];
					continue;
				}
				// end underline, [/U]      ([U]xxxxx[/U])
				if (str[i+2] == 'U' && str[i+3] == ']')
				{
					i += 4;
					c = str[i];
					continue;
				}
				// end overline, [/O]      ([O]xxxxx[/O])
				if (str[i+2] == 'O' && str[i+3] == ']')
				{
					i += 4;
					c = str[i];
					continue;
				}
			}
			break;
		}
// ********* end parser *********
#endif
		if (c > 31)
			res += TFT_text_getcharwidth(c);
		i++;
	}
	return res;
}
//==============================================================================





Int32U	TFT_GetCurrentFontHeight()
{
	return tft_current_font->height;
}
//==============================================================================





Int16U	TFT_GetScreenWidth()
{
	return tft_width;
}
//==============================================================================





Int16U	TFT_GetScreenHeight()
{
	return tft_height;
}
//==============================================================================





Int16U	TFT_GetCurrentCursorX()
{
	return tft_cursor_x;
}
//==============================================================================





Int16U	TFT_GetCurrentCursorY()
{
	return tft_cursor_y;
}
//==============================================================================





void	TFT_Clear()
{
	Int16U treg = 0;

	TFT_write_reg(ILI9225_RAM_ADDR_SET1, 0);
	TFT_write_reg(ILI9225_RAM_ADDR_SET2, 0);

	treg = TFTSPI->CR2 & (~SPI_DataSize_16b);
	TFTSPI->CR2 = treg;
	SPI_Cmd(TFTSPI, ENABLE);
	TFT_RS_LOW();
	TFT_CS_ON();
	*(Int8U*)&TFTSPI->DR = ILI9225_GRAM_DATA_REG;
	while ((TFTSPI->SR & SPI_I2S_FLAG_TXE) == 0);
	while ((TFTSPI->SR & SPI_I2S_FLAG_BSY) > 0);
	TFT_RS_HIGH();
	SPI_Cmd(TFTSPI, DISABLE);
	treg = TFTSPI->CR2 | SPI_DataSize_16b;
	TFTSPI->CR2 = treg;
	SPI_Cmd(TFTSPI, ENABLE);
	for (Int16U i = 0; i < tft_height; i++)
	{
		for (Int16U j = 0; j < tft_width; j++)
		{
/*
			if (tft_bggradient)
				TFTSPI->DR = gradback1[i];
			else
*/
				TFTSPI->DR =  tft_bgcolor;
			while(!(TFTSPI->SR & SPI_I2S_FLAG_TXE));
		}

	}
	while ((TFTSPI->SR & SPI_I2S_FLAG_TXE) == 0);
	while ((TFTSPI->SR & SPI_I2S_FLAG_BSY) > 0);
	TFT_CS_OFF();
	SPI_Cmd(TFTSPI, DISABLE);
	TFTSPI->CR2 = treg;
	tft_cursor_x = 0;
	tft_cursor_y = 0;
}
//==============================================================================





void	TFT_DrawPixel(Int16U x1, Int16U y1)
{
	if((x1 >= tft_width) || (y1 >= tft_height))
		return;
	TFT_write_reg(tft_ramAddrOne, x1);
	TFT_write_reg(tft_ramAddrTwo, y1);
	TFT_write_reg(ILI9225_GRAM_DATA_REG, tft_color);
}
//==============================================================================





void	TFT_DrawCircle(Int16U x0, Int16U y0, Int16U r)
{
	int16_t f = r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;
	TFT_DrawPixel(x0 - r, y0); // top left
	TFT_DrawPixel(x0, y0 + r); // top right
	TFT_DrawPixel(x0, y0-  r); // bottom right
	TFT_DrawPixel(x0 + r, y0); // bottom left

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

		TFT_DrawPixel(x0 + x, y0 + y);
		TFT_DrawPixel(x0 - x, y0 + y);
		TFT_DrawPixel(x0 + x, y0 - y);
		TFT_DrawPixel(x0 - x, y0 - y);
		TFT_DrawPixel(x0 + y, y0 + x);
		TFT_DrawPixel(x0 - y, y0 + x);
		TFT_DrawPixel(x0 + y, y0 - x);
		TFT_DrawPixel(x0 - y, y0 - x);
	}
}
//==============================================================================





void	TFT_DrawFastVLine(Int16S x, Int16S y, Int16S h)
{
	if((x >= tft_width) || (y >= tft_height))
		return;
	if((y+h) >= tft_height)
		h = tft_height-y;
	TFT_set_window(x, y, x, y+h-1);
	TFT_write_reg_beg(ILI9225_GRAM_DATA_REG);
	while (h-- > 0)
		TFT_write_data_cont(tft_color);
	TFT_write_data_last(tft_color);
	TFT_set_window(0, 0, tft_width-1, tft_height-1);
}
//==============================================================================





void	TFT_DrawFastHLine(Int16S x, Int16S y, Int16S w)
{
	if((x >= tft_width) || (y >= tft_height))
		return;
	if((x+w) >= tft_width)
		w = tft_width-x;
	TFT_set_window(x, y, x+w, y);
	TFT_write_reg_beg(ILI9225_GRAM_DATA_REG);
	while (w-- > 0)
		TFT_write_data_cont(tft_color);
	TFT_write_data_close();
	TFT_set_window(0, 0, tft_width-1, tft_height-1);
}
//==============================================================================





void	TFT_FillRect(Int16S x, Int16S y, Int16S w, Int16S h)
{
	if((x >= tft_width) || (y >= tft_height))
		return;
	if((x + w) >= tft_width)
		w = tft_width - x;
	if((y + h) >= tft_height)
		h = tft_height - y;

	TFT_set_window(x, y, x+w-1, y+h-1);
	TFT_write_reg_beg(ILI9225_GRAM_DATA_REG);
	h *= w;
	for(y = 0; y < h; y++)
	{
		TFT_write_data_cont(tft_color);
	}
	TFT_write_data_close();
	TFT_set_window(0, 0, tft_width-1, tft_height-1);
}
//==============================================================================





void	TFT_FillCircle(Int16S x0, Int16S y0, Int16S r)
{
	TFT_DrawFastVLine(x0, y0-r, 2*r);
	TFT_fill_circle_helper(x0, y0, r, 3, 0);
}
//==============================================================================





void	TFT_DrawLine(Int16U x0, Int16U y0, Int16U x1, Int16U y1)
{
	if (y0 == y1)
	{
		if (x1 > x0)
			TFT_DrawFastHLine(x0, y0, x1 - x0);
		else
			if (x1 < x0)
				TFT_DrawFastHLine(x1, y0, x0 - x1);
			else
				TFT_DrawPixel(x0, y0);
		return;
	}
	else
		if (x0 == x1)
		{
			if (y1 > y0)
				TFT_DrawFastVLine(x0, y0, y1 - y0);
			else
				TFT_DrawFastVLine(x0, y1, y0 - y1);
			return;
		}
	bool steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep)
	{
//		swap(x0, y0);
		Int16S s = x0;
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
		Int16S s = x0;
		x0 = x1;
		x1 = s;
//		swap(y0, y1);
		y0 = y1;
		y1 = s;
	}

	Int16S dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);
	Int16S err = dx / 2;
	Int16S ystep;

	if (y0 < y1)
		ystep = 1;
	else
		ystep = -1;

	Int16S xbegin = x0;
	if (steep)
	{
		for (; x0<=x1; x0++)
		{
			err -= dy;
			if (err < 0)
			{
				Int16S len = x0 - xbegin;
				if (len)
					TFT_DrawFastVLine(y0, xbegin, len + 1);
				else
					TFT_DrawPixel(y0, x0);
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1)
		TFT_DrawFastVLine(y0, xbegin, x0 - xbegin);
	}
	else
	{
		for (; x0<=x1; x0++)
		{
			err -= dy;
			if (err < 0)
			{
				Int16S len = x0 - xbegin;
				if (len)
					TFT_DrawFastHLine(xbegin, y0, len + 1);
				else
					TFT_DrawPixel(x0, y0);
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1)
			TFT_DrawFastHLine(xbegin, y0, x0 - xbegin);
	}
}
//==============================================================================





void	TFT_DrawRect(Int16S x, Int16S y, Int16S w, Int16S h)
{
	TFT_DrawFastHLine(x, y, w);
	TFT_DrawFastHLine(x, y+h, w);
	TFT_DrawFastVLine(x, y, h);
	TFT_DrawFastVLine(x+w, y, h);
}
//==============================================================================





void	TFT_DrawRoundRect(Int16S x, Int16S y, Int16S w, Int16S h, Int16S r)
{
	// smarter version
	TFT_DrawFastHLine(x+r, y, w-2*r); // Top
	TFT_DrawFastHLine(x+r, y+h, w-2*r); // Bottom
	TFT_DrawFastVLine(x, y+r, h-2*r); // Left
	TFT_DrawFastVLine(x+w, y+r, h-2*r); // Right
	// draw four corners
	TFT_draw_circle_helper(x+r, y+r, r, 1);
	TFT_draw_circle_helper(x+w-r, y+r, r, 2);
	TFT_draw_circle_helper(x+w-r, y+h-r, r, 4);
	TFT_draw_circle_helper(x+r, y+h-r, r, 8);
}
//==============================================================================





void	TFT_FillRoundRect(Int16S x, Int16S y, Int16S w, Int16S h, Int16S r)
{
	// smarter version
	TFT_FillRect(x+r, y, w-2*r, h);
	// draw four corners
	TFT_fill_circle_helper(x+w-r, y+r, r, 1, h-2*r);
	TFT_fill_circle_helper(x+r, y+r, r, 2, h-2*r);
}
//==============================================================================





void	TFT_DrawText(const char *str, Int16U opt, Int16S x1, Int16S y1, Int16S x2, Int16S y2)
{
	Int16U i = 0, oldcolor = tft_color, oldbgcolor = tft_bgcolor, copt = opt;
	char c;
	if (x1 > tft_width-1 || y1 > tft_height-1)
		return;
	if (x1 < 0)
		x1 = tft_cursor_x;
	if (y1 < 0)
		y1 = tft_cursor_y;
	if (x2 < 0)
		x2 = tft_width-1;
	if (y2 < 0)
		y2 = tft_height-1;
	tft_cursor_x = x1;
	tft_cursor_y = y1;



	char *cp = (char*)str;
	Int16S sp = 0, cw = 0, sw = 0;

	while(1)
	{
		c = cp[i];
		if (c == 0 || c == ' ')
		{
			sp = i;
			sw = cw;
		}
		cw += TFT_text_getcharwidth(c);
		if (c == '\n' || cw > x2-x1 || c == 0)
		{
			if (cw > x2-x1)
				cw = sw;
			if (sp == 0)
				sp = i;
			if (opt & TFT_TEXT_ALIGN_RIGHT)
				tft_cursor_x = x2 - cw;
			if (opt & TFT_TEXT_ALIGN_CENTER)
				tft_cursor_x = x1 + (x2 - x1 - cw) / 2;
			for (Int16U j = 0; j < sp; j++)
			{
#ifdef TFT_ENABLE_2B_FONTS
				if (tft_current_font->type == TFT_FONTTYPE_2BITS)
					TFT_text2b_char(cp[j], copt);
				else
#endif
					TFT_text1b_char(cp[j], copt);
			}
			cw = 0;
			sw = 0;
			cp += sp;
			i = 0;
			sp = 0;
			while(cp[0] == ' ')
				cp++;
			if (c != 0)
			{
				tft_cursor_y += tft_current_font->height;
				tft_cursor_x = x1;
			}
			if (c == 0 || tft_cursor_y+tft_current_font->height > y2)
				break;
			if (c == '\n')
				i++;;
			continue;
		}
		i++;
		if (c == '.' || c == ',' || c == ':' || c == ';' || c == '!' || c == '?')
		{
//			cw += TFT_text_getcharwidth(c);
			sp = i;
			sw = cw;
		}

#ifdef TFT_ENABLE_PARSER
// ******** parser **********
		while (c == '[')
		{
			char c1 = str[i+1];
			// not code
			if (c1 == '[')
			{
				i++;
				c = c1;
				break;
			}

			// color, [C112233]        ([C112233]xxxxx[/C])
			if (c1 == 'C')
			{
				Int32U color = 0x00000000;
				Int32S j = 0;
				for(j = 0; j < 8; j++)
				{
					c1 = str[i+2+j];
					if (c1 > 47 && c1 < 58)	// 0-9
						color = (color << 4) + (c1 - 48);
					else
						if (c1 > 64 && c1 < 71)	// A-F
							color = (color << 4) + (c1 - 55);
						else
							if (c1 > 96 && c1 < 103)	// a-f
								color = (color << 4) + (c1 - 87);
							else
								break;
				}
				if (j < 8 && j > 0 && c1 == ']')
				{
					tft_color = TFT_RGB(color);
					i += 3 + j;
					c = str[i];
					continue;
				}
				break;
			}
			// background, [F112233]        ([F112233]xxxxx[/F])
			if (c1 == 'F')
			{
				Int32U color = 0x00000000;
				Int32S j = 0;
				for(j = 0; j < 8; j++)
				{
					c1 = str[i+2+j];
					if (c1 > 47 && c1 < 58)	// 0-9
						color = (color << 4) + (c1 - 48);
					else
						if (c1 > 64 && c1 < 71)	// A-F
							color = (color << 4) + (c1 - 55);
						else
							if (c1 > 96 && c1 < 103)	// a-f
								color = (color << 4) + (c1 - 87);
							else
								break;
				}
				if (j < 8 && j > 0 && c1 == ']')
				{
					tft_bgcolor = TFT_RGB(color);
					i += 3 + j;
					c = str[i];
					continue;
				}
				break;
			}
			// underline, [U]        ([U]xxxxx[/U])
			if (c1 == 'U')
			{
				if (str[i+2] == ']')
				{
					copt |= TFT_TEXT_UNDERLINE;
					i += 3;
					c = str[i];
					continue;
				}
				break;
			}
			// overline, [O]        ([O]xxxxx[/O])
			if (c1 == 'O')
			{
				if (str[i+2] == ']')
				{
					copt |= TFT_TEXT_OVERLINE;
					i += 3;
					c = str[i];
					continue;
				}
				break;
			}
			// end code
			if (c1 == '/')
			{
				// end color, [/C]      ([C112233]xxxxx[/C])
				if (str[i+2] == 'C' && str[i+3] == ']')
				{
					tft_color = oldcolor;
					i += 4;
					c = str[i];
					continue;
				}
				// end background, [/F]      ([F112233]xxxxx[/F])
				if (str[i+2] == 'F' && str[i+3] == ']')
				{
					tft_bgcolor = oldbgcolor;
					i += 4;
					c = str[i];
					continue;
				}
				// end underline, [/U]      ([U]xxxxx[/U])
				if (str[i+2] == 'U' && str[i+3] == ']')
				{
					copt &= ~TFT_TEXT_UNDERLINE;
					i += 4;
					c = str[i];
					continue;
				}
				// end overline, [/O]      ([O]xxxxx[/O])
				if (str[i+2] == 'O' && str[i+3] == ']')
				{
					copt &= ~TFT_TEXT_OVERLINE;
					i += 4;
					c = str[i];
					continue;
				}
				break;
			}
		}
		if (c == 0)
			break;
// ********* end parser *********
#endif
	}

	tft_bgcolor = oldbgcolor;
	tft_color = oldcolor;
}
//==============================================================================





