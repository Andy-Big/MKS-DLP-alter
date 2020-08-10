/******************** (C) COPYRIGHT 2018 www.makerbase.com ********************
* File Name          : ssd2828.h
* Author             : www.armjishu.com
* Version            : V1.0
* Library            : Using STM32F2XX_STDPERIPH_VERSION V3.3.0
* Date               : 07/25/2018
* Description        : Header for ssd2828.c file.
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SSD2828_H
#define __SSD2828_H
#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif





// LS_VSP_N_ENA_OP
#define MIPI_PwrEn_On()				MIPI_EN_PWR_GPIO_Port->BSRR = MIPI_EN_PWR_Pin					// Set bit
#define MIPI_PwrEn_Off()			MIPI_EN_PWR_GPIO_Port->BSRR = (uint32_t)MIPI_EN_PWR_Pin << 16U	// Reset bit

// LS_IOVCC_ENA_OP
#define MIPI_Ce_On()				MIPI_CE_GPIO_Port->BSRR = MIPI_CE_Pin
#define MIPI_Ce_Off()				MIPI_CE_GPIO_Port->BSRR = (uint32_t)MIPI_CE_Pin << 16U

#define MIPI_SDC_Off()				SPI1_SDC_GPIO_Port->BSRR = SPI1_SDC_Pin
#define MIPI_SDC_On()				SPI1_SDC_GPIO_Port->BSRR = (uint32_t)SPI1_SDC_Pin << 16U

#define MIPI_SsdA_Cs_Off()			SPI1_CS_SSDA_GPIO_Port->BSRR = SPI1_CS_SSDA_Pin
#define MIPI_SsdA_Cs_On()			SPI1_CS_SSDA_GPIO_Port->BSRR = (uint32_t)SPI1_CS_SSDA_Pin << 16U

#define MIPI_SsdB_Cs_Off()			SPI1_CS_SSDB_GPIO_Port->BSRR = SPI1_CS_SSDB_Pin
#define MIPI_SsdB_Cs_On()			SPI1_CS_SSDB_GPIO_Port->BSRR = (uint32_t)SPI1_CS_SSDB_Pin << 16U





/* Includes ------------------------------------------------------------------*/
//#include "sys.h"  
#include "stm32f4xx.h"
//#include "stm32f4xx_spi.h"
//#include "mks_fastio.h"  
#include "ff.h"

/* User Define --------------------------------------------------------*/
//#define PCLK_100M
#define PCLK_50M

//#define PLL_768M
#define PLL_384M

#if defined(PCLK_100M)
#define SLEEP_DELAY	1
#elif defined(PCLK_50M)
#define SLEEP_DELAY 2
#endif


//#define SSD_CFG_CHECK_ENA

#define SSD_CFG_NUM     5       //閰嶇疆鍒嗘鏁
#define SSD_DEV1	1		//芯片1
#define SSD_DEV2	2		//芯片2
#define Dummy_Byte	0xFF
#define SSD_RRA		0xFA
#define SSD2828_ID	0x2828
/* SSD2828 Register Summary ---------------------------------------------------*/
//*	   Mnemonic	   Offset	Reset Value	---------------------------------------*/														*/
#define SSD_DIR		0xB0	//0x2828
#define SSD_VICR1	0xB1	//0x020A
#define SSD_VICR2	0xB2	//0x0214
#define SSD_VICR3	0xB3	//0x0428
#define SSD_VICR4	0xB4	//0x0780
#define SSD_VICR5	0xB5	//0x0438
#define SSD_VICR6	0xB6	//0x0024
#define SSD_CFGR	0xB7	//0x0301
#define SSD_VCR		0xB8	//0x0045
#define SSD_PCR		0xB9	//0x0000
#define SSD_PLCR	0xBA	//0x8120
#define SSD_CCR		0xBB	//0x0003
#define SSD_PSCR1	0xBC	//0x0000
#define SSD_PSCR2	0xBD	//0x0000
#define SSD_PSCR3	0xBE	//0x0100
#define SSD_PDR		0xBF	//0x0000
#define SSD_OCR		0xC0	//0x0000
#define SSD_MRSR	0xC1	//0x0001
#define SSD_PDCR	0xC2	//0x0000
#define SSD_ARSR	0xC3	//0x0000
#define SSD_LCR		0xC4	//0x0000
#define SSD_ICR		0xC5	//0x0080
#define SSD_ISR		0xC6	//0xCF06
#define SSD_ESR		0xC7	//0x0000
#define SSD_DAR1	0xC9	//0x1402
#define SSD_DAR2	0xCA	//0x2803
#define SSD_DAR3	0xCB	//0x0416
#define SSD_DAR4	0xCC	//0x0A0A
#define SSD_DAR5	0xCD	//0x1000
#define SSD_DAR6	0xCE	//0x0405
#define SSD_HTTR1 	0xCF	//0x0000
#define SSD_HTTR2 	0xD0	//0x0010
#define SSD_LRTR1 	0xD1	//0x0000
#define SSD_LRTR2 	0xD2	//0x0010
#define SSD_TSR 	0xD3	//0x0000
#define SSD_LRR 	0xD4	//0x00FA
#define SSD_PLLR 	0xD5	//0x1450
#define SSD_TR 		0xD6	//0x0005
#define SSD_TECR 	0xD7	//0x0001
#define SSD_ACR1 	0xD8	//0x2020
#define SSD_ACR2 	0xD9	//0x64A0
#define SSD_ACR3 	0xDA	//0x99A4
#define SSD_ACR4 	0xDB	//0x8098
#define SSD_IOCR 	0xDC	//0x0000
#define SSD_VICR7 	0xDD	//0x0000
#define SSD_LCFR 	0xDE	//0x0000
#define SSD_DAR7 	0xDF	//0x0010
#define SSD_PUCR1 	0xE0	//0x5556
#define SSD_PUCR2 	0xE1	//0x6656
#define SSD_PUCR3 	0xE2	//0x0159
#define SSD_CBCR1 	0xE9	//0x0000
#define SSD_CBCR2 	0xEA	//0x6900
#define SSD_CBSR 	0xEB	//0x0000
#define SSD_ECR 	0xEC	//0x7800
#define SSD_VSDR 	0xED	//0x0002
#define SSD_TMR 	0xEE	//0x0000
#define SSD_GPIO1 	0xEF	//0x0000
#define SSD_GPIO2 	0xF0	//0x0000
#define SSD_DLYA01 	0xF1	//0x2020
#define SSD_DLYA23 	0xF2	//0x2020
#define SSD_DLYB01 	0xF3	//0x2020
#define SSD_DLYB23 	0xF4	//0x2020
#define SSD_DLYC01 	0xF5	//0x2020
#define SSD_DLYC23 	0xF6	//0x2020
#define SSD_ACR5 	0xF7	//0x0000
#define SSD_RR 		0xFF	//0x0000


