
/* Includes ------------------------------------------------------------------*/
//#include "stm32f10x.h"
//#include "stm32f4xx.h" //skyblue 2006-12-13
//#include "stm32f4xx_fsmc.h"   //skyblue
//#include "fsmc.h"       //skyblue 2006-12-13
#include "lcd_low.h"
#include "lcd_ui_fonts.h"
#include "lcd_low_fonts.h"
#include <stdio.h>
#include "gpio.h"
#include "fsmc.h"
#include "config.h"


uint16_t	DeviceCode;

uint16_t	LCD_WIDTH = 480;
uint16_t	LCD_HEIGHT = 320;


/* Private typedef -----------------------------------------------------------*/
typedef struct
{
	__IO uint16_t LCD_REG;
	__IO uint16_t LCD_RAM;
} LCD_TypeDef;

/* LCD is connected to the FSMC_Bank1_NOR/SRAM4 and NE4 is used as ship select signal */
#ifdef __CHITU_BOARD__
  #define LCD_BASE		((uint32_t)(0x6C000000 | 0x00001FFE))    //A12,NE4
#else
  #define LCD_BASE		((uint32_t)(0x60000000 | 0x0003FFFE))    //A17,NE1
#endif

#define LCD				((LCD_TypeDef *) LCD_BASE)


DMA_HandleTypeDef		hLcdDma;


void		LCD_CtrlLinesConfig(void)
{
	__IO uint32_t i=0;
	volatile unsigned long delayCnt=0;


//	Lcd_Light_OFF();
	Lcd_RESET();
	HAL_Delay(50);
	//      for(delayCnt=0;delayCnt<10000;delayCnt++);
	//HAL_Delay(500);
	Lcd_SET();


	HAL_FSMC_MspInit();		// initialization FSMC pins
	
	// DMA
	__HAL_RCC_DMA2_CLK_ENABLE();
	hLcdDma.Instance = DMA2_Stream7;
	hLcdDma.Init.Channel = DMA_CHANNEL_0;
	hLcdDma.Init.Direction = DMA_MEMORY_TO_MEMORY;
	hLcdDma.Init.PeriphInc = DMA_PINC_ENABLE;
	hLcdDma.Init.MemInc = DMA_MINC_DISABLE;
	hLcdDma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	hLcdDma.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	hLcdDma.Init.Mode = DMA_NORMAL;
	hLcdDma.Init.Priority = DMA_PRIORITY_LOW;
	hLcdDma.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	hLcdDma.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	hLcdDma.Init.MemBurst = DMA_MBURST_SINGLE;
	hLcdDma.Init.PeriphBurst = DMA_PBURST_SINGLE;
	HAL_DMA_Init(&hLcdDma);
	// DMA2_Stream7_IRQn interrupt configuration
	HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

}




void		LCD_FSMCConfig(void)
{
	MX_FSMC_Init();			// initialization FSMC core
}




void		LCD_X_Init(void)
{
	/* Configure the LCD Control pins ------------------------------------------- -*/
	LCD_CtrlLinesConfig();

	/* Configure the FSMC Parallel interface -------------------------------------*/
	LCD_FSMCConfig();
}




void		LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue)
{
	/* Write 16-bit Index, then Write Reg */
	LCD->LCD_REG = LCD_Reg;
	/* Write 16-bit Reg */
	LCD->LCD_RAM = LCD_RegValue;
}




uint16_t	LCD_ReadReg(uint8_t LCD_Reg)
{
	uint16_t data;	  /* Write 16-bit Index (then Read Reg) */
	//LCD->LCD_REG = LCD_Reg;
	data = LCD->LCD_RAM; 
	return    data;
}




void		LCD_WriteCmd(uint16_t LCD_RegValue)
{
	/* Write 16-bit Index, then Write Reg */
	LCD->LCD_REG = LCD_RegValue;
}




void		LCD_WriteRAM_Prepare(void)
{
	LCD_WriteCmd(0x002C);
}                                //**




void		LCD_WriteRAM_Continue(void)
{
	LCD_WriteCmd(0x003C);
}                                //**




void		LCD_WriteRAM(uint16_t RGB_Code)					 
{
	/* Write 16-bit GRAM Reg */
	LCD->LCD_RAM = RGB_Code;
}




void		LCD_WriteRAM_DMA(uint16_t *buff, uint16_t len)					 
{
	while (HAL_DMA_GetState(&hLcdDma) != HAL_DMA_STATE_READY);
		HAL_DMA_Start_IT(&hLcdDma, (uint32_t)buff, (uint32_t)&(LCD->LCD_RAM), len);
}




void		LCD_WaitDMAReady()
{
	while (HAL_DMA_GetState(&hLcdDma) != HAL_DMA_STATE_READY);
}




void		LCD_ReadRAM_Prepare(void)
{
		LCD_WriteCmd(0x002E);
		volatile uint16_t temp = LCD->LCD_RAM;
}




void		LCD_ReadRAM_Continue(void)
{
		LCD_WriteCmd(0x003E);
		volatile uint16_t temp = LCD->LCD_RAM;
}




