#include "uvdisplay.h"
#include "fst_spi.h"
//#include "ssd2828.h"




uint8_t					Line_Pixel[Y_RATIO + FILLCODE * 2];












void		UVD_Init()
{
	CPLD_Init();
	SSD_Init();
//	ssd.init();
}
//==============================================================================



void		UVD_Sleep()
{
	SSD_SleepIn();
//	ssd.sleep_in();
}
//==============================================================================



void		UVD_Wakeup()
{
	SSD_SleepOut();
//	ssd.sleep_out();
}
//==============================================================================



void		UVD_ExposSetCircle()
{
	int i;
	uint8_t *p;

	_cpld_bank2disp_enable(TEST_USED_BANK, 0, 0);
	_cpld_fill_lines_zero(0,exposure_bmp_data_circle[0][2],TEST_USED_BANK); 
//	memset(&bmp_layer_buf[0],0,DATA_LEN_TFT*X_RATIO_TFT);	//TFT1
	
	for(i=0; i < CIRCLE_BMP_DATA_ROW; i++)
	{
		memset(Line_Pixel,0,sizeof(Line_Pixel));
		p = &Line_Pixel[exposure_bmp_data_circle[i][0]];
		for(int j = exposure_bmp_data_circle[i][0]; j < exposure_bmp_data_circle[i][1] + 1; j++)	
		{
			if(j < Y_RATIO / 2)
			{
				*p = 1;
			}
			else
			{
				*(p+FILLCODE) = 1;
			}
			p++;
		}
		_cpld_line_gen_data(i + exposure_bmp_data_circle[0][2], TEST_USED_BANK);
//		line_gen_data_TFT(i + exposure_bmp_data_circle[0][2]);	//TFT2
	}

	_cpld_fill_lines_zero(1, exposure_bmp_data_circle[CIRCLE_BMP_DATA_ROW-1][2]+1, TEST_USED_BANK);	//后面行填充"0"

	_cpld_bank2disp_enable(TEST_USED_BANK, 1, 1);
}
//==============================================================================



void		UVD_ExposSetSquare()
{
	int i;
	uint8_t *p;

	_cpld_bank2disp_enable(WORK_USED_BANK, 0, 0);
	_cpld_fill_lines_zero(0, exposure_bmp_data_square[0][2], WORK_USED_BANK); 
//	memset(&bmp_layer_buf[0],0,DATA_LEN_TFT*X_RATIO_TFT);	//TFT1
	
	for(i=0; i < SQUARE_BMP_DATA_ROW; i++)
	{
		memset(Line_Pixel,0,sizeof(Line_Pixel));
		p = &Line_Pixel[exposure_bmp_data_square[i][0]];
		for(int j = exposure_bmp_data_square[i][0]; j < exposure_bmp_data_square[i][1] + 1; j++)	
		{
			if(j < Y_RATIO / 2)
			{
				*p = 1;
			}
			else
			{
				*(p+FILLCODE) = 1;
			}
			p++;
		}
		_cpld_line_gen_data(i + exposure_bmp_data_square[0][2], WORK_USED_BANK);
//		line_gen_data_TFT(i + exposure_bmp_data_square[0][2]);	//TFT2
	}

	_cpld_fill_lines_zero(1, exposure_bmp_data_square[SQUARE_BMP_DATA_ROW-1][2]+1, WORK_USED_BANK);	//后面行填充"0"

	_cpld_bank2disp_enable(WORK_USED_BANK, 1, 1);
}
//==============================================================================



