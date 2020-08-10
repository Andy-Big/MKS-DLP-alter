/******************** (C) COPYRIGHT 2018 www.makerbase.com ********************
* File Name          : ssd2828.c
* Author             : www.makerbase.com
* Version            : V1.0
* Library            : Using STM32F2XX_STDPERIPH_VERSION V3.3.0
* Date               : 07/25/2018
* Description        : This file provides a set of functions needed to manage the
*                      communication between SPI peripheral.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
//#include "spi.h"
#include "ssd2828.h"
//#include "mks_dlp_main.h"
#include "fatfs.h"
#include "cpld_utils.h"
#include "unicode_utils.h"

#include <stdio.h>

//#define SSD_CFG_CHECK_ENA				1


#define SPIn_ReadWriteByte      SPI1_ReadWriteByte
#define SPIn 			SPI1
#define hspin			hspi1
#define hdma_spin_rx	hdma_spi1_rx

extern DMA_HandleTypeDef hdma_spin_rx;

#if defined(SSD_CFG_CHECK_ENA)	
char *ssd_cfg_check_file = "ssd_chk_1.txt";
#endif




/*
#define SPI_I2S_FLAG_RXNE               ((uint16_t)0x0001)
#define SPI_I2S_FLAG_TXE                ((uint16_t)0x0002)
#define I2S_FLAG_CHSIDE                 ((uint16_t)0x0004)
#define I2S_FLAG_UDR                    ((uint16_t)0x0008)
#define SPI_I2S_FLAG_OVR                ((uint16_t)0x0040)
#define SPI_I2S_FLAG_BSY                ((uint16_t)0x0080)
#define SPI_I2S_FLAG_TIFRFE             ((uint16_t)0x0100)

uint8_t SPI_I2S_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG)
{
  uint8_t bitstatus = 0;
  // Check the parameters
  assert_param(IS_SPI_ALL_PERIPH_EXT(SPIx));
  assert_param(IS_SPI_I2S_GET_FLAG(SPI_I2S_FLAG));
  
  // Check the status of the specified SPI flag
  if ((SPIx->SR & SPI_I2S_FLAG) != (uint16_t)0)
  {
    // SPI_I2S_FLAG is set
    bitstatus = 1;
  }
  else
  {
    // SPI_I2S_FLAG is reset
    bitstatus = 0;
  }
  // Return the SPI_I2S_FLAG status
  return  bitstatus;
}
void SPI_I2S_SendData(SPI_TypeDef* SPIx, uint16_t Data)
{
  // Check the parameters
  assert_param(IS_SPI_ALL_PERIPH_EXT(SPIx));
  
  // Write in the DR register the data to be sent
  SPIx->DR = Data;
}
uint16_t SPI_I2S_ReceiveData(SPI_TypeDef* SPIx)
{
  // Check the parameters
  assert_param(IS_SPI_ALL_PERIPH_EXT(SPIx));
  
  // Return the data in the DR register
  return SPIx->DR;
}
void SPI_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState)
{
  // Check the parameters
  assert_param(IS_SPI_ALL_PERIPH(SPIx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    // Enable the selected SPI peripheral
    SPIx->CR1 |= SPI_CR1_SPE;
  }
  else
  {
    // Disable the selected SPI peripheral
    SPIx->CR1 &= (uint16_t)~((uint16_t)SPI_CR1_SPE);
  }
}
void SPI1_SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
	SPI1->CR1&=0XFFC7;// Bits 3-5 are cleared to set the baud rate
	SPI1->CR1|=SPI_BaudRatePrescaler;	// Set SPI1 speed
	SPI_Cmd(SPI1,ENABLE); // enable SPI1
} 
uint8_t SPI1_ReadWriteByte(uint8_t TxData)
{		 			 
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}//µÈ´ý·¢ËÍÇø¿Õ  
	SPI_I2S_SendData(SPI1, TxData); //Í¨¹ýÍâÉèSPIx·¢ËÍÒ»¸öbyte  Êý¾Ý
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){} //µÈ´ý½ÓÊÕÍêÒ»¸öbyte  
	return SPI_I2S_ReceiveData(SPI1); //·µ»ØÍ¨¹ýSPIx×î½ü½ÓÊÕµÄÊý¾Ý	
}
int SPI1_ReadWrite2Bytes(void)  
{ 
	volatile uint16_t ans=0;
	uint16_t temp = 0;
	temp=SPI1_ReadWriteByte(0x00);
	ans=temp<<8;
	temp=SPI1_ReadWriteByte(0x00);
	ans|=temp;
	ans>>=3;
	return ans&0x0fff;

} 

*/









