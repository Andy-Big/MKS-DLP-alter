#include "cpld_utils.h"
#include "fst_spi.h"
//#include "mks_dlp_main.h"



volatile uint32_t		reTransmission_zero_cnt = 0, reTransmission_data_cnt = 0, reTransmission_all_cnt = 0;
extern uint8_t			Line_Pixel[Y_RATIO + FILLCODE * 2];

CPLD_CMD_FRAME			cmd;
DLP_BMP					bmp;



uint8_t		_cpld_bank2disp_enable(uint8_t bank_used_id, uint8_t scan_en, uint8_t rd_sdram_en)
{
	cmd.mark = 0;
	cmd.data1 = 0;
	//cmd.data2 =  (bank_used_id<<4 | 0xC0)&0xF0;
	//cmd.data2 =  (bank_used_id<<4 | 0x80)&0xF0;		//Чтение SDRAM запрещено
	cmd.data2 = ((scan_en << 7) | (rd_sdram_en << 6) | (bank_used_id << 4)) & 0xF0;
	for(uint8_t i=0; i < MAX_RETRAN_TIMES; i++)
	{
		CPLD_CS_On();
		FLASH_SPIWriteReadByte(cmd.mark);
		FLASH_SPIWriteReadByte(cmd.data1);
		FLASH_SPIWriteReadByte(cmd.data2);
		CPLD_CS_Off();

		if(cmd.data2 == (_cpld_bank2disp_read() & 0xF0))
			return 1;
	}
	return 0;
}
//==============================================================================



uint8_t		_cpld_bank2disp_read()
{
	volatile uint8_t mark=0;
	volatile uint8_t data1=0;
	uint8_t data2=0;

	CPLD_CS_On();
	mark  = FLASH_SPIWriteReadByte(0x80);
	data1 = FLASH_SPIWriteReadByte(0xFF);
	data2 = FLASH_SPIWriteReadByte(0xFF);
	CPLD_CS_Off();

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
		endLine = X_RATIO;
	}

	memset(bmp.line.d_frame.data, 0, DATA_LEN);
	
	for(cur_line = startLine; cur_line < endLine; cur_line++)
	{
		
		bmp.line.d_frame.mark1 = 0x40 | (bank_used_id<<4) | ((cur_line >> 8)&0x0F);
		bmp.line.d_frame.mark2 =  cur_line & 0xFF;
		bmp.line.d_frame.CRC16 = _cpld_CRC16_XMODEM(&bmp.line.d_frame.mark1, DATA_CRC_LEN);

		bmp.line.crc_status = CPLD_Get_CRC();

		if(bmp.line.crc_status == 0)	//Решение о повторной передаче
		{
			_cpld_reTransmission();
			reTransmission_zero_cnt++;
		}

		CPLD_CS_On();
		memcpy(&bmp.line.d_frame_bakup[0], &bmp.line.d_frame.mark1, TXDATA_LEN);
		FLASH_SPIWriteBuffDMA(TXDATA_LEN, &bmp.line.d_frame_bakup[0]);
		CPLD_CS_Off();
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
		CPLD_CS_On();
		FLASH_SPIWriteBuffDMA(TXDATA_LEN, &bmp.line.d_frame_bakup[0]);
		CPLD_CS_Off();
		for(volatile uint8_t k=0; k<10; k++);	
		bmp.line.crc_status = CPLD_Get_CRC();
		if(bmp.line.crc_status == 1)
			return 1;
	}
	return 0;
}
//==============================================================================



void		_cpld_line_gen_data(uint16_t line, uint8_t bank_used_id)
{
	uint8_t		pixel_sum = 0;
	uint8_t		*p, *q;
	uint8_t		i, j;
	
	bmp.line.d_frame.mark1 = 0x40 | (bank_used_id<<4) | ((line >> 8)&0x0F);//tan ---напиши SDRAM
	bmp.line.d_frame.mark2 =  line&0xFF;
	memset(bmp.line.d_frame.data, 0, DATA_LEN);

	p = bmp.line.d_frame.data;
	q = Line_Pixel;         //tan ---Из среза - строка данных, которые были проанализированы (данные отображаются на экране 2k).
	for(i=0; i < DATA_LEN; i++) //tan ---Скопируйте данные из Line_Pixel в bmp.line.d_frame.data
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
	bmp.line.d_frame.CRC16 = _cpld_CRC16_XMODEM(&bmp.line.d_frame.mark1, DATA_CRC_LEN);

	bmp.line.crc_status = CPLD_Get_CRC();              //tan ---Проверьте штифт crc, проверка высокого уровня в норме.
	if(bmp.line.crc_status == 0)	//Решение о повторной передаче
	{
		_cpld_reTransmission();
		reTransmission_data_cnt++;
	}
	
	CPLD_CS_On();
	memcpy(&bmp.line.d_frame_bakup[0], &bmp.line.d_frame.mark1, TXDATA_LEN);
	FLASH_SPIWriteBuffDMA(TXDATA_LEN, &bmp.line.d_frame_bakup[0]);   //Начать передачу DMA
	CPLD_CS_Off();

}
//==============================================================================









void		CPLD_Init()
{
}
//==============================================================================



