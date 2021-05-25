#include "uvdisplay.h"
#include "fst_spi.h"
//#include "ssd2828.h"




uint8_t					Line_Pixel[CPLD_Y_RATIO + CPLD_FILLCODE * 2];












uint8_t		UVD_Init()
{
	CPLD_Init();
#ifdef __MKSDLP_BOARD__
	return SSD_Init();
#endif  // __MKSDLP_BOARD__
	return 1;
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



