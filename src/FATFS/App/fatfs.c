/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include "fatfs.h"

uint8_t		retUSBH;    /* Return value for USBH */
TCHAR		UsbPath[4];   /* USBH logical drive path */
FATFS		UsbFS;    /* File system object for USBH logical drive */

uint8_t		retSPIFL;
TCHAR		SpiflPath[4];
FATFS		SpiflFS;

void FATFS_Init(void) 
{
	//## FatFS: Link the USBH driver ###########################
	retUSBH = FATFS_LinkDriver(&USBH_Driver, UsbPath);

	retSPIFL = FATFS_LinkDriver(&SPIFL_Driver, SpiflPath);
}

/**
  * @brief  Gets Time from RTC 
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  return 0;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
