#include "ff.h"
#include "ff_gen_drv.h"
#include "spifl_diskio.h"




DSTATUS		SPIFL_initialize (BYTE);
DSTATUS		SPIFL_status (BYTE);
DRESULT		SPIFL_read (BYTE, BYTE*, DWORD, UINT);
DRESULT		SPIFL_write (BYTE, const BYTE*, DWORD, UINT);
DRESULT		SPIFL_ioctl (BYTE, BYTE, void*);



const Diskio_drvTypeDef  SPIFL_Driver =
{
  SPIFL_initialize,
  SPIFL_status,
  SPIFL_read,
  SPIFL_write,
  SPIFL_ioctl,
};





DSTATUS SPIFL_initialize(BYTE lun)
{
	// CAUTION : SPI Flash library must to be initialized in the application
	
	if (_spifl_info.sector_size == 0)
	{
		SPIFL_Init();
		if (_spifl_info.sector_size == 0)
			return RES_NOTRDY;
	}

	return RES_OK;
}
//==============================================================================




DSTATUS SPIFL_status(BYTE lun)
{
	if (_spifl_info.sector_size == 0)
		return RES_NOTRDY;

	return RES_OK;
}
//==============================================================================




DRESULT SPIFL_read(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{
	DRESULT res = RES_ERROR;

	if (_spifl_info.sector_size == 0)
		return RES_NOTRDY;

	uint16_t	spd = FLASH_SPIGetSpeed();
	FLASH_SPISetSpeed(SPI_BAUDRATEPRESCALER_2);
	SPIFL_ReadBuff(sector * (_spifl_info.sector_size / _SPIFL_SECT_DIVIDER), count * (_spifl_info.sector_size / _SPIFL_SECT_DIVIDER), (uint8_t*)buff);
	FLASH_SPISetSpeed(spd);
	res = RES_OK;

	return res;
}
//==============================================================================




DRESULT SPIFL_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
	DRESULT res = RES_ERROR;

	if (_spifl_info.sector_size == 0)
		return RES_NOTRDY;

	uint16_t	spd = FLASH_SPIGetSpeed();
	FLASH_SPISetSpeed(SPI_BAUDRATEPRESCALER_2);
	SPIFL_WriteBuff(sector * (_spifl_info.sector_size / _SPIFL_SECT_DIVIDER), count * (_spifl_info.sector_size / _SPIFL_SECT_DIVIDER), (uint8_t*)buff);
	FLASH_SPISetSpeed(spd);

	res = RES_OK;

	return res;
}
//==============================================================================




DRESULT SPIFL_ioctl(BYTE lun, BYTE cmd, void *buff)
{
	DRESULT res = RES_ERROR;


	res = RES_OK;
	switch (cmd)
	{
		// Make sure that no pending write process
		case CTRL_SYNC:
			if (lun == 0)
				_spifl_WaitBusy();
			res = RES_OK;
			break;

		// Get number of sectors on the disk (DWORD)
		case GET_SECTOR_COUNT :
			*(DWORD*)buff = _spifl_info.sectors_count * _SPIFL_SECT_DIVIDER - (128 * _SPIFL_SECT_DIVIDER); // 128 sectors (128*4096 bytes) reserved for firmware image
			res = RES_OK;
			break;

		// Get R/W sector size (WORD)
		case GET_SECTOR_SIZE :
			*(DWORD*)buff = _spifl_info.sector_size / _SPIFL_SECT_DIVIDER;
			res = RES_OK;
			break;

		// Get erase block size in unit of sector (DWORD)
		case GET_BLOCK_SIZE :
			*(DWORD*)buff = _SPIFL_SECT_DIVIDER;
			res = RES_OK;
			break;

		// Erase sectors
		case CTRL_TRIM :
			//SPIFL_WriteBuff(sector * (_spifl_info.sector_size / _SPIFL_SECT_DIVIDER), count * (_spifl_info.sector_size / _SPIFL_SECT_DIVIDER), (uint8_t*)buff);
			res = RES_OK;
			break;

		default:
			res = RES_PARERR;
	}

	return res;
}
//==============================================================================




