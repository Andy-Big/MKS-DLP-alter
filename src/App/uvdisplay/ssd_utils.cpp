#include "ssd_utils.h"
#include "cpld_utils.h"
#include "fst_spi.h"





uint16_t		_ssd_read_data(uint8_t DEV, uint8_t reg)
{
	uint16_t data = 0;	
	uint8_t data_L = 0;
	uint8_t data_H = 0;
	if(DEV == SSD_DEV1)
		MIPI_SsdA_Cs_On();
	else
		if(DEV == SSD_DEV2)
			MIPI_SsdB_Cs_On();
	
	MIPI_SDC_On();
	FLASH_SPIWriteReadByte(reg);//Зарегистрируйте адрес для чтения	    
	FLASH_SPIWriteReadByte(SSD_RRA); 	
	MIPI_SDC_Off();

	data_L = FLASH_SPIWriteReadByte(0xFF);
	data_H = FLASH_SPIWriteReadByte(0xFF);
	data|=data_H<<8;	 
	data|=data_L;

	if(DEV == SSD_DEV1)
		MIPI_SsdA_Cs_Off();
	else
		if(DEV == SSD_DEV2)
			MIPI_SsdB_Cs_Off();

	return data;

}
//==============================================================================



uint8_t		_ssd_read_byte(uint8_t DEV, uint8_t reg)
{
	uint8_t data = 0;	

	if(DEV == SSD_DEV1)
		MIPI_SsdA_Cs_On();
	else
		if(DEV == SSD_DEV2)
			MIPI_SsdB_Cs_On();
	
	MIPI_SDC_On();
	FLASH_SPIWriteReadByte(reg);//Зарегистрируйте адрес для чтения    
	FLASH_SPIWriteReadByte(SSD_RRA); 	
	MIPI_SDC_Off();

	data = FLASH_SPIWriteReadByte(0xFF);

	if(DEV == SSD_DEV1)
		MIPI_SsdA_Cs_Off();
	else
		if(DEV == SSD_DEV2)
			MIPI_SsdB_Cs_Off();

	return data;
}
//==============================================================================



void		_ssd_write_data(uint8_t DEV, uint8_t reg, uint16_t data)
{
	uint8_t data_L = 0;
	uint8_t data_H = 0;

	data_L = data&0xff;
	data_H = (data>>8)&0xff;
	
	if(DEV == SSD_DEV1)
		MIPI_SsdA_Cs_On();
	else
		if(DEV == SSD_DEV2)
			MIPI_SsdB_Cs_On();
	
	MIPI_SDC_On();
	FLASH_SPIWriteReadByte(reg);	//Зарегистрируйте адрес для записи
	MIPI_SDC_Off();

	FLASH_SPIWriteReadByte(data_L);
	FLASH_SPIWriteReadByte(data_H);

	if(DEV == SSD_DEV1)
		MIPI_SsdA_Cs_Off();
	else
		if(DEV == SSD_DEV2)
			MIPI_SsdB_Cs_Off();
/*
	uint16_t data_check = 0;
	data_check = _ssd_read_data(DEV,reg);
	printf("0x%02x: write=0x%04x, read=0x%04x",reg,data,data_check);
	if(data_check == data)
		printf("\n");
	else
		printf("		*\n");
*/
}
//==============================================================================



void		_ssd_write_byte(uint8_t DEV, uint8_t reg, uint8_t data)
{
	
	if(DEV == SSD_DEV1)
		MIPI_SsdA_Cs_On();
	else
		if(DEV == SSD_DEV2)
			MIPI_SsdB_Cs_On();
	
	MIPI_SDC_On();
	FLASH_SPIWriteReadByte(reg);	//Зарегистрируйте адрес для записи
	MIPI_SDC_Off();

	FLASH_SPIWriteReadByte(data);

	if(DEV == SSD_DEV1)
		MIPI_SsdA_Cs_Off();
	else
		if(DEV == SSD_DEV2)
			MIPI_SsdB_Cs_Off();
}
//==============================================================================



uint16_t	_ssd_read_id(uint8_t DEV)
{
	return _ssd_read_data(DEV, SSD_DIR);
}
//==============================================================================



