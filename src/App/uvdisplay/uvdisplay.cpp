#include "uvdisplay.h"
#include "fst_spi.h"
//#include "ssd2828.h"




uint8_t					Line_Pixel[3000];











uint8_t		UVD_Init()
{
	CPLD_Init();
#ifdef __MKSDLP_BOARD__
	return SSD_Init();
#endif  // __MKSDLP_BOARD__

#ifdef __CHITU_BOARD__
  #ifdef __CHITU_LV3__
	uint8_t		res[20] = {0};

	CPLD_EnableCS();
	for (uint32_t p = 0; p < 20; p++)
		__asm("nop");
	res[0] = CPLD_SendReceiveByte(0xFE);
	res[1] = CPLD_SendReceiveByte(0x00);
	CPLD_DisableCS();
	for (uint32_t p = 0; p < 50; p++)
		__asm("nop");
	CPLD_EnableCS();
	for (uint32_t p = 0; p < 5; p++)
		__asm("nop");
	res[0] = CPLD_SendReceiveByte(0xFE);
	res[1] = CPLD_SendReceiveByte(0x03);
	CPLD_DisableCS();
	for (uint32_t p = 0; p < 50; p++)
		__asm("nop");
	CPLD_EnableCS();
	for (uint32_t p = 0; p < 5; p++)
		__asm("nop");
	res[0] = CPLD_SendReceiveByte(0xFE);
	res[1] = CPLD_SendReceiveByte(0x04);
	CPLD_DisableCS();
	for (uint32_t p = 0; p < 50; p++)
		__asm("nop");
	CPLD_EnableCS();
	for (uint32_t p = 0; p < 5; p++)
		__asm("nop");
	for (uint32_t i = 0; i < 17; i++)
	{
		res[i] = CPLD_SendReceiveByte(0xFE);
	}
	CPLD_DisableCS();
	for (uint32_t p = 0; p < 50; p++)
		__asm("nop");
	CPLD_EnableCS();
	for (uint32_t p = 0; p < 5; p++)
		__asm("nop");
	res[0] = CPLD_SendReceiveByte(0xFE);
	res[1] = CPLD_SendReceiveByte(0x9D);
	CPLD_DisableCS();
	for (uint32_t p = 0; p < 50; p++)
		__asm("nop");
	CPLD_EnableCS();
	for (uint32_t p = 0; p < 5; p++)
		__asm("nop");
	res[0] = CPLD_SendReceiveByte(0xFE);
	res[1] = CPLD_SendReceiveByte(0xDF);
	CPLD_DisableCS();
	for (uint32_t p = 0; p < 50; p++)
		__asm("nop");

	return 1;
  #endif  // __CHITU_LV3__
#endif  // __CHITU_BOARD__
}
//==============================================================================



void		UVD_Sleep()
{
#ifdef __MKSDLP_BOARD__
	SSD_SleepIn();
#endif  // __MKSDLP_BOARD__
}
//==============================================================================



void		UVD_Wakeup()
{
#ifdef __MKSDLP_BOARD__
	SSD_SleepOut();
#endif  // __MKSDLP_BOARD__
}
//==============================================================================



void		UVD_Off()
{
#ifdef __MKSDLP_BOARD__
	MIPI_Off();
#endif  // __MKSDLP_BOARD__
}
//==============================================================================



void		UVD_On()
{
#ifdef __MKSDLP_BOARD__
	SSD_Init();
#endif  // __MKSDLP_BOARD__
}
//==============================================================================



