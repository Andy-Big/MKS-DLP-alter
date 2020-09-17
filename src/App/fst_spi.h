#ifndef __fst_spi_H
#define __fst_spi_H

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"
#include "main.h"


#define USE_HAL_SPI_REGISTER_CALLBACKS	1U

#define		_touch_CS_Enable()	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET)
#define 	_touch_CS_Disable()	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET)
#define		TOUCH_READS			(uint8_t)16
#define		TOUCH_BUFF_SIZE		(uint8_t)6*TOUCH_READS



#define		_flash_CS_Enable()	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET)
#define 	_flash_CS_Disable()	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET)



#define		_ssda_CS_Enable()	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET)
#define 	_ssda_CS_Disable()	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET)
#define		_ssdb_CS_Enable()	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET)
#define 	_ssdb_CS_Disable()	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET)



#define		_cpld_CS_Enable()	SPI1_CS_CPLD_GPIO_Port->BSRR = (uint32_t)SPI1_CS_CPLD_Pin << 16U
#define 	_cpld_CS_Disable()	SPI1_CS_CPLD_GPIO_Port->BSRR = SPI1_CS_CPLD_Pin	



void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);


void		TOUCH_SPIInit(void);
void		TOUCH_SPIEnable();
void		TOUCH_SPIDisable();
void		TOUCH_SPIStartRead();
HAL_SPI_StateTypeDef	TOUCH_SPIGetState();



uint16_t	_flash_SPIGetFlags();

void		FLASH_SPIInit(void);
void		FLASH_SPIEnable();
void		FLASH_SPIDisable();
void		FLASH_SPISetSpeed(uint16_t prescaler);
uint16_t		FLASH_SPIGetSpeed();

uint8_t		FLASH_SPIWriteReadByte(uint8_t txval);
void		FLASH_SPIReadBuff(uint32_t dlen, uint8_t *dbuff);
void		FLASH_SPIWriteBuff(uint32_t dlen, uint8_t *dbuff);
void		FLASH_SPIReadBuffDMA(uint32_t dlen, uint8_t *dbuff);
void		FLASH_SPIWriteBuffDMA(uint32_t dlen, uint8_t *dbuff);
uint8_t		FLASH_IsDMAReady();



#ifdef __cplusplus
}
#endif
#endif /*__fst_spi_H */

