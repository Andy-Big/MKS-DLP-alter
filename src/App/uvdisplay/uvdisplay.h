#ifndef _uvdisplay_H_
#define _uvdisplay_H_

#include "stm32f4xx_hal.h"
#include "main.h"
	 
#define MIPI_PwrEn_On()				MIPI_EN_PWR_GPIO_Port->BSRR = MIPI_EN_PWR_Pin
#define MIPI_PwrEn_Off()			MIPI_EN_PWR_GPIO_Port->BSRR = (uint32_t)MIPI_EN_PWR_Pin << 16U

#define MIPI_Ce_On()				MIPI_CE_GPIO_Port->BSRR = MIPI_CE_Pin
#define MIPI_Ce_Off()				MIPI_CE_GPIO_Port->BSRR = (uint32_t)MIPI_CE_Pin << 16U

#define MIPI_SsdaCs_Off()			SPI1_CS_SSDA_GPIO_Port->BSRR = SPI1_CS_SSDA_Pin
#define MIPI_SsdaCs_On()			SPI1_CS_SSDA_GPIO_Port->BSRR = (uint32_t)SPI1_CS_SSDA_Pin << 16U

#define MIPI_SsdbCs_Off()			SPI1_CS_SSDB_GPIO_Port->BSRR = SPI1_CS_SSDB_Pin
#define MIPI_SsdbCs_On()			SPI1_CS_SSDB_GPIO_Port->BSRR = (uint32_t)SPI1_CS_SSDB_Pin << 16U

#define CPLD_Cs_Off()				SPI1_CS_SSDA_GPIO_Port->BSRR = SPI1_CS_SSDA_Pin
#define CPLD_Cs_On()				SPI1_CS_SSDA_GPIO_Port->BSRR = (uint32_t)SPI1_CS_SSDA_Pin << 16U



void		UVD_Init();




#endif // _uvdisplay_H