void		_ssd_cfg(uint8_t DEV)
{
	_ssd_write_data(DEV, 0xB1, 0x010A);	
	_ssd_write_data(DEV, 0xB2, 0x042E);
	_ssd_write_data(DEV, 0xB3, 0x0464);
	_ssd_write_data(DEV, 0xB4, 0x02D0);
	_ssd_write_data(DEV, 0xB5, 0x0A00);
	_ssd_write_data(DEV, 0xB6, 0x0007);
	_ssd_write_data(DEV, 0xB8, 0x0000);
	_ssd_write_data(DEV, 0xB9, 0x0000);
	HAL_Delay(50);
	/*--------------PLL Configuration----------------*/
#if defined(PLL_384M)
	_ssd_write_data(DEV, 0xBA, 0x8120);	//PCLK = 50M  PLL = 384
	_ssd_write_data(DEV, 0xBB, 0x0007);	//LP = 6.0M   
#elif defined(PLL_768M)
	_ssd_write_data(DEV, 0xBA, 0xC140);	//PCLK = 100M	Fin=12M PLL=768M
	_ssd_write_data(DEV, 0xBB, 0x000F);	//LP = 6.0M
#endif
	_ssd_write_data(DEV, 0xD5, 0x1860);
	/*--------------PLL Configuration----------------*/
#if defined(PLL_384M)
	_ssd_write_data(DEV, 0xC9, 0x1501);
	_ssd_write_data(DEV, 0xCA, 0x1E01);
	_ssd_write_data(DEV, 0xCB, 0x0511);	//0x0F2F
	_ssd_write_data(DEV, 0xCC, 0x0708);
#elif defined(PLL_768M)
	_ssd_write_data(DEV, 0xC9, 0x2A06);
	_ssd_write_data(DEV, 0xCA, 0x3D06);
	_ssd_write_data(DEV, 0xCB, 0x0C24);	//0x0F2F
	_ssd_write_data(DEV, 0xCC, 0x0E0E);
#endif

	_ssd_write_data(DEV, 0xDE, 0x0003);
	_ssd_write_data(DEV, 0xB9, 0x0001);
	_ssd_write_data(DEV, 0xD6, 0x0005);
	HAL_Delay(50);

}
//==============================================================================



void		_ssd_ls055r1sx04_cfg(uint8_t DEV)		
{
	_ssd_write_Generic(DEV, 0xB0, 0x00);
	_ssd_write_Generic(DEV, 0xD6, 0x01);
	_ssd_write_Generic(DEV, 0xB3, 0x18);
	_ssd_write_DCS(DEV, 0X51, 0xFF, 2);
	_ssd_write_DCS(DEV, 0X53, 0x0C, 2);
	_ssd_write_DCS(DEV, 0X35, 0x00, 2);
	HAL_Delay(20);	//If customer need, please add initial command in here.
	_ssd_write_Generic(DEV, 0xB0, 0x03);
	HAL_Delay(20);	
	_ssd_write_DCS(DEV, 0x29, 0x00, 1);
	HAL_Delay(20);	
	_ssd_write_DCS(DEV, 0x11, 0x00, 1);
	HAL_Delay(20);	

	_ssd_write_data(DEV, 0xDE, 0x0003);
	_ssd_write_data(DEV, 0xB8, 0x0000);
	_ssd_write_data(DEV, 0xB7, 0x030B);	//Режим HS, частота ФАПЧ определяется tx_clk, разрешение видеорежима
	HAL_Delay(20);	//*500
}
//==============================================================================



void		_ssd_write_DCS_sleep(uint8_t DEV, uint8_t addr)
{
	_ssd_write_data(DEV, 0xB7, 0x034B);
	HAL_Delay(5  *SLEEP_DELAY);
	_ssd_write_data(DEV, 0xB8, 0x0000);
	HAL_Delay(5 * SLEEP_DELAY);
	_ssd_write_data(DEV, 0xBC, 0x0001);
	HAL_Delay(5 * SLEEP_DELAY);   
	_ssd_write_byte(DEV, 0xBF, addr);
	HAL_Delay(5 * SLEEP_DELAY);
}
//==============================================================================



void		_ssd_write_Generic(uint8_t DEV,uint8_t addr,uint8_t data)	
{
	_ssd_write_data(DEV, 0xB7, 0x0302);	//LP generic write
	_ssd_write_data(DEV, 0xB8, 0x0000);
	_ssd_write_data(DEV, 0xBC, 0x0002);   //Записываемый байт длины пакета	
	_ssd_write_data(DEV, 0xBF, (data<<8)|addr);
}
//==============================================================================



void		_ssd_write_DCS(uint8_t DEV,uint8_t addr,uint8_t data,uint8_t size)
{
	_ssd_write_data(DEV, 0xB7, 0x0342);	//LP DCS write
	_ssd_write_data(DEV, 0xB8, 0x0000);
	_ssd_write_data(DEV, 0xBC, size);   //Записываемый байт длины пакета
	if(size == 2)
		_ssd_write_data(DEV, 0xBF, (data<<8)|addr);   													
	else
		_ssd_write_byte(DEV, 0xBF, addr);	
}
//==============================================================================








