#include "spi_flash.h"


extern			SPI_HandleTypeDef	hFlashSpi;

uint8_t			sectorbuff[4096];
SPIFL_INFO		_spifl_info = {0, 0};

uint32_t		_spifl_bust_counts;
volatile uint32_t		d = 0;



static inline void		___spifl_wait_cs()
{
	volatile uint8_t i = 100;
	while (i)
		i--;
}







uint32_t		_spifl_ReadStatus()
{
	uint32_t	retval = 0;
	uint16_t	rxval = 0;

	___spifl_wait_cs();
	_flash_CS_Enable();
	FLASH_SPIWriteReadByte(W25Q_STATUS1READ_CMD);
	rxval = FLASH_SPIWriteReadByte(0);
	_flash_CS_Disable();
	retval = rxval;

	___spifl_wait_cs();
	_flash_CS_Enable();
	FLASH_SPIWriteReadByte(W25Q_STATUS2READ_CMD);
	rxval = FLASH_SPIWriteReadByte(0);
	_flash_CS_Disable();
	retval += (uint32_t)rxval << 8;
	
	___spifl_wait_cs();
	_flash_CS_Enable();
	FLASH_SPIWriteReadByte(W25Q_STATUS3READ_CMD);
	rxval = FLASH_SPIWriteReadByte(0);
	_flash_CS_Disable();
	retval += (uint32_t)rxval << 16;

	return retval;
}
//==============================================================================




void			_spifl_WriteStatus(uint32_t val)
{
	uint16_t	txval = 0;

	___spifl_wait_cs();
	txval = val & 0xFF;
	_flash_CS_Enable();
	FLASH_SPIWriteReadByte(W25Q_STATUS1WRITE_CMD);
	FLASH_SPIWriteReadByte(txval);
	_flash_CS_Disable();
	txval = (val >> 8) & 0xFF;

	___spifl_wait_cs();
	_flash_CS_Enable();
	FLASH_SPIWriteReadByte(W25Q_STATUS2WRITE_CMD);
	FLASH_SPIWriteReadByte(txval);
	_flash_CS_Disable();
	txval = (val >> 16) & 0xFF;

	___spifl_wait_cs();
	_flash_CS_Enable();
	FLASH_SPIWriteReadByte(W25Q_STATUS3WRITE_CMD);
	FLASH_SPIWriteReadByte(txval);
	_flash_CS_Disable();

	return;
}
//==============================================================================




void		_spifl_WaitBusy()
{
	uint16_t	rxval = 0;
	_spifl_bust_counts = 0;
	
	while ((_flash_SPIGetFlags() & SPI_FLAG_BSY) || (_flash_SPIGetFlags() & SPI_FLAG_TXE) == 0 || hFlashSpi.State != HAL_SPI_STATE_READY);
	___spifl_wait_cs();

	_flash_CS_Enable();
	FLASH_SPIWriteReadByte(W25Q_STATUS1READ_CMD);
	rxval = FLASH_SPIWriteReadByte(0);
	while (rxval & W25Q_FLAG_BUSY)
	{
		d++;
		rxval = FLASH_SPIWriteReadByte(0);
	};
	_flash_CS_Disable();

	return;
}
//==============================================================================




void			_spifl_WriteEnable()
{
	while ((_flash_SPIGetFlags() & SPI_FLAG_BSY) || (_flash_SPIGetFlags() & SPI_FLAG_TXE) == 0 || hFlashSpi.State != HAL_SPI_STATE_READY);
	_spifl_WaitBusy();
	___spifl_wait_cs();

	_flash_CS_Enable();
	FLASH_SPIWriteReadByte(W25Q_WRITEENABLE_CMD);
	_flash_CS_Disable();
	
	return;
}
//==============================================================================









void		SPIFL_Init()
{
	uint16_t f_id = SPIFL_ReadID();
	switch (f_id)
	{
		// w25q64
		case 0xEF16:
			_spifl_info.sectors_count = 2048;
			_spifl_info.sector_size = 4096;
			_spifl_info.page_size = 256;
			break;
			
		// w25q128
		case 0xEF17:
			_spifl_info.sectors_count = 4096;
			_spifl_info.sector_size = 4096;
			_spifl_info.page_size = 256;
			break;
			
	}
	___spifl_wait_cs();
}
//==============================================================================




uint32_t		SPIFL_GetSectorSize()
{
	return _spifl_info.sector_size;
}
//==============================================================================




uint32_t		SPIFL_GetSectorsCount()
{
	return _spifl_info.sectors_count;
}
//==============================================================================




uint16_t		SPIFL_ReadID()
{
	uint16_t	retval = 0;
	uint16_t	rxval = 0;

	while ((_flash_SPIGetFlags() & SPI_FLAG_BSY) || (_flash_SPIGetFlags() & SPI_FLAG_TXE) == 0 || hFlashSpi.State != HAL_SPI_STATE_READY);
	_spifl_WaitBusy();
	___spifl_wait_cs();

	_flash_CS_Enable();
	FLASH_SPIWriteReadByte(W25Q_READID_CMD);
	FLASH_SPIWriteReadByte(0);
	FLASH_SPIWriteReadByte(0);
	FLASH_SPIWriteReadByte(0);
	rxval = FLASH_SPIWriteReadByte(0);
	retval = rxval << 8;
	rxval = FLASH_SPIWriteReadByte(0);
	retval += rxval;
	
	_flash_CS_Disable();

	return retval;
}
//==============================================================================