#if defined(SSD_CFG_CHECK_ENA)
FIL		ena_file;
bool open_file()
{
	TCHAR name[30]={0};
	
	tstrcat(name, UsbPath);

	tstrcat_utf(name, ssd_cfg_check_file);

	
	if (f_open(&ena_file, (const TCHAR *)name, FA_CREATE_ALWAYS | FA_READ | FA_WRITE) == FR_OK)		
        {
		return true;
        }
	else
		return false;
}
void close_file()
{
	f_close(&ena_file);
}
#endif



SSD2828::SSD2828() 
{

}

/*******************************************************************************
* Function Name  : read_data
* Description    : Прочтите данные регистра SSD2828
* Input          : - reg : Зарегистрируйте адрес
*                  - DEV : Выбрать чип
* Output         : None
* Return         : data	 : Вернуть прочитанное значение
*******************************************************************************/
uint16_t SSD2828::read_data(uint8_t DEV, uint8_t reg)
{
	uint16_t data = 0;	
	uint8_t data_L = 0;
	uint8_t data_H = 0;
	if(DEV == SSD_DEV1)			MIPI_SsdA_Cs_On();
	else if(DEV == SSD_DEV2)	MIPI_SsdB_Cs_On();
	
	MIPI_SDC_On();
	FLASH_SPIWriteReadByte(reg);//Зарегистрируйте адрес для чтения	    
	FLASH_SPIWriteReadByte(SSD_RRA); 	
	MIPI_SDC_Off();

	data_L = FLASH_SPIWriteReadByte(Dummy_Byte);
	data_H = FLASH_SPIWriteReadByte(Dummy_Byte);
	data|=data_H<<8;	 
	data|=data_L;
	if(DEV == SSD_DEV1)			MIPI_SsdA_Cs_Off();
	else if(DEV == SSD_DEV2)	MIPI_SsdB_Cs_Off();

	return data;

}
uint8_t SSD2828::read_byte(uint8_t DEV,uint8_t reg)
{
	uint8_t data = 0;	
	if(DEV == SSD_DEV1)			MIPI_SsdA_Cs_On();
	else if(DEV == SSD_DEV2)	MIPI_SsdB_Cs_On();
	
	MIPI_SDC_On();
	FLASH_SPIWriteReadByte(reg);//Зарегистрируйте адрес для чтения    
	FLASH_SPIWriteReadByte(SSD_RRA); 	
	MIPI_SDC_Off();

	data = FLASH_SPIWriteReadByte(Dummy_Byte);

	if(DEV == SSD_DEV1)			MIPI_SsdA_Cs_Off();
	else if(DEV == SSD_DEV2)	MIPI_SsdB_Cs_Off();

	return data;

}