uint16_t	LCD_ReadRAM(void)
{
	uint16_t temp;
	temp = LCD->LCD_RAM;
	return temp;
}





uint16_t	LCD_Read_ID(void) //**Read SSD1963ID
{
	uint16_t Lcd_ID;
	LCD_WriteCmd(0XA1);
	Lcd_ID=LCD_ReadRAM();
	Lcd_ID=LCD_ReadRAM(); //read back 0X57
	Lcd_ID<<=8;
	Lcd_ID|=LCD_ReadRAM(); //read back 0X61
	return Lcd_ID;
}




//Get the current clock frequency
uint16_t	LCD_get_lshift_freq(void)
{
	uint16_t data;
	LCD_WriteCmd(0xE7);
	data=LCD_ReadRAM();
	data=LCD_ReadRAM();
	data=LCD_ReadRAM();
	return data;
}




void		LCD_Init(void)
{
	volatile uint16_t i;
	volatile uint16_t data1, data2, data3;
	LCD_X_Init(); //Initialize the connection to the pins on the LCD color LCD screen, for example, the FSMC bus is connected here


	Lcd_SET();
	HAL_Delay(50);
	Lcd_RESET();
	HAL_Delay(50);
	Lcd_SET();

	{
		HAL_Delay(50);
		LCD_WriteReg(0x0000,0x0001);
		HAL_Delay(50);//Delay(5); /* delay 50 ms */ //start internal osc
		DeviceCode = LCD_ReadReg(0x0000);
		HAL_Delay(50);//Delay(5); /* delay 50 ms */
		DeviceCode = LCD_Read_ID(); //**Read SSD1963ID
	}
	if(DeviceCode != 0x5761)
		DeviceCode = LCD_ReadReg(0x0000);

	if(DeviceCode < 0XFF || DeviceCode == 0XFFFF || DeviceCode == 0)//Read the ID is incorrect, add lcddev.id==0X9300 judgment, because 9341 will be read as 9300 without reset
	{
		LCD_WriteCmd(0xD3);
		DeviceCode = LCD_ReadRAM(); //dummy read
		DeviceCode = LCD_ReadRAM(); //read to 0X00
		DeviceCode = LCD_ReadRAM(); //Read 93
		DeviceCode <<= 8;
		DeviceCode |= LCD_ReadRAM(); //Read 41
	}
	if(DeviceCode == 0x9488)
	{

/*		--- 9488 ---
0xe0 =>TFT_REG 
0x0  =>TFT_DATA
0x3  =>TFT_DATA
0xc  =>TFT_DATA
0x9  =>TFT_DATA
0x17 =>TFT_DATA
0x9  =>TFT_DATA
0x3e =>TFT_DATA
0x89 =>TFT_DATA
0x49 =>TFT_DATA
0x8  =>TFT_DATA
0xd  =>TFT_DATA
0xa  =>TFT_DATA
0x13 =>TFT_DATA
0x15 =>TFT_DATA
0xf  =>TFT_DATA
0xe1 =>TFT_REG 
0x0  =>TFT_DATA
0x11 =>TFT_DATA
0x15 =>TFT_DATA
0x3  =>TFT_DATA
0xf  =>TFT_DATA
0x5  =>TFT_DATA
0x2d =>TFT_DATA
0x34 =>TFT_DATA
0x41 =>TFT_DATA
0x2  =>TFT_DATA
0xb  =>TFT_DATA
0xa  =>TFT_DATA
0x33 =>TFT_DATA
0x37 =>TFT_DATA
0xf  =>TFT_DATA
0xc0 =>TFT_REG 
0x17 =>TFT_DATA
0x15 =>TFT_DATA
0xc1 =>TFT_REG 
0x41 =>TFT_DATA
0xc5 =>TFT_REG 
0x0  =>TFT_DATA
0x12 =>TFT_DATA
0x80 =>TFT_DATA
0x3a =>TFT_REG 
0x55 =>TFT_DATA
0xb0 =>TFT_REG 
0x0  =>TFT_DATA
0xb1 =>TFT_REG 
0xa0 =>TFT_DATA
0xb4 =>TFT_REG 
0x2  =>TFT_DATA
0xe9 =>TFT_REG 
0x0  =>TFT_DATA
0xf7 =>TFT_REG 
0xa9 =>TFT_DATA
0x51 =>TFT_DATA
0x2c =>TFT_DATA
0x82 =>TFT_DATA
0xb6 =>TFT_REG 
0x2  =>TFT_DATA
0x2  =>TFT_DATA
0x36 =>TFT_REG 
0xe8 =>TFT_DATA
0x11 =>TFT_REG 
0x78 FUN_0804fd30          
0x29 =>TFT_REG 

*/
		//************* Start Initial Sequence **********//
		LCD_WriteCmd(0x00E0);
		LCD_WriteRAM(0x0000);
		LCD_WriteRAM(0x0007);
		LCD_WriteRAM(0x000f);
		LCD_WriteRAM(0x000D);
		LCD_WriteRAM(0x001B);
		LCD_WriteRAM(0x000A);
		LCD_WriteRAM(0x003c);
		LCD_WriteRAM(0x0078);
		LCD_WriteRAM(0x004A);
		LCD_WriteRAM(0x0007);
		LCD_WriteRAM(0x000E);
		LCD_WriteRAM(0x0009);
		LCD_WriteRAM(0x001B);
		LCD_WriteRAM(0x001e);
		LCD_WriteRAM(0x000f);

		LCD_WriteCmd(0x00E1);
		LCD_WriteRAM(0x0000);
		LCD_WriteRAM(0x0022);
		LCD_WriteRAM(0x0024);
		LCD_WriteRAM(0x0006);
		LCD_WriteRAM(0x0012);
		LCD_WriteRAM(0x0007);
		LCD_WriteRAM(0x0036);
		LCD_WriteRAM(0x0047);
		LCD_WriteRAM(0x0047);
		LCD_WriteRAM(0x0006);
		LCD_WriteRAM(0x000a);
		LCD_WriteRAM(0x0007);
		LCD_WriteRAM(0x0030);
		LCD_WriteRAM(0x0037);
		LCD_WriteRAM(0x000f);

		LCD_WriteCmd(0x00C0);
		LCD_WriteRAM(0x0010);
		LCD_WriteRAM(0x0010);

		LCD_WriteCmd(0x00C1);
		LCD_WriteRAM(0x0041);

		LCD_WriteCmd(0x00C5);
		LCD_WriteRAM(0x0000);
		LCD_WriteRAM(0x0022);
		LCD_WriteRAM(0x0080);

		LCD_WriteCmd(0x0036);
		//LCD_WriteRAM(0x0068);
//		LCD_WriteRAM(0x0098);	// rotate 90
//		LCD_WriteRAM(0x0078);	// rotate 180
		if (cfgConfig.display_rotate == 0)
			LCD_WriteRAM(0x00B8);
		else
			LCD_WriteRAM(0x0078);

		LCD_WriteCmd(0x003A); //Interface Mode Control
		LCD_WriteRAM(0x0055);

		LCD_WriteCmd(0X00B0); //Interface Mode Control
		LCD_WriteRAM(0x0000);
		
		LCD_WriteCmd(0x00B1); //Frame rate 70HZ
		LCD_WriteRAM(0x00B0);
		LCD_WriteRAM(0x0011);
		
		LCD_WriteCmd(0x00B4);
		LCD_WriteRAM(0x0002);
		
		LCD_WriteCmd(0x00B6); //RGB/MCU Interface Control
		LCD_WriteRAM(0x0002);
		LCD_WriteRAM(0x0042);

		LCD_WriteCmd(0x00B7);
		LCD_WriteRAM(0x00C6);

		//WriteComm(0XBE);
		//WriteData(0x00);
		//WriteData(0x04);

		LCD_WriteCmd(0x00E9);
		LCD_WriteRAM(0x0000);

		LCD_WriteCmd(0X00F7);
		LCD_WriteRAM(0x00A9);
		LCD_WriteRAM(0x0051);
		LCD_WriteRAM(0x002C);
		LCD_WriteRAM(0x0082);

		LCD_WriteCmd(0x0011);
		HAL_Delay(120);
		LCD_WriteCmd(0x0029);

		LCD_SetWindows(0,0,480,320);
		LCD_Clear(0x0000);

		LCD_WIDTH = 480;
		LCD_HEIGHT = 320;
	} else
	if(DeviceCode == 0x9486)
	{
/*		--- 9486 ---
0xf2 =>TFT_REG 
0x18 =>TFT_DATA
0xa3 =>TFT_DATA
0x12 =>TFT_DATA
0x2  =>TFT_DATA
0xb2 =>TFT_DATA
0x12 =>TFT_DATA
0xff =>TFT_DATA
0x10 =>TFT_DATA
0x0  =>TFT_DATA
0xf8 =>TFT_REG 
0x21 =>TFT_DATA
0x4  =>TFT_DATA
0xf9 =>TFT_REG 
0x0  =>TFT_DATA
0x8  =>TFT_DATA
0x36 =>TFT_REG 
0xa8 =>TFT_DATA
0x3a =>TFT_REG 
0x5  =>TFT_DATA
0xb4 =>TFT_REG 
0x1  =>TFT_DATA
0xb6 =>TFT_REG 
0x2  =>TFT_DATA
0x22 =>TFT_DATA
0xc1 =>TFT_REG 
0x41 =>TFT_DATA
0xc5 =>TFT_REG 
0x0  =>TFT_DATA
0x7  =>TFT_DATA
0xe0 =>TFT_REG 
0xf  =>TFT_DATA
0x1f =>TFT_DATA
0x1c =>TFT_DATA
0xc  =>TFT_DATA
0xf  =>TFT_DATA
0x8  =>TFT_DATA
0x48 =>TFT_DATA
0x98 =>TFT_DATA
0x37 =>TFT_DATA
0xa  =>TFT_DATA
0x13 =>TFT_DATA
0x4  =>TFT_DATA
0x11 =>TFT_DATA
0xd  =>TFT_DATA
0x0  =>TFT_DATA
0xe1 =>TFT_REG 
0xf  =>TFT_DATA
0x32 =>TFT_DATA
0x2e =>TFT_DATA
0xb  =>TFT_DATA
0xd  =>TFT_DATA
0x5  =>TFT_DATA
0x47 =>TFT_DATA
0x75 =>TFT_DATA
0x37 =>TFT_DATA
0x6  =>TFT_DATA
0x10 =>TFT_DATA
0x3  =>TFT_DATA
0x24 =>TFT_DATA
0x20 =>TFT_DATA
0x0  =>TFT_DATA
0x11 =>TFT_REG 
0x29 =>TFT_REG 
*/
		LCD_WIDTH = 480;
		LCD_HEIGHT = 320;
	} else
	if(DeviceCode == 0x9481)
	{
/*		--- 9481 ---
0x11 =>TFT_REG 
0x14 FUN_0804fd30          
0xd0 =>TFT_REG 
0x7  =>TFT_DATA
0x42 =>TFT_DATA
0x18 =>TFT_DATA
0xd1 =>TFT_REG 
0x0  =>TFT_DATA
0x7  =>TFT_DATA
0x10 =>TFT_DATA
0xd2 =>TFT_REG 
0x1  =>TFT_DATA
0x2  =>TFT_DATA
0xc0 =>TFT_REG 
0x10 =>TFT_DATA
0x3b =>TFT_DATA
0x0  =>TFT_DATA
0x2  =>TFT_DATA
0x11 =>TFT_DATA
0xc5 =>TFT_REG 
0x3  =>TFT_DATA
0xc8 =>TFT_REG 
0x0  =>TFT_DATA
0x32 =>TFT_DATA
0x36 =>TFT_DATA
0x45 =>TFT_DATA
0x6  =>TFT_DATA
0x16 =>TFT_DATA
0x37 =>TFT_DATA
0x75 =>TFT_DATA
0x77 =>TFT_DATA
0x54 =>TFT_DATA
0xc  =>TFT_DATA
0x0  =>TFT_DATA
0x36 =>TFT_REG 
0xa  =>TFT_DATA
0x3a =>TFT_REG 
0x55 =>TFT_DATA
0x2a =>TFT_REG 
0x0  =>TFT_DATA
0x0  =>TFT_DATA
0x1  =>TFT_DATA
0x3f =>TFT_DATA
0x2b =>TFT_REG 
0x0  =>TFT_DATA
0x0  =>TFT_DATA
0x1  =>TFT_DATA
0xe0 =>TFT_DATA
0x29 =>TFT_REG 
0x2c =>TFT_REG 

*/
		LCD_WIDTH = 480;
		LCD_HEIGHT = 320;
	} else
	if(DeviceCode == 0x9341)
	{
/*		--- 9341 ---
0x3a =>TFT_REG  
0x55 =>TFT_DATA 
0xf6 =>TFT_REG  
0x1  =>TFT_DATA  
0x33 =>TFT_DATA 
0xb5 =>TFT_REG  
0x4  =>TFT_DATA  
0x4  =>TFT_DATA  
0xa  =>TFT_DATA  
0x14 =>TFT_DATA 
0x35 =>TFT_REG  
0x0  =>TFT_DATA  
0xcf =>TFT_REG  
0x0  =>TFT_DATA  
0xea =>TFT_DATA 
0xf0 =>TFT_DATA 
0xed =>TFT_REG  
0x64 =>TFT_DATA 
0x3 =>TFT_DATA  
0x12 =>TFT_DATA 
0x81 =>TFT_DATA 
0xe8 =>TFT_REG  
0x85 =>TFT_DATA 
0x0  =>TFT_DATA  
0x78 =>TFT_DATA 
0xcb =>TFT_REG  
0x39 =>TFT_DATA 
0x2c =>TFT_DATA 
0x0  =>TFT_DATA  
0x33 =>TFT_DATA 
0x6  =>TFT_DATA  
0xf7 =>TFT_REG  
0x20 =>TFT_DATA 
0xea =>TFT_REG  
0x0  =>TFT_DATA  
0x0  =>TFT_DATA  
0xc0 =>TFT_REG  
0x21 =>TFT_DATA 
0xc1 =>TFT_REG  
0x10 =>TFT_DATA 
0xc5 =>TFT_REG  
0x4f =>TFT_DATA 
0x38 =>TFT_DATA 
0xc7 =>TFT_REG  
0x98 =>TFT_DATA 
0x36 =>TFT_REG  
0xa8 =>TFT_DATA 
0xb1 =>TFT_REG  
0x0  =>TFT_DATA  
0x13 =>TFT_DATA 
0xb6 =>TFT_REG  
0xa  =>TFT_DATA  
0xa2 =>TFT_DATA 
0xf2 =>TFT_REG  
0x2  =>TFT_DATA  
0xe0 =>TFT_REG  
0xf  =>TFT_DATA  
0x27 =>TFT_DATA 
0x24 =>TFT_DATA 
0xc  =>TFT_DATA  
0x10 =>TFT_DATA 
0x8  =>TFT_DATA  
0x55 =>TFT_DATA 
0x87 =>TFT_DATA 
0x45 =>TFT_DATA 
0x8  =>TFT_DATA  
0x14 =>TFT_DATA 
0x7  =>TFT_DATA  
0x13 =>TFT_DATA 
0x8  =>TFT_DATA  
0x0  =>TFT_DATA  
0xe1 =>TFT_REG  
0x0  =>TFT_DATA  
0xf  =>TFT_DATA  
0x12 =>TFT_DATA 
0x5  =>TFT_DATA  
0x11 =>TFT_DATA 
0x6  =>TFT_DATA  
0x25 =>TFT_DATA 
0x34 =>TFT_DATA 
0x37 =>TFT_DATA 
0x1  =>TFT_DATA  
0x8  =>TFT_DATA  
0x7  =>TFT_DATA  
0x2b =>TFT_DATA 
0x34 =>TFT_DATA 
0xf  =>TFT_DATA  
0x11 =>TFT_REG  
0x29 =>TFT_REG  

*/

		LCD_WriteCmd(0x003a); // =>TFT_REG  
		LCD_WriteRAM(0x0055); // =>TFT_DATA 
		LCD_WriteCmd(0x00f6); // =>TFT_REG  
		LCD_WriteRAM(0x0001); //  =>TFT_DATA  
		LCD_WriteRAM(0x0033); // =>TFT_DATA 
		LCD_WriteCmd(0x00b5); // =>TFT_REG  
		LCD_WriteRAM(0x0004); //  =>TFT_DATA  
		LCD_WriteRAM(0x0004); //  =>TFT_DATA  
		LCD_WriteRAM(0x000a); //  =>TFT_DATA  
		LCD_WriteRAM(0x0014); // =>TFT_DATA 
		LCD_WriteCmd(0x0035); // =>TFT_REG  
		LCD_WriteRAM(0x0000); //  =>TFT_DATA  
		LCD_WriteCmd(0x00cf); // =>TFT_REG  
		LCD_WriteRAM(0x0000); //  =>TFT_DATA  
		LCD_WriteRAM(0x00ea); // =>TFT_DATA 
		LCD_WriteRAM(0x00f0); // =>TFT_DATA 
		LCD_WriteCmd(0x00ed); // =>TFT_REG  
		LCD_WriteRAM(0x0064); // =>TFT_DATA 
		LCD_WriteRAM(0x0003); // =>TFT_DATA  
		LCD_WriteRAM(0x0012); // =>TFT_DATA 
		LCD_WriteRAM(0x0081); // =>TFT_DATA 
		LCD_WriteCmd(0x00e8); // =>TFT_REG  
		LCD_WriteRAM(0x0085); // =>TFT_DATA 
		LCD_WriteRAM(0x0000); //  =>TFT_DATA  
		LCD_WriteRAM(0x0078); // =>TFT_DATA 
		LCD_WriteCmd(0x00cb); // =>TFT_REG  
		LCD_WriteRAM(0x0039); // =>TFT_DATA 
		LCD_WriteRAM(0x002c); // =>TFT_DATA 
		LCD_WriteRAM(0x0000); //  =>TFT_DATA  
		LCD_WriteRAM(0x0033); // =>TFT_DATA 
		LCD_WriteRAM(0x0006); //  =>TFT_DATA  
		LCD_WriteCmd(0x00f7); // =>TFT_REG  
		LCD_WriteRAM(0x0020); // =>TFT_DATA 
		LCD_WriteCmd(0x00ea); // =>TFT_REG  
		LCD_WriteRAM(0x0000); //  =>TFT_DATA  
		LCD_WriteRAM(0x0000); //  =>TFT_DATA  
		LCD_WriteCmd(0x00c0); // =>TFT_REG  
		LCD_WriteRAM(0x0021); // =>TFT_DATA 
		LCD_WriteCmd(0x00c1); // =>TFT_REG  
		LCD_WriteRAM(0x0010); // =>TFT_DATA 
		LCD_WriteCmd(0x00c5); // =>TFT_REG  
		LCD_WriteRAM(0x004f); // =>TFT_DATA 
		LCD_WriteRAM(0x0038); // =>TFT_DATA 
		LCD_WriteCmd(0x00c7); // =>TFT_REG  
		LCD_WriteRAM(0x0098); // =>TFT_DATA 
		LCD_WriteCmd(0x0036); // =>TFT_REG  
		LCD_WriteRAM(0x00a8); // =>TFT_DATA 
		LCD_WriteCmd(0x00b1); // =>TFT_REG  
		LCD_WriteRAM(0x0000); //  =>TFT_DATA  
		LCD_WriteRAM(0x0013); // =>TFT_DATA 
		LCD_WriteCmd(0x00b6); // =>TFT_REG  
		LCD_WriteRAM(0x000a); //  =>TFT_DATA  
		LCD_WriteRAM(0x00a2); // =>TFT_DATA 
		LCD_WriteCmd(0x00f2); // =>TFT_REG  
		LCD_WriteRAM(0x0002); //  =>TFT_DATA  
		LCD_WriteCmd(0x00e0); // =>TFT_REG  
		LCD_WriteRAM(0x000f); //  =>TFT_DATA  
		LCD_WriteRAM(0x0027); // =>TFT_DATA 
		LCD_WriteRAM(0x0024); // =>TFT_DATA 
		LCD_WriteRAM(0x000c); //  =>TFT_DATA  
		LCD_WriteRAM(0x0010); // =>TFT_DATA 
		LCD_WriteRAM(0x0008); //  =>TFT_DATA  
		LCD_WriteRAM(0x0055); // =>TFT_DATA 
		LCD_WriteRAM(0x0087); // =>TFT_DATA 
		LCD_WriteRAM(0x0045); // =>TFT_DATA 
		LCD_WriteRAM(0x0008); //  =>TFT_DATA  
		LCD_WriteRAM(0x0014); // =>TFT_DATA 
		LCD_WriteRAM(0x0007); //  =>TFT_DATA  
		LCD_WriteRAM(0x0013); // =>TFT_DATA 
		LCD_WriteRAM(0x0008); //  =>TFT_DATA  
		LCD_WriteRAM(0x0000); //  =>TFT_DATA  
		LCD_WriteCmd(0x00e1); // =>TFT_REG  
		LCD_WriteRAM(0x0000); //  =>TFT_DATA  
		LCD_WriteRAM(0x000f); //  =>TFT_DATA  
		LCD_WriteRAM(0x0012); // =>TFT_DATA 
		LCD_WriteRAM(0x0005); //  =>TFT_DATA  
		LCD_WriteRAM(0x0011); // =>TFT_DATA 
		LCD_WriteRAM(0x0006); //  =>TFT_DATA  
		LCD_WriteRAM(0x0025); // =>TFT_DATA 
		LCD_WriteRAM(0x0034); // =>TFT_DATA 
		LCD_WriteRAM(0x0037); // =>TFT_DATA 
		LCD_WriteRAM(0x0001); //  =>TFT_DATA  
		LCD_WriteRAM(0x0008); //  =>TFT_DATA  
		LCD_WriteRAM(0x0007); //  =>TFT_DATA  
		LCD_WriteRAM(0x002b); // =>TFT_DATA 
		LCD_WriteRAM(0x0034); // =>TFT_DATA 
		LCD_WriteRAM(0x000f); //  =>TFT_DATA  
		LCD_WriteCmd(0x0011); // =>TFT_REG  
		LCD_WriteCmd(0x0029); // =>TFT_REG  
		
		HAL_Delay(300);

		LCD_WIDTH = 320;
		LCD_HEIGHT = 240;
	}
		
}




