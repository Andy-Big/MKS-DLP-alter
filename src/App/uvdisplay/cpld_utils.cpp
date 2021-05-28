#include "cpld_utils.h"
#include "fst_spi.h"
#include "gpio.h"



uint16_t		CPLD_X_RATIO = 2560;
uint16_t		CPLD_Y_RATIO = 1440;





#ifdef __MKSDLP_BOARD__


const unsigned short Crc16Table[256] = {
		0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
		0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
		0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
		0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
		0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
		0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
		0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
		0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
		0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
		0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
		0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
		0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
		0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
		0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
		0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
		0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
		0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
		0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
		0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
		0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
		0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
		0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
		0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
		0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
		0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
		0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
		0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
		0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
		0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
		0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
		0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
		0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};




volatile uint32_t		reTransmission_zero_cnt = 0, reTransmission_data_cnt = 0, reTransmission_all_cnt = 0;
extern uint8_t			Line_Pixel[3000];

CPLD_CMD_FRAME			cpld_cmd;
DLP_BMP					cpld_bmp;



uint8_t		_cpld_bank2disp_enable(uint8_t bank_used_id, uint8_t scan_en, uint8_t rd_sdram_en)
{
	cpld_cmd.mark = 0;
	cpld_cmd.data1 = 0;
	//cmd.data2 =  (bank_used_id<<4 | 0xC0)&0xF0;
	//cmd.data2 =  (bank_used_id<<4 | 0x80)&0xF0;		//Чтение SDRAM запрещено
	cpld_cmd.data2 = ((scan_en << 7) | (rd_sdram_en << 6) | (bank_used_id << 4)) & 0xF0;
	for(uint8_t i=0; i < MAX_RETRAN_TIMES; i++)
	{
		_cpld_CS_Enable();
		FLASH_SPIWriteReadByte(cpld_cmd.mark);
		FLASH_SPIWriteReadByte(cpld_cmd.data1);
		FLASH_SPIWriteReadByte(cpld_cmd.data2);
		_cpld_CS_Disable();

		if(cpld_cmd.data2 == (_cpld_bank2disp_read() & 0xF0))
			return 1;
	}
	return 0;
}
//==============================================================================



uint8_t		_cpld_bank2disp_read()
{
	volatile uint8_t mark = 0;
	volatile uint8_t data1 = 0;
	uint8_t data2=0;

	_cpld_CS_Enable();
	mark  = FLASH_SPIWriteReadByte(0x80);
	data1 = FLASH_SPIWriteReadByte(0xFF);
	data2 = FLASH_SPIWriteReadByte(0xFF);
	_cpld_CS_Disable();

	return data2;
}
//==============================================================================



uint8_t		_cpld_get_version()
{
	return((_cpld_bank2disp_read()&0x0F)%10);
}
//==============================================================================