/*******************************************************************************
* Function Name  : write_data
* Description    : Записать регистр SSD2828
* Input          : - reg : Зарегистрируйте адрес
*                  - DEV : Выбрать чип
*                  - data : Данные для записи
* Output         : None
* Return         : None
*******************************************************************************/
void SSD2828::write_data(uint8_t DEV,uint8_t reg,uint16_t data)
{
	uint8_t data_L = 0;
	uint8_t data_H = 0;

	data_L = data&0xff;
	data_H = (data>>8)&0xff;
	
	if(DEV == SSD_DEV1)			MIPI_SsdA_Cs_On();
	else if(DEV == SSD_DEV2)	MIPI_SsdB_Cs_On();
	
	MIPI_SDC_On();
	FLASH_SPIWriteReadByte(reg);	//Зарегистрируйте адрес для записи
	MIPI_SDC_Off();

	FLASH_SPIWriteReadByte(data_L);
	FLASH_SPIWriteReadByte(data_H);

	if(DEV == SSD_DEV1)			MIPI_SsdA_Cs_Off();
	else if(DEV == SSD_DEV2)	MIPI_SsdB_Cs_Off();

#if defined(SSD_CFG_CHECK_ENA)
	uint16_t data_check = 0;
	data_check = read_data(DEV,reg);
	char cmsg[256];
	sprintf(cmsg, "0x%02x: write=0x%04x, read=0x%04x", reg, data, data_check);
	if(data_check == data)
		strcat(cmsg,"\n");
	else
		strcat(cmsg,"		*\n");
	f_write(&ena_file, cmsg, strlen(cmsg), NULL);
#endif	

}

void SSD2828::write_byte(uint8_t DEV,uint8_t reg,uint8_t data)
{
	
	if(DEV == SSD_DEV1)			MIPI_SsdA_Cs_On();
	else if(DEV == SSD_DEV2)	MIPI_SsdB_Cs_On();
	
	MIPI_SDC_On();
	FLASH_SPIWriteReadByte(reg);	//Зарегистрируйте адрес для записи
	MIPI_SDC_Off();

	FLASH_SPIWriteReadByte(data);

	if(DEV == SSD_DEV1)			MIPI_SsdA_Cs_Off();
	else if(DEV == SSD_DEV2)	MIPI_SsdB_Cs_Off();
	
#if defined(SSD_CFG_CHECK_ENA)
	uint8_t data_check = 0;
	data_check = read_byte(DEV,reg);
	char cmsg[256];
	sprintf(cmsg, "0x%02x: write=0x%02x, read=0x%02x",reg,data,data_check);
	if(data_check == data)
		strcat(cmsg,"\n");
	else
		strcat(cmsg,"		*\n");
	f_write(&ena_file, cmsg, strlen(cmsg), NULL);
#endif		

}

/*******************************************************************************
* Function Name  : read_id
* Description    : Чтение идентификатора микросхемы SSD2828
* Input          : - DEV : 选择芯片
* Output         : None
* Return         : ID值
*******************************************************************************/
uint16_t SSD2828::read_id(uint8_t DEV)
{
	return read_data(DEV,SSD_DIR);
}

/*******************************************************************************
* Function Name  : reset
* Description    : Сброс микросхемы SSD2828
* Input          : None
* Output         : Выходной сигнал сброса низкого импульса 500us
* Return         : None
* 
*******************************************************************************/
void SSD2828::reset()
{
	 CPLD_RST_On();
 
	 HAL_Delay(200);
	 MIPI_Ce_On();
	 HAL_Delay(200);
	 MIPI_PwrEn_On();
	 HAL_Delay(50);
	 CPLD_RST_Off();
	 HAL_Delay(100);
}

/*******************************************************************************
* Function Name  : configure
* Description    : Настроить режим работы SSD2828
* Input          : - DEV : 选择芯片 
*                  - seq : Серийный номер конфигурации
* Output         : None
* Return         : None
*******************************************************************************/


