#ifndef _LCD_LOW_H_
#define _LCD_LOW_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "main.h"
	 
//Hardware-related sub-functions
#define Lcd_Light_ON()		TFT_LIGHT_GPIO_Port->BSRR = TFT_LIGHT_Pin
#define Lcd_Light_OFF()		TFT_LIGHT_GPIO_Port->BSRR = (uint32_t)TFT_LIGHT_Pin << 16U

#define Lcd_SET()			TFT_RESET_GPIO_Port->BSRR = TFT_RESET_Pin
#define Lcd_RESET()			TFT_RESET_GPIO_Port->BSRR = (uint32_t)TFT_RESET_Pin << 16U

#define LCD_WIDTH			(uint16_t)480
#define LCD_HEIGHT			(uint16_t)320




void		LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue);
uint16_t	LCD_ReadReg(uint8_t LCD_Reg);
void		LCD_WriteCmd(uint16_t LCD_RegValue);
void		LCD_WriteRAM_Prepare(void);
void		LCD_WriteRAM_Continue(void);
void		LCD_WriteRAM(uint16_t RGB_Code);
void		LCD_WriteRAM_DMA(uint16_t *buff, uint16_t len);
void		LCD_WaitDMAReady();
void		LCD_ReadRAM_Prepare(void);
void		LCD_ReadRAM_Continue(void);
uint16_t	LCD_ReadRAM(void);
uint16_t	LCD_Read_ID(void);
uint16_t	LCD_get_lshift_freq(void);

void		LCD_CtrlLinesConfig(void);
void		LCD_FSMCConfig(void);
void		LCD_X_Init(void);

void		LCD_Initializtion(void);
void		LCD_SetCursor(uint16_t x, uint16_t y);
void		LCD_SetWindows(uint16_t StartX, uint16_t StartY, uint16_t width, uint16_t heigh);
void		LCD_Clear(uint16_t Color);

uint16_t	LCD_BGR2RGB(uint16_t c);
uint16_t	LCD_Color565(uint32_t c);
void		LCD_BackLight(uint8_t status);
void		LCD_WindowMax (unsigned int xsta,unsigned int ysta,unsigned int xend,unsigned int yend);

uint16_t	LCD_GetPoint(uint16_t x, uint16_t y);
void		LCD_SetPoint(uint16_t x,uint16_t y,uint16_t point);

void		LCD_DrawPicture(uint16_t StartX,uint16_t StartY,uint16_t EndX,uint16_t EndY,uint16_t *pic);
void		LCD_PutChar(uint16_t x,uint16_t y,uint8_t c,uint16_t charColor,uint16_t bkColor);
void		LCD_PutChar_16x24(uint16_t x,uint16_t y,uint8_t c,uint16_t charColor,uint16_t bkColor);


#ifdef __cplusplus
}
#endif

#endif