inline uint16_t		_cpld_CRC16_XMODEM(unsigned char *puchMsg, unsigned int usDataLen)
{
	unsigned short wCRCin = 0x0000;
	unsigned short wCPoly = 0x1021;
	unsigned char wChar = 0;
	unsigned short wCTemp = 0;
	while (usDataLen--) 	
	{
		wChar = *(puchMsg++);
		wCRCin ^= (wChar << 8);
		for(int i = 0; i < 8; i++)
		{
			if(wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	wCTemp = (wCRCin << 8) & 0xFF00;
	wCTemp |= (wCRCin >> 8) & 0x00FF;
	return (wCTemp) ;
}
//==============================================================================



inline uint16_t		_cpld_CRC16_XMODEM_new(unsigned char *puchMsg, unsigned int usDataLen)
{
	unsigned short crc = 0;

	while (usDataLen--)
		crc = (crc << 8) ^ Crc16Table[(crc >> 8) ^ *puchMsg++];

	return crc;
}
//==============================================================================



void		_cpld_fill_lines_zero(char flag, uint16_t line, uint8_t bank_used_id)
{
	uint16_t	cur_line, startLine, endLine;
	
	if(flag == 0)
	{
		startLine = 0;
		endLine = line;
	}
	else
	{
		startLine = line;
		endLine = CPLD_X_RATIO;
	}

	memset(cpld_bmp.line.d_frame.data, 0, CPLD_DATA_LEN);
	
	for(cur_line = startLine; cur_line < endLine; cur_line++)
	{
		
		cpld_bmp.line.d_frame.mark1 = 0x40 | (bank_used_id<<4) | ((cur_line >> 8)&0x0F);
		cpld_bmp.line.d_frame.mark2 =  cur_line & 0xFF;
		cpld_bmp.line.d_frame.CRC16 = _cpld_CRC16_XMODEM(&cpld_bmp.line.d_frame.mark1, CPLD_DATA_CRC_LEN);

		cpld_bmp.line.crc_status = CPLD_Get_CRC();

		if(cpld_bmp.line.crc_status == 0)	//Решение о повторной передаче
		{
			_cpld_reTransmission();
			reTransmission_zero_cnt++;
		}

		_cpld_CS_Enable();
		memcpy(&cpld_bmp.line.d_frame_bakup[0], &cpld_bmp.line.d_frame.mark1, CPLD_TXDATA_LEN);
		FLASH_SPIWriteBuffDMA(CPLD_TXDATA_LEN, &cpld_bmp.line.d_frame_bakup[0]);
//		_cpld_CS_Disable();
		//hspi1.hdmatx.Instance.CR |= DMA_IT_TC | DMA_IT_TE | DMA_IT_DME;
//		*spi1_hdmatx_CR &= 0xFFFFFFF7;
	}
}
//==============================================================================



uint8_t		_cpld_reTransmission()
{
	for(uint8_t i=0; i < MAX_RETRAN_TIMES; i++)		
	{
		reTransmission_all_cnt++;
		_cpld_CS_Enable();
		FLASH_SPIWriteBuffDMA(CPLD_TXDATA_LEN, &cpld_bmp.line.d_frame_bakup[0]);
//		_cpld_CS_Disable();
		for(volatile uint8_t k=0; k<10; k++);	
		cpld_bmp.line.crc_status = CPLD_Get_CRC();
		if(cpld_bmp.line.crc_status == 1)
			return 1;
	}
	return 0;
}
//==============================================================================



void		_cpld_line_gen_data(uint16_t line, uint8_t bank_used_id)
{
	uint8_t				*p, *q;
	uint8_t				i, j;
	
	cpld_bmp.line.d_frame.mark1 = 0x40 | (bank_used_id<<4) | ((line >> 8)&0x0F);//tan ---напиши SDRAM
	cpld_bmp.line.d_frame.mark2 =  line&0xFF;
	memset(cpld_bmp.line.d_frame.data, 0, CPLD_DATA_LEN);

	p = cpld_bmp.line.d_frame.data;
	q = Line_Pixel;         //tan ---Из среза - строка данных, которые были проанализированы (данные отображаются на экране 2k).
	for(i=0; i < CPLD_DATA_LEN; i++) //tan ---Скопируйте данные из Line_Pixel в bmp.line.d_frame.data
	{
		for(j = 0; j < 8; j++)
		{
			*p <<= 1;		//tan ---bmp.line.d_frame.data сдвигается влево на один бит и зацикливается 8 раз для преобразования данных Line_Pixel []
/*
			if(*q++ == 1)	//tan ---Сдвиньте bmp.line.d_frame.data влево на одно место, выполните цикл 8 раз, преобразуйте данные Line_Pixel [], преобразуйте Line_Pixel побайтно в побитно и поместите его в bmp.line.d_frame.data
			{
				*p |= 1;
				pixel_sum++;
			}
*/
			*p |= *q++;
		}
		p++;
	}
	cpld_bmp.line.d_frame.CRC16 = __REVSH(_cpld_CRC16_XMODEM_new(&cpld_bmp.line.d_frame.mark1, CPLD_DATA_CRC_LEN));
//	cpld_bmp.line.d_frame.CRC16 = _cpld_CRC16_XMODEM(&cpld_bmp.line.d_frame.mark1, CPLD_DATA_CRC_LEN);

	cpld_bmp.line.crc_status = CPLD_Get_CRC();              //tan ---Проверьте штифт crc, проверка высокого уровня в норме.
	if(cpld_bmp.line.crc_status == 0)	//Решение о повторной передаче
	{
		_cpld_reTransmission();
		reTransmission_data_cnt++;
	}
	
	while (!FLASH_IsDMAReady());
	_cpld_CS_Enable();
	memcpy(&cpld_bmp.line.d_frame_bakup[0], &cpld_bmp.line.d_frame.mark1, CPLD_TXDATA_LEN);
	FLASH_SPIWriteBuffDMA(CPLD_TXDATA_LEN, &cpld_bmp.line.d_frame_bakup[0]);   //Начать передачу DMA
//	_cpld_CS_Disable();

}
//==============================================================================









void		CPLD_Init()
{
	CPLD_X_RATIO = 2560;
	CPLD_Y_RATIO = 1440;
}
//==============================================================================



#endif  // __MKSDLP_BOARD__



#ifdef __CHITU_BOARD__


void		CPLD_Init()
{
	CPLD_X_RATIO = 2560;
	CPLD_Y_RATIO = 1620;

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = CPLD_CS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(CPLD_CS_GPIO, &GPIO_InitStruct);
	CPLD_CS_DISABLE();

	GPIO_InitStruct.Pin = CPLD_CLK_Pin;
	HAL_GPIO_Init(CPLD_CLK_GPIO, &GPIO_InitStruct);
	CPLD_CLK_SET();
	
	GPIO_InitStruct.Pin = CPLD_MOSI_Pin;
	HAL_GPIO_Init(CPLD_MOSI_GPIO, &GPIO_InitStruct);
	CPLD_MOSI_RESET();

	GPIO_InitStruct.Pin = CPLD_MISO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(CPLD_MISO_GPIO, &GPIO_InitStruct);

	
	
}
//==============================================================================



void		CPLD_Deinit()
{
	HAL_GPIO_DeInit(CPLD_CS_GPIO, CPLD_CS_Pin);
	HAL_GPIO_DeInit(CPLD_CLK_GPIO, CPLD_CLK_Pin);
	HAL_GPIO_DeInit(CPLD_MOSI_GPIO, CPLD_MOSI_Pin);
	HAL_GPIO_DeInit(CPLD_MISO_GPIO, CPLD_MISO_Pin);
}
//==============================================================================



void		CPLD_EnableCS()
{
	CPLD_CS_ENABLE();
}
//==============================================================================



void		CPLD_DisableCS()
{
	CPLD_CS_DISABLE();
}
//==============================================================================



uint8_t	CPLD_SendReceiveByte(uint8_t val)
{
	uint8_t		retval = 0;
	for (uint32_t i = 0; i < 8; i++)
	{
		retval <<= 1;
		CPLD_CLK_RESET();
		if (val & 0x80)
			CPLD_MOSI_SET();
		else
			CPLD_MOSI_RESET();
		for (uint32_t p = 0; p < 8; p++)
			__asm("nop");
		retval |= CPLD_MISO_READ();
		val <<= 1;
		CPLD_CLK_SET();
		for (uint32_t p = 0; p < 8; p++)
			__asm("nop");
	}
	return retval;
}
//==============================================================================











#endif  // __CHITU_BOARD__