void SSD2828::ssd_cfg(uint8_t DEV)
{
		write_data(DEV,0xB1,0x010A);	
		write_data(DEV,0xB2,0x042E);
		write_data(DEV,0xB3,0x0464);
		write_data(DEV,0xB4,0x02D0);
		write_data(DEV,0xB5,0x0A00);
		write_data(DEV,0xB6,0x0007);
		write_data(DEV,0xB8,0x0000);
		write_data(DEV,0xB9,0x0000);
		HAL_Delay(100);
		/*--------------PLL Configuration----------------*/
#if defined(PLL_384M)
				write_data(DEV,0xBA,0x8120);	//PCLK = 50M  PLL = 384
				write_data(DEV,0xBB,0x0007);	//LP = 6.0M   
#elif defined(PLL_768M)
			write_data(DEV,0xBA,0xC140);	//PCLK = 100M	Fin=12M PLL=768M
			write_data(DEV,0xBB,0x000F);	//LP = 6.0M
#endif
			write_data(DEV,0xD5,0x1860);
		/*--------------PLL Configuration----------------*/
#if defined(PCLK_RESET_M)		
		write_data(DEV,0xC9,0x3609);
		write_data(DEV,0xCA,0x4F09);
		write_data(DEV,0xCB,0x0A31);	//0x0F2F
		write_data(DEV,0xCC,0x1211);
#elif defined(PLL_384M)
			write_data(DEV,0xC9,0x1501);
			write_data(DEV,0xCA,0x1E01);
			write_data(DEV,0xCB,0x0511);	//0x0F2F
			write_data(DEV,0xCC,0x0708);
#elif defined(PLL_768M)
			write_data(DEV,0xC9,0x2A06);
			write_data(DEV,0xCA,0x3D06);
			write_data(DEV,0xCB,0x0C24);	//0x0F2F
			write_data(DEV,0xCC,0x0E0E);
#endif

		write_data(DEV,0xDE,0x0003);
		write_data(DEV,0xB9,0x0001);
		write_data(DEV,0xD6,0x0005);
		HAL_Delay(100);
	
}

void SSD2828::write_DCS_sleep(uint8_t DEV,uint8_t addr)
{
	write_data(DEV,0xB7,0x034B);HAL_Delay(5*SLEEP_DELAY);
	write_data(DEV,0xB8,0x0000);HAL_Delay(5*SLEEP_DELAY);
	write_data(DEV,0xBC,0x0001);HAL_Delay(5*SLEEP_DELAY);   
	write_byte(DEV,0xBF,addr);	HAL_Delay(5*SLEEP_DELAY);
}

void SSD2828::sleep_in()
{
	HAL_Delay(2*SLEEP_DELAY);
	write_DCS_sleep(SSD_DEV1,0x28);	HAL_Delay(5*SLEEP_DELAY);	//dev1 panel Display Off
	write_DCS_sleep(SSD_DEV1,0x10);	HAL_Delay(20*SLEEP_DELAY);	//dev1 panel Sleep In

	write_DCS_sleep(SSD_DEV2,0x28);	HAL_Delay(5*SLEEP_DELAY);	//dev2 panel Display Off
	write_DCS_sleep(SSD_DEV2,0x10);	HAL_Delay(20*SLEEP_DELAY);	//dev2 panel Sleep In

	write_data(SSD_DEV1,0xB7,0x0004);	
	write_data(SSD_DEV2,0xB7,0x0004);	
}
void SSD2828::sleep_out()
{
	HAL_Delay(2*SLEEP_DELAY);
	write_DCS_sleep(SSD_DEV1,0x29);		//dev1 panel Display On
	write_DCS_sleep(SSD_DEV1,0x11); 	//dev1 panel Sleep Out

	write_DCS_sleep(SSD_DEV2,0x29);		//dev2 panel Display On
	write_DCS_sleep(SSD_DEV2,0x11);		//dev2 panel Sleep Out

	write_data(SSD_DEV1,0xB7,0x030B);	//dev1 Sleep mode is disabled
	write_data(SSD_DEV2,0xB7,0x030B);	//dev2 Sleep mode is disabled
}

volatile uint32_t vbist_test_cnt = 0;
volatile uint32_t mipi_te_cnt = 0;

//#define SLEEP_USE_BIST
#define SLEEP_USE_TEST

