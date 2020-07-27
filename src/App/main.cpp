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


__no_init uint8_t 		filebuff[8192] @ "CCMRAM";
__no_init uint8_t 		uibuff[8192];


FIL			file;
TCHAR		path[512];


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_USB_HOST_Process(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

int main()
{
	HAL_Init();
	SystemClock_Config();
	SCB->VTOR = FLASH_BASE | 0x10000;
	
	GPIO_Init();

	Touch_Init();
	Touch_Enable();
	
	FLASH_SPIInit();
	FLASH_SPIEnable();
	FLASH_SPISetSpeed(SPI_BAUDRATEPRESCALER_2);
	SPIFL_Init();
	
	volatile TGUI_BUTTON btn1;
	btn1.text = "dfghj";
	
	RTC_Init();
	USB_HOST_VbusFS(1);

	RTC_Enable(&hRTC);

	LCD_Initializtion();
	LCDUI_Init();
	
	LCDUI_SetFont(LCDUI_FONT_H18);
	LCDUI_SetColor(COLOR_GREEN);
	LCDUI_SetBackColor(COLOR_BLACK);
	LCDUI_Clear();
	LCD_BackLight(1);
	
	LCDUI_SetCursorCoord(5, 0);
	LCDUI_DrawTextUTF((char*)"> Проверка\n");
	LCDUI_DrawTextUTF((char*)"> Starting...\n");

	USB_HOST_Init();
	Appli_state_old = Appli_state;
	
	FATFS_Init();

	FRESULT		fres;
	DIR			dir = {0};
	FILINFO		finfo = {0};
	char		msg[512];
	TCHAR		path[512];
	char		cpath[256];
	TCHAR		fname[512];
	
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
/*
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
*/
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
	
	LCDUI_DrawTextUTF((char*)"> System ready\n");


//	LCDUI_DrawBitmap(0, 0, (uint8_t*)&TEST_BMP);
	
	USB_HOST_VbusFS(0);

	TOUCH_POINT		tcrd;
	TOUCH_STATES	tstate;
	
	while(1)
	{
		if ((tstate = Touch_GetState()) != TS_FREE)
		{
			if (tstate == TS_SPRESSED || tstate == TS_LPRESSED)
			{
				Touch_GetCoords(&tcrd);
				sprintf(msg, "x: %5u", tcrd.xc);
				LCDUI_DrawTextUTF(msg, 0, 400, 5);
				sprintf(msg, "y: %5u", tcrd.yc);
				LCDUI_DrawTextUTF(msg, 0, 400, 25);
			}
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
					LCDUI_DrawTextUTF((char*)"> USB mass storage connected\n");
					if (f_mount(&UsbFS, UsbPath, 1) != FR_OK)
					{
						LCDUI_DrawTextUTF((char*)"> Error mounting USB drive\n");
					}
					else
					{
						DWORD fclust, fsect, tsect;
						FATFS *fs;
						if (f_getfree(UsbPath, &fclust, &fs) != FR_OK)
						{
							LCDUI_DrawTextUTF((char*)"> ! Error reading USB drive info\n");
							break;
						}
						tsect = (fs->n_fatent - 2) * fs->csize / 1000;
						tsect = tsect * fs->ssize / 1000;
						fsect = fclust * fs->csize / 1000;
						fsect = fsect * fs->ssize / 1000;
						sprintf(msg, "> USB drive size: %u MB total, %u MB free\n", tsect, fsect);
						LCDUI_DrawTextUTF(msg);
						LCDUI_DrawTextUTF((char*)"> --------- ROOT DIRECTORY LIST -------\n");
						tstrcpy(path, UsbPath);
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
								sprintf(msg, "%s", cpath);
								uint16_t cy = LCDUI_GetCurrentCursorY();
								LCDUI_DrawText(msg, 0, 10, -1, 350, cy + LCDUI_GetCurrentFontHeight());
								sprintf(msg, "%u\n", finfo.fsize);
								LCDUI_DrawText(msg, 0, 360, cy);

/*								
								// Read speed test
								if (strcmp(cpath, "speedtest.bin") == 0)
								{
									LCDUI_DrawTextUTF((char*)"> Found SPEEDTEST file\n");
									tstrcpy(fname, UsbPath);
									tstrcat(fname, finfo.fname);
									fres = f_open(&file, fname, FA_READ | FA_OPEN_EXISTING);
									if (fres != FR_OK)
									{
										LCDUI_DrawTextUTF((char*)"> ! Error open SPEEDTEST file\n");
									}
									else
									{
										LCDUI_DrawTextUTF((char*)"> Read speed test begin...\n");
										uint32_t ticks = HAL_GetTick();
										uint32_t readed = 0, treaded = 0, toread = 8000;
										uint32_t cycles = 0;
										do
										{
											f_read(&file, filebuff, toread, &readed);
											treaded += readed;
											cycles++;
										}
										while (readed == toread);
										ticks = HAL_GetTick() - ticks;
										float rate = (float)treaded/1000000.0;
										rate = rate / ((float)ticks/1000.0);
										sprintf(msg, "> Readed %u MB in %u ms - %0.4f MB/s\n", treaded/1000000, ticks, rate);
										LCDUI_DrawTextUTF(msg);
									}

									
									fres = f_close(&file);
								}
*/
							}
							
						}
						LCDUI_SetCursorCoord(0, -1);
						LCDUI_DrawTextUTF((char*)"> -------------------------------------\n");
						
					}
					break;

				case APPLICATION_DISCONNECT:
					Appli_state_old = Appli_state = APPLICATION_IDLE;
					LCDUI_Clear();
					LCDUI_SetCursorCoord(5, 0);
					LCDUI_DrawTextUTF((char*)"> USB disconnected\n");
					LCDUI_DrawTextUTF((char*)"> Ready\n");
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