void		SPIFL_ReadBuff(uint32_t addr, uint32_t dlen, uint8_t *dbuff)
{
	if (dlen == 0 || dbuff == 0)
		return;
	
	while ((_flash_SPIGetFlags() & SPI_FLAG_BSY) || (_flash_SPIGetFlags() & SPI_FLAG_TXE) == 0 || hFlashSpi.State != HAL_SPI_STATE_READY);
	while (FLASH_IsDMAReady() == 0);
	_spifl_WaitBusy();
	___spifl_wait_cs();

	_flash_CS_Enable();
	FLASH_SPIWriteReadByte(W25Q_READDATA_CMD);
	FLASH_SPIWriteReadByte((addr >> 16) & 0xFF);
	FLASH_SPIWriteReadByte((addr >> 8) & 0xFF);
	FLASH_SPIWriteReadByte(addr & 0xFF);

	// DMA is not allowed with CCM SRAM (at address 0x10000000)
	if (dlen > 48 && ((uint32_t)dbuff & 0xFF000000) != 0x10000000)
		FLASH_SPIReadBuffDMA(dlen, dbuff);
	else
		FLASH_SPIReadBuff(dlen, dbuff);
		
	_flash_CS_Disable();

	return;
}
//==============================================================================




void		SPIFL_ReadBuffDMA(uint32_t addr, uint32_t dlen, uint8_t *dbuff)
{
	if (dlen == 0 || dbuff == 0)
		return;
	
	while ((_flash_SPIGetFlags() & SPI_FLAG_BSY) || (_flash_SPIGetFlags() & SPI_FLAG_TXE) == 0 || hFlashSpi.State != HAL_SPI_STATE_READY);
	_spifl_WaitBusy();
	___spifl_wait_cs();

	_flash_CS_Enable();
	FLASH_SPIWriteReadByte(W25Q_READDATA_CMD);
	FLASH_SPIWriteReadByte((addr >> 16) & 0xFF);
	FLASH_SPIWriteReadByte((addr >> 8) & 0xFF);
	FLASH_SPIWriteReadByte(addr & 0xFF);

	FLASH_SPIReadBuffDMA(dlen, dbuff);
		
	_flash_CS_Disable();

	return;
}
//==============================================================================



// addr - address of a any byte in sector
void			SPIFL_EraseSector(uint32_t addr)
{
	addr &= ~(_spifl_info.sector_size - 1);
	
	while ((_flash_SPIGetFlags() & SPI_FLAG_BSY) || (_flash_SPIGetFlags() & SPI_FLAG_TXE) == 0 || hFlashSpi.State != HAL_SPI_STATE_READY);
	_spifl_WriteEnable();	
	___spifl_wait_cs();
	
	_flash_CS_Enable();
	FLASH_SPIWriteReadByte(W25Q_ERASESECTOR_CMD);
	FLASH_SPIWriteReadByte((addr >> 16) & 0xFF);
	FLASH_SPIWriteReadByte((addr >> 8) & 0xFF);
	FLASH_SPIWriteReadByte(addr & 0xFF);
	_flash_CS_Disable();
}
//==============================================================================




void		SPIFL_WriteBuff(uint32_t addr, uint32_t dlen, uint8_t *dbuff)
{
	if (dlen == 0 || dbuff == 0)
		return;

	uint8_t		*buff = dbuff;
	
//	uint32_t	sector = addr & ~(_spifl_info.sector_size - 1);
//	uint32_t	oldsector = sector;
	// begin address in a sector
	uint32_t	addrinsect = addr & (_spifl_info.sector_size - 1);
	// begin address in a page
	uint32_t	addrinpage = addr & (_spifl_info.page_size - 1);
	
	// remain data length in a sector
	uint32_t	towritesect;
	// remain data in a page
	uint32_t	towritepage;
	
	while ((_flash_SPIGetFlags() & SPI_FLAG_BSY) || (_flash_SPIGetFlags() & SPI_FLAG_TXE) == 0 || hFlashSpi.State != HAL_SPI_STATE_READY);
	_spifl_WaitBusy();
	
	towritesect = dlen;
	if ((towritesect + addrinsect) > _spifl_info.sector_size)
		towritesect = _spifl_info.sector_size - addrinsect;
	// remain data in a page
	towritepage = towritesect;
	if ((towritepage + addrinpage) > _spifl_info.page_size)
		towritepage = _spifl_info.page_size - addrinpage;

	addrinpage = 0;
	addrinsect = 0;

	while (dlen)
	{
		// sector erase
		SPIFL_EraseSector(addr);
		
		while (towritesect)
		{
			_spifl_WaitBusy();
			___spifl_wait_cs();
			
			_spifl_WriteEnable();	
			___spifl_wait_cs();

			_flash_CS_Enable();
			FLASH_SPIWriteReadByte(W25Q_PROGRAMPAGE_CMD);
			FLASH_SPIWriteReadByte((addr >> 16) & 0xFF);
			FLASH_SPIWriteReadByte((addr >> 8) & 0xFF);
			FLASH_SPIWriteReadByte(addr & 0xFF);
			for (uint32_t i = 0; i < towritepage; i++)
			{
				FLASH_SPIWriteReadByte(*buff);
				buff++;
			}
			_flash_CS_Disable();
			
			dlen -= towritepage;
			towritesect -= towritepage;
			addr += towritepage;
			// remain data in a page
			towritepage = towritesect;
			if (towritepage > _spifl_info.page_size)
				towritepage = _spifl_info.page_size;
		}
		towritesect = dlen;
		if (towritesect > _spifl_info.sector_size)
			towritesect = _spifl_info.sector_size;
		towritepage = towritesect;
		if (towritepage > _spifl_info.page_size)
			towritepage = _spifl_info.page_size;
	}

	return;
}
//==============================================================================