/************************************************* ***************************
* Name: void LCD_SetCursor(uint16_t x,uint16_t y)
* Function: set screen coordinates
* Entry parameters: x line coordinates
* y column coordinates
* Export parameters: none
* Explanation:
* Calling method: ili9320_SetCursor(10,10);
************************************************** **************************/
//inline void LCD_SetCursor(uint16_t x,uint16_t y)
void		LCD_SetCursor(uint16_t x, uint16_t y)
{
	LCD_WriteCmd(0X002A);
	LCD_WriteRAM(x>>8);
	LCD_WriteRAM(x&0X00FF);
	LCD_WriteRAM(x>>8);
	LCD_WriteRAM(x&0X00FF);
	//LCD_WriteRAM(0X01);
	//LCD_WriteRAM(0XDF);
	LCD_WriteCmd(0X002B);
	LCD_WriteRAM(y>>8);
	LCD_WriteRAM(y&0X00FF);
	LCD_WriteRAM(y>>8);
	LCD_WriteRAM(y&0X00FF);
	//LCD_WriteRAM(0X01);
	//LCD_WriteRAM(0X3F);
}




/************************************************* ***************************
* Name: void LCD_SetWindows(uint16_t StartX, uint16_t StartY, uint16_t EndX, uint16_t EndY)
* Function: Set window area
* Entry parameters: StartX line start coordinates
* Start Y column start coordinates
* EndX line end coordinates
* EndY column end coordinates
* Export parameters: none
* Explanation:
* Calling method: LCD_SetWindows(0,0,100,100);
************************************************** **************************/
//inline void LCD_SetWindows(uint16_t StartX,uint16_t StartY,uint16_t EndX,uint16_t EndY)
void		LCD_SetWindows(uint16_t StartX, uint16_t StartY, uint16_t width, uint16_t heigh)
{
	uint16_t s_h, s_l, e_h, e_l;
	s_h = (StartX >> 8) & 0X00ff;
	s_l = StartX & 0X00ff;
	e_h = ((StartX + width-1) >> 8) & 0X00ff;
	e_l = (StartX + width-1) & 0X00ff;

	LCD_WriteCmd(0x002A);
	LCD_WriteRAM(s_h);
	LCD_WriteRAM(s_l);
	LCD_WriteRAM(e_h);
	LCD_WriteRAM(e_l);

	s_h = (StartY >> 8) & 0X00ff;
	s_l = StartY & 0X00ff;
	e_h = ((StartY + heigh-1) >> 8) & 0X00ff;
	e_l = (StartY + heigh-1) & 0X00ff;

	LCD_WriteCmd(0x002B);
	LCD_WriteRAM(s_h);
	LCD_WriteRAM(s_l);
	LCD_WriteRAM(e_h);
	LCD_WriteRAM(e_l);
}