uint8_t		SSD_Init()
{
	MIPI_Off();
	MIPI_On();

	if(_ssd_read_id(SSD_DEV1) != SSD2828_ID)
	{
		return 0;
	}
	if(_ssd_read_id(SSD_DEV2) != SSD2828_ID)
	{
		return 0;
	}

	FLASH_SPISetSpeed(SPI_BAUDRATEPRESCALER_128);	

	_cpld_bank2disp_enable(WORK_USED_BANK,1,0);
	HAL_Delay(50);

	_ssd_cfg(SSD_DEV1);
	HAL_Delay(200);
	_ssd_ls055r1sx04_cfg(SSD_DEV1);
	HAL_Delay(200);

	_ssd_cfg(SSD_DEV2);
	HAL_Delay(100);
	_ssd_ls055r1sx04_cfg(SSD_DEV2);
	HAL_Delay(100);

	SSD_SleepIn();
	HAL_Delay(10);
	_cpld_bank2disp_enable(WORK_USED_BANK, 0, 0);
	HAL_Delay(20);

	FLASH_SPISetSpeed(SPI_BAUDRATEPRESCALER_8);	

	return 1;
}
//==============================================================================



void		SSD_SleepIn()
{
	HAL_Delay(2*SLEEP_DELAY);
	_ssd_write_DCS_sleep(SSD_DEV1, 0x28);
	HAL_Delay(5*SLEEP_DELAY);	//dev1 panel Display Off
	_ssd_write_DCS_sleep(SSD_DEV1, 0x10);
	HAL_Delay(20*SLEEP_DELAY);	//dev1 panel Sleep In

	_ssd_write_DCS_sleep(SSD_DEV2, 0x28);
	HAL_Delay(5*SLEEP_DELAY);	//dev2 panel Display Off
	_ssd_write_DCS_sleep(SSD_DEV2, 0x10);
	HAL_Delay(20*SLEEP_DELAY);	//dev2 panel Sleep In

	_ssd_write_data(SSD_DEV1, 0xB7, 0x0004);	
	_ssd_write_data(SSD_DEV2, 0xB7, 0x0004);	
}
//==============================================================================



void		SSD_SleepOut()
{
	HAL_Delay(2*SLEEP_DELAY);
	_ssd_write_DCS_sleep(SSD_DEV1, 0x29);		//dev1 panel Display On
	_ssd_write_DCS_sleep(SSD_DEV1, 0x11); 	//dev1 panel Sleep Out

	_ssd_write_DCS_sleep(SSD_DEV2, 0x29);		//dev2 panel Display On
	_ssd_write_DCS_sleep(SSD_DEV2, 0x11);		//dev2 panel Sleep Out

	_ssd_write_data(SSD_DEV1, 0xB7, 0x030B);	//dev1 Sleep mode is disabled
	_ssd_write_data(SSD_DEV2, 0xB7, 0x030B);	//dev2 Sleep mode is disabled
}
//==============================================================================



void		MIPI_Off()
{
/*
	CPLD_RST_On();
	HAL_Delay(5);
*/
/*
	MIPI_Ce_Off();
	HAL_Delay(50);
	MIPI_PwrEn_Off();
	HAL_Delay(50);
*/
	//original
	CPLD_RST_On();
	HAL_Delay(5);
	MIPI_PwrEn_Off();
	HAL_Delay(200);
	MIPI_Ce_Off();
	HAL_Delay(100);
}
//==============================================================================



void		MIPI_On()
{
/*
	HAL_Delay(50);
	MIPI_Ce_On();
	HAL_Delay(50);
	MIPI_PwrEn_On();
	HAL_Delay(50);
	CPLD_RST_Off();
	HAL_Delay(50);
*/
/*
	HAL_Delay(50);
	MIPI_PwrEn_On();
	HAL_Delay(50);
	CPLD_RST_On();
	HAL_Delay(50);
	CPLD_RST_Off();
	HAL_Delay(50);
	MIPI_Ce_On();
	HAL_Delay(50);
*/

	// original
	CPLD_RST_On();
 
	HAL_Delay(200);
	MIPI_Ce_On();
	HAL_Delay(200);
	MIPI_PwrEn_On();
	HAL_Delay(50);
	CPLD_RST_Off();
	HAL_Delay(100);
}
//==============================================================================



