#ifndef __fst_spi_H
#define __fst_spi_H

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"
#include "main.h"


#define USE_HAL_SPI_REGISTER_CALLBACKS	1U

#define		_touch_CS_Enable()	TOUCH_CS_GPIO->BSRR = (uint32_t)TOUCH_CS_Pin << 16U
#define 	_touch_CS_Disable()	TOUCH_CS_GPIO->BSRR = TOUCH_CS_Pin
#define		TOUCH_READS			(uint8_t)16
#define		TOUCH_BUFF_SIZE		(uint8_t)6*TOUCH_READS



#define		_flash_CS_Enable()	FST_SPI_CS_FLASH_GPIO->BSRR = (uint32_t)FST_SPI_CS_FLASH_Pin << 16U
#define 	_flash_CS_Disable()	FST_SPI_CS_FLASH_GPIO->BSRR = FST_SPI_CS_FLASH_Pin



#define		_ssda_CS_Enable()	FST_SPI_CS_SSDA_GPIO->BSRR = (uint32_t)FST_SPI_CS_SSDA_Pin << 16U
#define 	_ssda_CS_Disable()	FST_SPI_CS_SSDA_GPIO->BSRR = FST_SPI_CS_SSDA_Pin
#define		_ssdb_CS_Enable()	FST_SPI_CS_SSDB_GPIO->BSRR = (uint32_t)FST_SPI_CS_SSDB_Pin << 16U
#define 	_ssdb_CS_Disable()	FST_SPI_CS_SSDB_GPIO->BSRR = FST_SPI_CS_SSDB_Pin



#define		_cpld_CS_Enable()	FST_SPI_CS_CPLD_GPIO->BSRR = (uint32_t)FST_SPI_CS_CPLD_Pin << 16U
#define 	_cpld_CS_Disable()	FST_SPI_CS_CPLD_GPIO->BSRR = FST_SPI_CS_CPLD_Pin	


extern SPI_HandleTypeDef		hFlashSpi;



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
inline uint8_t		FLASH_IsDMAReady() { return (hFlashSpi.State == HAL_SPI_STATE_READY); }




#ifdef __cplusplus
}
#endif
#endif /*__fst_spi_H */

