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
#include "touch_spi.h"

#include "lcd_ui.h"
//#include "wchar.h"
#include "unicode_utils.h"

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


__no_init uint8_t filebuff[8192] @ "CCMRAM";




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

	RTC_Init();
	USB_HOST_VbusFS(1);

	RTC_Enable(&hRTC);

	Touch_Enable();
	Touch_Init();
	
	LCD_Initializtion();
	LCDUI_Init();
	
	LCDUI_SetFont(LCDUI_FONT_H18);
	LCDUI_SetColor(COLOR_GREEN);
	LCDUI_SetBackColor(COLOR_BLACK);
	LCDUI_Clear();
	LCD_BackLight(1);
	
	LCDUI_SetCursorCoord(5, 0);
	LCDUI_DrawTextUTF((char*)"> Starting\n");

	USB_HOST_Init();
	Appli_state_old = Appli_state;
	
	FATFS_Init();
	
	
	LCDUI_DrawTextUTF((char*)"> Ready\n");

	
	USB_HOST_VbusFS(0);

	FRESULT		fres;
	DIR			dir = {0};
	FILINFO		finfo = {0};
	char		msg[512];
	TCHAR		path[256];
	char		cpath[256];
	TCHAR		fname[512];
	FIL			file;
	
	uint16_t	xcrd = 0, ycrd = 0;
	
	while(1)
	{
		xcrd = Touch_GetX();
		ycrd = Touch_GetY();
		if (xcrd > 0 && ycrd > 0)
			LCDUI_DrawPixel(xcrd, ycrd);
		sprintf(msg, "x: %5u", xcrd);
		LCDUI_DrawTextUTF(msg, 0, 400, 5);
		sprintf(msg, "y: %5u", ycrd);
		LCDUI_DrawTextUTF(msg, 0, 400, 25);
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
						strcpy_uni(path, UsbPath);
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
								UnicodeToUTF8_Str((char*)cpath, finfo.fname, sizeof(cpath));
								sprintf(msg, "  %s    %u\n", cpath, finfo.fsize);
								LCDUI_DrawTextUTF(msg);

								
								// Read speed test
								if (strcmp(cpath, "speedtest.bin") == 0)
								{
									LCDUI_DrawTextUTF((char*)"> Found SPEEDTEST file\n");
									strcpy_uni(fname, UsbPath);
									strcat_uni(fname, finfo.fname);
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
								
							}
							
						}
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
