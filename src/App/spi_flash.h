#ifndef __spi_flash_H
#define __spi_flash_H

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"
#include "main.h"
#include "fst_spi.h"

	
#define W25Q_FLAG_BUSY				(uint32_t)(1 << 0)

#define W25Q_STATUS1READ_CMD		(uint8_t)0x05
#define W25Q_STATUS1WRITE_CMD		(uint8_t)0x01
#define W25Q_STATUS2READ_CMD		(uint8_t)0x35
#define W25Q_STATUS2WRITE_CMD		(uint8_t)0x31
#define W25Q_STATUS3READ_CMD		(uint8_t)0x15
#define W25Q_STATUS3WRITE_CMD		(uint8_t)0x11
#define W25Q_READID_CMD				(uint8_t)0x90
#define W25Q_READDATA_CMD			(uint8_t)0x03
#define W25Q_WRITEENABLE_CMD		(uint8_t)0x06
#define W25Q_ERASESECTOR_CMD		(uint8_t)0x20
#define W25Q_PROGRAMPAGE_CMD		(uint8_t)0x02


typedef struct
{
	uint32_t	sector_size;
	uint32_t	sectors_count;
	uint32_t	page_size;
} SPIFL_INFO;
	

uint32_t		_spifl_ReadStatus();
void			_spifl_WriteStatus(uint32_t val);
void			_spifl_WaitBusy();
void			_spifl_WriteEnable();

void			SPIFL_Init();
uint32_t		SPIFL_GetSectorSize();
uint32_t		SPIFL_GetSectorsCount();

uint16_t		SPIFL_ReadID();
void			SPIFL_ReadBuff(uint32_t addr, uint32_t dlen, uint8_t *dbuff);
void			SPIFL_ReadBuffDMA(uint32_t addr, uint32_t dlen, uint8_t *dbuff);
void			SPIFL_EraseSector(uint32_t addr);
void			SPIFL_WriteBuff(uint32_t addr, uint32_t dlen, uint8_t *dbuff);

	

#ifdef __cplusplus
}
#endif
#endif /*__spi_flash_H */