/************************************************* ***************************
* Name: void LCD_Clear(uint16_t dat)
* Function: fill the screen with a specified color, such as clearing the screen, then fill it with 0xffff
* Entry parameter: dat filling value
* Export parameters: none
* Explanation:
* Calling method: LCD_Clear(0xffff);
************************************************** **************************/
void		LCD_Clear(uint16_t Color)
{
	uint32_t index = 0;

	LCD_SetCursor(0,0);
	LCD_SetWindows(0,0,480,320);
	LCD_WriteRAM_Prepare();
	for(index=0;index<320*480;index++)
	{
		LCD->LCD_RAM=Color;
	}
}




/************************************************* ***************************
* Name: uint16_t LCD_GetPoint(uint16_t x, uint16_t y)
* Function: Get the color value of the specified coordinate
* Entry parameters: x line coordinates
* y column coordinates
* Export parameters: current coordinate color value
* Explanation:
* Calling method: i=LCD_GetPoint(10,10);
************************************************** **************************/
uint16_t	LCD_GetPoint(uint16_t x, uint16_t y)
{
	LCD_SetCursor(x,y);
	return (LCD_BGR2RGB(LCD_ReadRAM()));
}




/************************************************* ***************************
* Name: void LCD_SetPoint(uint16_t x,uint16_t y,uint16_t point)
* Function: draw a point at the specified coordinate
* Entry parameters: x line coordinates
* y column coordinates
* point color
* Export parameters: none
* Explanation:
* Calling method: LCD_SetPoint(10,10,0x0fe0);
************************************************** **************************/
void		LCD_SetPoint(uint16_t x,uint16_t y,uint16_t point)
{
	if ((x>480)||(y>320))
		return;
	LCD_SetCursor(x,y); /*Set the cursor position*/

	LCD_WriteRAM_Prepare(); /* Start writing to GRAM*/
	LCD_WriteRAM(point);
}