class  SSD2828{

	public:
#if defined(SSD_CFG_CHECK_ENA)	
		FIL file;
#endif	

		SSD2828();
		void reset();
		uint16_t read_id(uint8_t DEV);
		uint16_t read_data(uint8_t DEV,uint8_t reg);
		uint8_t read_byte(uint8_t DEV,uint8_t reg);
		void write_data(uint8_t DEV,uint8_t reg,uint16_t data);
		void write_byte(uint8_t DEV,uint8_t reg,uint8_t data);
		void ssd_cfg(uint8_t DEV);
		void ls055r1sx04_cfg(uint8_t DEV);
		
		void configure(uint8_t DEV);
		
		uint8_t init();
		void test();
/*--------------------TEST----------------------------*/
	uint16_t read_mipi(uint8_t DEV);
	void writec(uint8_t DEV,uint8_t data);
	 void writed(uint8_t DEV,uint8_t data);
	 void gp_commad_pa(uint8_t DEV,uint16_t data);
	 void passWord(uint8_t DEV,uint8_t data);
	 uint8_t rd_dsi_basicPAs(uint8_t DEV,uint8_t Num,uint8_t addr,uint8_t Page);
	 void write_Generic(uint8_t DEV,uint8_t addr,uint16_t data);
	 void write_DCS(uint8_t DEV,uint8_t addr,uint16_t data,uint8_t size);
	 void write_DCS_sleep(uint8_t DEV,uint8_t addr);
	 void sleep_in();
	 void sleep_out();
	 void sleep_test();


 };



/* Exported functions ------------------------------------------------------- */
/*----- High layer function -----*/
#if 0
//void SSD2828_RESET();
uint16_t SSD2828_READ_ID(uint16_t DEV);
uint16_t SSD2828_READ_DATA(uint16_t reg,uint16_t DEV);
void SSD2828_WRITE_DATA(uint16_t reg,uint16_t DEV,uint16_t data);
void SSD2828_CFG(uint16_t DEV,uint16_t seq);
void SSD2828_TEST();
uint16_t SSD2828_INIT();

void LS055R1SX04_POWER_ON();
void LS055R1SX04_POWER_OFF();


uint16_t SSD2828_READ_MIPI(uint16_t DEV);

void SSD2828_WRITE_CMD(uint16_t DEV);
#endif

/*----- Low layer function -----*/

//LCD 视频显示参数，根据LCD不同更改
//----  Bei li jia -----//
#define LCD_VBPD		 5
#define LCD_VFPD	 	 30
#define LCD_VSPW		 2

#define LCD_HBPD		 118
#define LCD_HFPD		 60
#define LCD_HSPW	   	 4

#define LCD_XSIZE_TFT    1080 
#define LCD_YSIZE_TFT    1920


#if defined(__cplusplus)
}     /* Make sure we have C-declarations in C++ programs */
#endif


#endif /* __SSD2828_H */

/******************* (C) COPYRIGHT 2018 www.makerbase.com *****END OF FILE****/
