/**
  ******************************************************************************
  * @file   fatfs.h
  * @brief  Header for fatfs applications
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

#ifndef __fatfs_H
#define __fatfs_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "ff.h"
#include "ff_gen_drv.h"
#include "usbh_diskio.h" /* defines USBH_Driver as external */
#include "spifl_diskio.h" /* defines SPIFL_Driver as external */


extern uint8_t		retUSBH; /* Return value for USBH */
extern TCHAR		UsbPath[4]; /* USBH logical drive path */
extern FATFS		USBHFatFS; /* File system object for USBH logical drive */

extern uint8_t		retSPIFL;
extern TCHAR		SpiflPath[4];
extern FATFS		SpiflFS;


void FATFS_Init(void);

#ifdef __cplusplus
}
#endif
#endif /*__fatfs_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