/************************************************* ***************************
* Name: void LCD_DrawPicture(uint16_t StartX,uint16_t StartY,uint16_t EndX,uint16_t EndY,uint16_t *pic)
* Function: display a picture in the specified coordinate range
* Entry parameters: StartX line start coordinates
* Start Y column start coordinates
* EndX line end coordinates
* EndY column end coordinates
pic picture header pointer
* Export parameters: none
* Explanation: The image modulus format is horizontal scan, 16-bit color mode
* Calling method: LCD_DrawPicture(0,0,100,100,(uint16_t*)demo);
* Author: www.armjishu.com
************************************************** **************************/
void		LCD_DrawPicture(uint16_t StartX,uint16_t StartY,uint16_t EndX,uint16_t EndY,uint16_t *pic)
{
	uint32_t i, total;
	uint16_t data1,data2,data3;
	uint16_t *picturepointer = pic;
	uint16_t x,y;

//	printf("LCD_DrawPicture StartX %d StartY %d EndX %d EndY %d \n\r", StartX, StartY, EndX, EndY);

	x=StartX;
	y=StartY;

	total = (EndX-StartX + 1)*(EndY-StartY + 1 )/2;

	for (i=0;i<total;i++)
	{
		data1 = *picturepointer++;
		data2 = *picturepointer++;
		data3 = ((data1 >>3)& 0x001f) |((data1>>5) & 0x07E0) | ((data2<<8) & 0xF800);
		LCD_SetPoint(x,y,data3);
		y++;
		if(y> EndY)
		{
			x++;
			y=StartY;
		}


		data1 = data2;
		data2 = *picturepointer++;
		data3 = ((data1 >>11)& 0x001f) |((data2<<3) & 0x07E0) | ((data2) & 0xF800);
		LCD_SetPoint(x,y,data3);
		y++;
		if(y> EndY)
		{
			x++;
			y=StartY;
		}
	}

}