void SSD2828:: sleep_test()
{
	while(1)
		{
		vbist_test_cnt++;

#if defined(SLEEP_USE_BIST)		
		write_data(SSD_DEV1,0xEE,0x0000);
		write_data(SSD_DEV2,0xEE,0x0000);
#elif defined(SLEEP_USE_TEST)
		_cpld_bank2disp_enable(CLEAN_USED_BANK,1,1);
#endif
		sleep_in();                  
		HAL_Delay(1000);

#if defined(SLEEP_USE_BIST)		
		write_data(SSD_DEV1,0xEE,0x0600);
		write_data(SSD_DEV2,0xEE,0x0600);
#elif defined(SLEEP_USE_TEST)
		_cpld_bank2disp_enable(TEST_USED_BANK,1,1);
#endif
		sleep_out();
		
		mipi_te_cnt = 0;
		HAL_Delay(1000);
		if(mipi_te_cnt < 15)
			while(1)
				{
				HAL_Delay(1);
				}
				
		}

}
/*******************************************************************************
* Function Name  : init
* Description    : Сбросьте два SSD2828 и настройте их отдельно
* Input          : None
* Output         : None
* Return         : 配置结果 0-err; 1-ok
*******************************************************************************/

uint8_t SSD2828::init()
{	
#if defined(SSD_CFG_CHECK_ENA)
	open_file();
#endif
	 CPLD_RST_On();
	 HAL_Delay(5);
	 MIPI_PwrEn_Off();
	 HAL_Delay(200);
	 MIPI_Ce_Off();
	 HAL_Delay(100);

	 CPLD_RST_On();
	 HAL_Delay(200);
	 MIPI_Ce_On();
	 HAL_Delay(200);
	 MIPI_PwrEn_On();
	 HAL_Delay(50);
	 CPLD_RST_Off();
	 HAL_Delay(100);

	if(read_id(SSD_DEV1) != SSD2828_ID) {return 0;}
	if(read_id(SSD_DEV2) != SSD2828_ID) {return 0;}
	
  	FLASH_SPISetSpeed(SPI_BAUDRATEPRESCALER_128);	
	
	_cpld_bank2disp_enable(TEST_USED_BANK,1,0);
	HAL_Delay(50);

	ssd_cfg(SSD_DEV1);
	HAL_Delay(200);
	ls055r1sx04_cfg(SSD_DEV1);
	HAL_Delay(200);
  
	ssd_cfg(SSD_DEV2);
	HAL_Delay(100);
	ls055r1sx04_cfg(SSD_DEV2);
	HAL_Delay(100);

	sleep_in();
	HAL_Delay(10);
	_cpld_bank2disp_enable(TEST_USED_BANK,0,0);
	HAL_Delay(20);

	FLASH_SPISetSpeed(SPI_BAUDRATEPRESCALER_8);	
#if defined(SSD_CFG_CHECK_ENA)
	close_file();
#endif
	
	
	return 1;
}

/*******************************************************************************
* Function Name  : test
* Description    : SSD2828 тестовый вход
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void SSD2828::test()
{
        init();
		SPI1_SetSpeed(SPI_BAUDRATEPRESCALER_128);
		HAL_Delay(20);
		while(1)
			{
			//MIPI_Read_Init(SSD_DEV1);
			rd_dsi_basicPAs(SSD_DEV1,4,0x04,0);
			rd_dsi_basicPAs(SSD_DEV1,2,0xf4,0);

		
			//MIPI_Read_Init(SSD_DEV2);
			rd_dsi_basicPAs(SSD_DEV2,4,0x04,0);
			rd_dsi_basicPAs(SSD_DEV2,2,0xf4,0);
			HAL_Delay(1000);
			}
		
		
}



/*--------------------TEST----------------------------*/

