/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "rtc.h"
#include "usb_host.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "usbh_platform.h"
#include "lcd_low.h"
#include "touch.h"
#include "lcd_ui.h"
//#include "wchar.h"
#include "unicode_utils.h"
#include "spi_flash.h"
#include "tgui.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern USBH_HandleTypeDef		hUsbHostFS;
extern ApplicationTypeDef		Appli_state;
ApplicationTypeDef				Appli_state_old;

extern TCHAR					UsbPath[4];   /* USBH logical drive path */
extern FATFS					UsbFS;    /* File system object for USBH logical drive */

extern TCHAR					SpiflPath[4];
extern FATFS					SpiflFS;

extern TOUCH_INFO				touch_info;

extern uint8_t			*TEST_BMP;


__no_init uint8_t 		fbuff[8192] @ "CCMRAM";
__no_init uint8_t 		uibuff[8192];


FIL				ufile;
FIL				sfile;
TCHAR			u_tfname[512];
TCHAR			s_tfname[512];
char			cfname[1024];
char			msg[512];
FRESULT			fres;
DIR				dir = {0};
FILINFO			finfo = {0};


void SystemClock_Config(void);
void MX_USB_HOST_Process(void);


int main()
{
	HAL_Init();
	SystemClock_Config();
	SCB->VTOR = FLASH_BASE | 0x10000;
	
	GPIO_Init();

	LCD_Initializtion();
	LCDUI_Init();
	
	FLASH_SPIInit();
	FLASH_SPIEnable();
	FLASH_SPISetSpeed(SPI_BAUDRATEPRESCALER_2);
	SPIFL_Init();

	FATFS_Init();

	LCDUI_SetFont(LCDUI_FONT_H18);
	LCDUI_SetColor(COLOR_GREEN);
	LCDUI_SetBackColor(COLOR_BLACK);
	LCDUI_Clear();
	TGUI_Init();
	
	LCD_BackLight(1);
	LCDUI_DrawTextUTF((char*)"> Booting...\n");
	
	
	if (f_mount(&SpiflFS,  SpiflPath, 1) != FR_OK)
	{
		LCDUI_DrawTextUTF((char*)"> !! Internal flash filesystem error, booting stoped\n");
		while(1);
	}

	Touch_Init();
	Touch_Enable();
	
	RTC_Init();
	USB_HOST_VbusFS(1);

	RTC_Enable(&hRTC);

	USB_HOST_Init();
	Appli_state_old = Appli_state;
	
/*	
	if (tguiActiveScreen->buttons[0].funcs._call_paint != NULL)
		tguiActiveScreen->buttons[0].funcs._call_paint((void*)&(tguiActiveScreen->buttons[0]), NULL);
	
	LCDUI_SetCursorCoord(5, 0);
	LCDUI_DrawTextUTF((char*)"> Проверка\n");
	LCDUI_DrawTextUTF((char*)"> Starting...\n");


	if (f_mount(&SpiflFS,  SpiflPath, 1) != FR_OK)
	{
		LCDUI_DrawTextUTF((char*)"> !! Internal flash filesystem error, creating new one...\n");
		if (f_mkfs(SpiflPath, 0, uibuff, sizeof(uibuff)) != FR_OK)
		{
			LCDUI_DrawTextUTF((char*)"> !! Failed to create filesystem\n");
		}
		else
		{
			LCDUI_DrawTextUTF((char*)"> Filesystem created success\n");
			if (f_mount(&SpiflFS,  SpiflPath, 1) != FR_OK)
				LCDUI_DrawTextUTF((char*)"> !! Failed to mount new filesystem\n");
		}
	}

	else
	{
		LCDUI_DrawTextUTF((char*)"> Internal flash filesystem is ready\n");
			// Test for file write/read speed

			uint32_t	wr = 0;
			char		fname[] = "test файл.txt";

			tstrcpy(path, SpiflPath);
			tstrcat_utf(path, fname);
			if (f_open(&file, path, FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
			{
				uint32_t tc = HAL_GetTick();
				for (uint32_t i = 0; i < 40; i++)
				{
					if (f_write(&file, uibuff, 8192, &wr) != FR_OK || wr != 8192)
					{
						LCDUI_DrawTextUTF((char*)"> !! Error writing test file\n");
						break;
					}
				}
				f_close(&file);
				tc = HAL_GetTick() - tc;
				sprintf(msg, "> Test file writed: %u KB for %u msec\n", (8192*40)/1000, tc);
				LCDUI_DrawTextUTF(msg);

				if (f_open(&file, path, FA_OPEN_EXISTING | FA_READ) == FR_OK)
				{
					uint32_t tc = HAL_GetTick();
					for (uint32_t i = 0; i < 40; i++)
					{
						if (f_read(&file, uibuff, 8192, &wr) != FR_OK || wr != 8192)
						{
							LCDUI_DrawTextUTF((char*)"> !! Error writing test file\n");
							break;
						}
					}
					f_close(&file);
					tc = HAL_GetTick() - tc;
					sprintf(msg, "> Test file readed: %u KB for %u msec\n", (8192*40)/1000, tc);
					LCDUI_DrawTextUTF(msg);
				}
				else
				{
					LCDUI_DrawTextUTF((char*)"> !! Error opening test file\n");
				}
			}
			else
			{
				LCDUI_DrawTextUTF((char*)"> !! Error creating test file\n");
			}

						tstrcpy(path, SpiflPath);
						fres = f_opendir(&dir, path);
						while(1)
						{
							if ((fres = f_readdir(&dir, &finfo)) != FR_OK)
							{
								LCDUI_DrawTextUTF((char*)"> ! Error reading root directory\n");
								break;
							}
							if (finfo.fname[0] == 0)
								break;
							if (finfo.fattrib & AM_DIR)
							{
								cpath[0] = 0;
								UnicodeToUTF8_Str((char*)cpath, finfo.fname, sizeof(cpath));
								sprintf(msg, "  %s    DIR\n", cpath);
								LCDUI_DrawTextUTF(msg);
							}
							else
							{
								cpath[0] = 0;
								UnicodeToANSI_Str((char*)cpath, finfo.fname, sizeof(cpath));
								sprintf(msg, "  %s    %u\n", cpath, finfo.fsize);
								LCDUI_DrawText(msg);
							}
							tstrcpy(path, SpiflPath);
							tstrcat(path, finfo.fname);
//							f_unlink(path);
						}
	}
*/	
//	LCDUI_DrawBitmap(0, 0, (uint8_t*)&TEST_BMP);
	
	USB_HOST_VbusFS(0);

	TOUCH_POINT		tcrd;
	TOUCH_STATES	tstate;
	
	while(1)
	{
		if ((tstate = Touch_GetState()) != TS_FREE)
		{
			if (tstate == TS_SRELEASED || tstate == TS_LRELEASED)
			{
				Touch_SetState(TS_WORKED);
			}
		}
		USB_HOST_Process();
		if (Appli_state_old != Appli_state)
		{
			Appli_state_old = Appli_state;
			switch (Appli_state_old)
			{
				case APPLICATION_IDLE:
					break;

				case APPLICATION_START:
					break;

				case APPLICATION_READY:
					if (f_mount(&UsbFS, UsbPath, 1) == FR_OK)
					{
						// Check for images update on USB disk
						tstrcpy(u_tfname, UsbPath);
						tstrcat_utf(u_tfname, (char*)"alterupd\\images");
						// Found update directory
						if (f_opendir(&dir, u_tfname) == FR_OK)
						{
							while(1)
							{
								if (f_readdir(&dir, &finfo) != FR_OK)
									break;
								if (finfo.fname[0] == 0)
									break;
								if (!(finfo.fattrib & AM_DIR))
								{
									UnicodeToANSI_Str((char*)cfname, finfo.fname, sizeof(cfname));
									if ( (strcmp(cfname, "logo.bmp") == 0 && finfo.fsize < 320000) 
										|| (strcmp(cfname, "scr_main.bmp") == 0 && finfo.fsize < 320000)
										|| (strcmp(cfname, "scr_service.bmp") == 0 && finfo.fsize < 320000) )
									{
										// Found update file, copying it into SPIFlash filesystem
										
										// Open file for read in USB
										tstrcpy(u_tfname, UsbPath);
										tstrcat_utf(u_tfname, (char*)"alterupd\\images\\");
										tstrcat(u_tfname, finfo.fname);
										if (f_open(&ufile, u_tfname, FA_OPEN_EXISTING | FA_READ) != FR_OK)
										{
											LCDUI_DrawTextUTF((char*)"> !! Error opening file in USB filesystem\n");
											break;
										}
												
										// Open file for write in SPIFlash
										tstrcpy(s_tfname, SpiflPath);
										tstrcat(s_tfname, finfo.fname);
										if (f_open(&sfile, s_tfname, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
										{
											LCDUI_DrawTextUTF((char*)"> !! Error creating file in SPIFlash filesystem\n");
											break;
										}
										
										// Copying
										sprintf(msg, "> Copying file \"%s\" ", cfname);
										LCDUI_DrawTextUTF(msg);
										DWORD 		readed = 0, writed = 0;
										uint8_t		iter = 0;
										do
										{
											if (f_read(&ufile, fbuff, sizeof(uibuff), &readed) != FR_OK)
											{
												LCDUI_DrawTextUTF((char*)"> !! Error reading file from USB filesystem\n");
												break;
											}
											if (f_write(&sfile, fbuff, readed, &writed) != FR_OK)
											{
												LCDUI_DrawTextUTF((char*)"> !! Error writing file to SPIFlash filesystem\n");
												break;
											}
											iter++;
											if ((iter % 2) == 0)
												LCDUI_DrawTextUTF((char*)".");
										} while (readed == sizeof(uibuff));
										f_close(&ufile);
										f_close(&sfile);
										LCDUI_DrawTextUTF((char*)" success\n");
										LCDUI_SetCursorCoord(0, -1);
									}
								}
								
							}
							f_close(&ufile);
							f_close(&sfile);
							f_closedir(&dir);
							// Rename directory
							tstrcpy(u_tfname, UsbPath);
							tstrcat_utf(u_tfname, (char*)"alterupd\\images");
							tstrcpy(s_tfname, UsbPath);
							tstrcat_utf(s_tfname, (char*)"alterupd\\images.old");
							f_rename(u_tfname, s_tfname);
						}
					}
					break;

				case APPLICATION_DISCONNECT:
					Appli_state_old = Appli_state = APPLICATION_IDLE;
					break;

			}
		}
	}
	
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