/************************************************* ***************************
* Name: void LCD_PutChar(uint16_t x,uint16_t y,uint8_t c,uint16_t charColor,uint16_t bkColor)
* Function: display an 8x16 dot matrix ascii character at the specified coordinate
* Entry parameters: x line coordinates
* y column coordinates
* charColor character color
* bkColor character background color
* Export parameters: none
* Explanation: The display range is limited to the ascii codes that can be displayed
* Calling method: LCD_PutChar(10,10,'a',0x0000,0xffff);
************************************************** **************************/
void		LCD_PutChar(uint16_t x,uint16_t y,uint8_t c,uint16_t charColor,uint16_t bkColor) // Lihao
{
/*
	uint16_t i=0;
	uint16_t j=0;

	uint8_t tmp_char=0;

	if(HyalineBackColor == bkColor)
	{
		for (i=0;i<16;i++)
		{
			tmp_char=ascii_8x16[((c-0x20)*16)+i];
			for (j=0;j<8;j++)
			{
				if ((tmp_char >> 7-j) & 0x01 == 0x01)
				{
					LCD_SetPoint(x+j,y+i,charColor); // character color
				}
				else
				{
				// do nothing // transparent background
				}
			}
		}
	}
	else
	{
		for (i=0;i<16;i++)
		{
			tmp_char=ascii_8x16[((c-0x20)*16)+i];
			for (j=0;j<8;j++)
			{
				if ((tmp_char >> 7-j) & 0x01 == 0x01)
				{
					LCD_SetPoint(x+j,y+i,charColor); // character color
				}
				else
				{
					LCD_SetPoint(x+j,y+i,bkColor); // background color
				}
			}
		}
	}
*/
}