uint16_t SSD2828::read_mipi(uint8_t DEV)
{
	uint16_t data = 0;	
	uint8_t data_L = 0;
	uint8_t data_H = 0;
	if(DEV == SSD_DEV1)			MIPI_SsdA_Cs_On();
	else if(DEV == SSD_DEV2)	MIPI_SsdB_Cs_On();
	
	MIPI_SDC_On();
	FLASH_SPIWriteReadByte(SSD_RRA); 	
	MIPI_SDC_Off();

	data_L = FLASH_SPIWriteReadByte(Dummy_Byte);
	data_H = FLASH_SPIWriteReadByte(Dummy_Byte);
	data|=data_H<<8;	 
	data|=data_L;
	if(DEV == SSD_DEV1)			MIPI_SsdA_Cs_Off();
	else if(DEV == SSD_DEV2)	MIPI_SsdB_Cs_Off();

	return data;

}

uint16_t oregist1,oregist2;
uint16_t oregistBuf[20];
uint16_t ssd_id;

uint8_t SSD2828::rd_dsi_basicPAs(uint8_t DEV,uint8_t Num,uint8_t addr,uint8_t Page)
{
	uint8_t PAs,LEs,i,j,m,read_dat0,read_dat1;
	LEs = Num;
	PAs = addr;
	j = Num;
	j = (j/2)+(j%2);
	do{ 
		switch(Page)
		{
		   case 0: passWord(DEV,0);break;
		   case 1: passWord(DEV,1);break;
		   case 2: passWord(DEV,2);break;
		} 
			
		ssd_id = read_data(DEV,0xB7);
	//---MIPI Read ------------------//
		write_data(DEV,0xb7,0x0382);
		write_data(DEV,0xbb,0x0003);

		write_data(DEV,0xc1,(LEs & 0x00FF));
		write_data(DEV,0xc0,0x0001);

		write_data(DEV,0xbc,0x0001);
		
		
		//printf("Read Driver IC Reg is 0x%x\n\r",(uint16_t)PAs);	 
		// Send Read Driver IC Reg Addr
		write_data(DEV,0xbf,(PAs & 0x00FF));
		HAL_Delay(20);
	
		write_data(DEV,0xd4,0x00fa);	//Read CMD
		
	  	oregist1= read_data(DEV,0xC6);	//ISR 0XC6
		
		}
     while(!((oregist1 & 0x01)==0x01));	//This bit reflects whether	1.0xFFFF;0xC79F;0xC796
										//the data from the MIPI slave is ready for read by
										//the application processor.
	writec(DEV,0xFF);

	for(i = 0; i < j;i++)
	{
	oregist2=read_mipi(DEV);
	if(i<20)
		oregistBuf[i] = oregist2;
 	//printf("Read mipi data is (0x%x)\n",(uint16_t)(oregist2));
 	}
	oregist1= read_data(DEV,0xC6);	//ISR 0XC6				2.0xFFFF;0xC796
	
	return 1;
}


void SSD2828::writec(uint8_t DEV,uint8_t data)
{
	if(DEV == SSD_DEV1)			MIPI_SsdA_Cs_On();
	else if(DEV == SSD_DEV2)	MIPI_SsdB_Cs_On();
	
	MIPI_SDC_On();
	FLASH_SPIWriteReadByte(data);	//待写入命令
	MIPI_SDC_Off();

	if(DEV == SSD_DEV1)			MIPI_SsdA_Cs_Off();
	else if(DEV == SSD_DEV2)	MIPI_SsdB_Cs_Off();

}
void SSD2828::writed(uint8_t DEV,uint8_t data)
{
	if(DEV == SSD_DEV1)			MIPI_SsdA_Cs_On();
	else if(DEV == SSD_DEV2)	MIPI_SsdB_Cs_On();
	
	FLASH_SPIWriteReadByte(data);	//待写入数据

	if(DEV == SSD_DEV1)			MIPI_SsdA_Cs_Off();
	else if(DEV == SSD_DEV2)	MIPI_SsdB_Cs_Off();

}

void SSD2828::gp_commad_pa(uint8_t DEV,uint16_t data)
{
	write_data(DEV,0xbc,data);
	writec(DEV,0xbf);
}

void SSD2828::passWord(uint8_t DEV,uint8_t data)
{
	gp_commad_pa(DEV,2);
	writed(DEV,0xFF);
	writed(DEV,data);
}

