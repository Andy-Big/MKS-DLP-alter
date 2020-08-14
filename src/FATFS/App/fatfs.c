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
#include "rtc.h"
#include "unicode_utils.h"

uint8_t		retUSBH;    /* Return value for USBH */
TCHAR		UsbPath[4];   /* USBH logical drive path */
FATFS		UsbFS;    /* File system object for USBH logical drive */

uint8_t		retSPIFL;
TCHAR		SpiflPath[4];
FATFS		SpiflFS;

void		FATFS_Init(void) 
{
	//## FatFS: Link the USBH driver ###########################
	retUSBH = FATFS_LinkDriver(&USBH_Driver, UsbPath);

	retSPIFL = FATFS_LinkDriver(&SPIFL_Driver, SpiflPath);
}
//==============================================================================



/**
  * @brief  Gets Time from RTC 
  * @param  None
  * @retval Time in DWORD
  */
DWORD		get_fattime(void)
{
	DWORD		datetime = 0;
	RTC_TimeTypeDef		htime;
	RTC_DateTypeDef		hdate;
	
	HAL_RTC_GetTime(&hRTC, &htime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hRTC, &hdate, RTC_FORMAT_BIN);
	
	datetime = (uint32_t)(hdate.Year + 20) << 25;
	datetime += (uint32_t)(hdate.Month) << 21;
	datetime += (uint32_t)(hdate.Date) << 16;
	datetime += (uint32_t)(htime.Hours) << 11;
	datetime += (uint32_t)(htime.Minutes) << 5;
	datetime += htime.Seconds;
	
	return datetime;
}
//==============================================================================




char*		FATFS_GetFileExtension(char *fname)
{
	if (fname == NULL)
		return NULL;
	
	int16_t	i = strlen(fname) - 1;
	while (fname[i] != 0 && fname[i] != '.' && i >= 0)
		i--;
	if (i < 0)
		return "";
	
	return (fname+i+1);
}
//==============================================================================




char*		FATFS_GetFileExtensionUTF(char *fname)
{
	if (fname == NULL)
		return NULL;
	
	int16_t	i = strlen(fname) - 1;
	if (fname[i] >= 0x80)
		i--;
	while (fname[i] != 0 && fname[i] != '.' && i >= 0)
	{
		i--;
		if (fname[i] >= 0x80)
			i--;
	}
	if (i < 0)
		return "";
	
	return (fname+i+1);
}
//==============================================================================




void		FATFS_DelFileExtension(char *fname)
{
	if (fname == NULL)
		return;
	
	char	*ffname = FATFS_GetFileExtension(fname);
	if (*ffname != 0)
	{
		ffname--;
		*ffname = 0;
	}
	return;
}
//==============================================================================




void		FATFS_DelFileExtensionUTF(char *fname)
{
	if (fname == NULL)
		return;
	
	char	*ffname = FATFS_GetFileExtensionUTF(fname);
	if (*ffname != 0)
	{
		ffname--;
		if (*ffname >= 0x80)
			ffname--;
		*ffname = 0;
	}
	
	return;
}
//==============================================================================




char*		FATFS_GetPrevDir(char *fname)
{
	if (fname == NULL)
		return NULL;
	
	int16_t	i = strlen(fname) - 1;
	while (fname[i] != 0 && fname[i] != '/' && i >= 0)
		i--;
	if (i < 0)
		return "";
	
	return (fname+i+1);
}
//==============================================================================




char*		FATFS_GetPrevDirUTF(char *fname)
{
	if (fname == NULL)
		return NULL;
	
	int16_t	i = strlen(fname) - 1;
	if (fname[i] >= 0x80)
		i--;
	while (fname[i] != 0 && fname[i] != '/' && i >= 0)
	{
		i--;
		if (fname[i] >= 0x80)
			i--;
	}
	if (i < 0)
		return "";
	
	return (fname+i+1);
}
//==============================================================================