/************************************************* ***************************
* Name: void LCD_PutChar(uint16_t x,uint16_t y,uint8_t c,uint16_t charColor,uint16_t bkColor)
* Function: display an 8x16 dot matrix ascii character at the specified coordinate
* Entry parameters: x line coordinates
* y column coordinates
* charColor character color
* bkColor character background color
* Export parameters: none
* Explanation: The display range is limited to the ascii codes that can be displayed
* Calling method: LCD_PutChar(10,10,'a',0x0000,0xffff);
************************************************** **************************/
void		LCD_PutChar_16x24(uint16_t x,uint16_t y,uint8_t c,uint16_t charColor,uint16_t bkColor)
{
/*
	uint16_t i=0;
	uint16_t j=0;

	uint16_t tmp_char=0;

	if(HyalineBackColor == bkColor)
	{
		for (i=0;i<24;i++)
		{
			tmp_char=ASCII_Table_16x24[((c-0x20)*24)+i];
			for (j=0;j<16;j++)
			{
				if ((tmp_char >> j) & 0x01 == 0x01)
				{
					LCD_SetPoint(x+j,y+i,charColor); // character color
				}
				else
				{
				// do nothing // transparent background
				}
			}
		}
	}
	else
	{
		for (i=0;i<24;i++)
		{
			tmp_char=ASCII_Table_16x24[((c-0x20)*24)+i];
			for (j=0;j<16;j++)
			{
				if ((tmp_char >> j) & 0x01 == 0x01)
				{
					LCD_SetPoint(x+j,y+i,charColor); // character color
				}
				else
				{
					LCD_SetPoint(x+j,y+i,bkColor); // background color
				}
			}
		}
	}
*/
}




/************************************************* ***************************
* Name: uint16_t LCD_BGR2RGB(uint16_t c)
* Function: RRRRRGGGGGGBBBBB changed to BBBBBGGGGGGRRRRR format
* Entry parameters: c BRG color value
* Export parameters: RGB color value
* Explanation: Internal function call
* Calling method:
************************************************** **************************/
uint16_t	LCD_BGR2RGB(uint16_t c)
{
	uint16_t r, g, b, rgb;

	b = (c>>0) & 0x1f;
	g = (c>>5) & 0x3f;
	r = (c>>11) & 0x1f;

	rgb = (b<<11) + (g<<5) + (r<<0);

	return( rgb );
}




uint16_t	LCD_Color565(uint32_t c)
{
	uint32_t R, G, B;
	uint16_t rgb16;

	R = (c& 0xFF0000);
	G = (c& 0x00FF00);
	B = (c & 0x0000FF);

	rgb16 = (uint16_t)((R>>3) + (G>>2) + (B>>3));

	return(LCD_BGR2RGB(rgb16));
}




/************************************************* ***************************
* Name: void LCD_BackLight(uint8_t status)
* Function: turn on and off the LCD backlight
* Entry parameters: status 1: backlight on 0: backlight off
* Export parameters: none
* Explanation:
* Calling method: LCD_BackLight(1);
************************************************** **************************/
void		LCD_BackLight(uint8_t status)
{
	//#if 0 //skyblue 2016-12-13
	if (status >= 1)
	{
		Lcd_Light_ON();
	}
	else
	{
		Lcd_Light_OFF();
	}
	//#endif //skyblue 2016-12-13
}



void		LCD_WindowMax (unsigned int xsta,unsigned int ysta,unsigned int xend,unsigned int yend) //**
{
	LCD_WriteCmd(0X002A);
	LCD_WriteRAM(xsta>>8);
	LCD_WriteRAM(xsta&0X00FF);
	LCD_WriteRAM(xend>>8);
	LCD_WriteRAM(xend&0X00FF);

	LCD_WriteCmd(0X002B);
	LCD_WriteRAM(ysta>>8);
	LCD_WriteRAM(ysta&0X00FF);
	LCD_WriteRAM(yend>>8);
	LCD_WriteRAM(yend&0X00FF);
}




