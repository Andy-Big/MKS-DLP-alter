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
#include "main.h"
#include "fatfs.h"
#include "rtc.h"
#include "usb_host.h"
#include "gpio.h"
#include "fsmc.h"
#include "usbh_platform.h"
#include "tim.h"

#include "lcd_low.h"
#include "touch.h"
#include "lcd_ui.h"
//#include "wchar.h"
#include "unicode_utils.h"
#include "spi_flash.h"
#include "tgui.h"
#include "motor.h"
#include "z_endstops.h"
#include "config.h"
#include "ext_i2c.h"
#include "ds3231.h"
#include "z_stepper.h"
#include "uvdisplay.h"
#include "eeprom.h"
#include "tgui_defaultfuncs.h"
#include "tgui_movezscreenfuncs.h"
#include "tgui_printscreenfuncs.h"
#include "tgui_screensaverfuncs.h"
#include "printing.h"
#include "tgui_uvtestscreenfuncs.h"




extern USBH_HandleTypeDef		hUsbHostFS;
extern ApplicationTypeDef		Appli_state;
ApplicationTypeDef				Appli_state_old;

extern uint16_t					LCD_WIDTH;
extern uint16_t					LCD_HEIGHT;

extern TCHAR					UsbPath[4];   /* USBH logical drive path */
extern FATFS					UsbFS;    /* File system object for USBH logical drive */
uint8_t							UsbMounted;
extern TCHAR					SpiflPath[4];
extern FATFS					SpiflFS;

extern TOUCH_INFO				touchInfo;

extern PRINT_STATE				prtState;
extern FILES_TYPE				fv_filetype;
extern LAYER_INFO				l_info;

TOUCH_POINT						touchCoord;
TOUCH_STATES					touchState;

uint8_t							tguiTimer;
uint8_t							zHoldTimer;
uint8_t							zDisTimer;
uint8_t							tguiScreenTimer;
uint8_t							timerWorkTimeTimer;

uint8_t							srvMode;
	


__no_init uint8_t 				fbuff[16384] PLACE_TO_CCMRAM;
		
		
__no_init FIL					ufile PLACE_TO_CCMRAM;
__no_init FIL					sfile;
TCHAR							u_tfname[512];
TCHAR							s_tfname[512];
char							cfname[1024];
char							msg[512];
FRESULT							fres;
DIR								dir = {0};
FILINFO							finfo = {0};


void		SystemClock_Config(void);


int main()
{
	for (uint8_t i = 0; i < 8; i++)
	{
		NVIC->ICER[i] = 0xFFFFFFFF;
    	__DSB();
    	__ISB();
		NVIC->ICPR[i] = 0xFFFFFFFF;
	}
	
#ifdef __MKSDLP_BOARD__
	SCB->VTOR = FLASH_BASE | 0x10000;
#endif  // __MKSDLP_BOARD__
#ifdef __CHITU_BOARD__
	SCB->VTOR = FLASH_BASE | 0x20000;
#endif  // __CHITU_BOARD__
	HAL_PWR_DeInit();
	HAL_DeInit();
	HAL_Init();
	SystemClock_Config();
	__ASM volatile("cpsie i");


	
	GPIO_Init();

	// FatFS init
	FATFS_Init();

	SYSTIMER_Init();

	tguiTimer = SYSTIMER_NewCountDown(TIMER_DISABLE);
	zHoldTimer = SYSTIMER_NewCountDown(TIMER_DISABLE);
	zDisTimer = SYSTIMER_NewCountDown(TIMER_DISABLE);
	tguiScreenTimer = SYSTIMER_NewCountDown(TIMER_DISABLE);
	timerWorkTimeTimer = SYSTIMER_NewCountDown(TIMERS_SAVE_PERIOD * 1000);

	
	EEPROM_Init();

	
	
	
	
	
/*
	// clear EPROM and SPI-FLASH	
	FLASH_SPIInit();
	FLASH_SPIEnable();
	FLASH_SPISetSpeed(FST_SPI_CLOCKSPEED);
	SPIFL_Init();
	
	memset(fbuff, 0xFFFFFFFF, 2048);
	EEPROM_WriteMemBuff(0, fbuff, 2048);
	memset(fbuff, 0x00000000, 2048);
	EEPROM_ReadMemBuff(0, fbuff, 2048);

	memset(fbuff, 0xFFFFFFFF, 4096);
	for (uint32_t i = 0; i < 1024; i++)
		SPIFL_WriteBuff(i * 4096, 4096, fbuff);
	while (1);
*/
	
	
	
	
		
	CFG_Init();
	
	// Motherboard fan
	if (cfgConfig.mb_fan_mode == MBFAN_ALWAYS_ON)
		MBFAN_On();
	else
		MBFAN_Off();

	LANG_SetLanguage(0);
	
//	LCD_BackLight(1);
	// LCD init
	LCD_Init();
	LCDUI_Init();
	
	// Touch interface init
	Touch_Init();
	Touch_Enable();

	
	// SPI flash interface init
	FLASH_SPIInit();
	FLASH_SPIEnable();
	FLASH_SPISetSpeed(FST_SPI_CLOCKSPEED);
	// SPI flash logic init
	SPIFL_Init();

	
	if (LCDUI_GetScreenWidth() == 480)
		LCDUI_SetFont(LCDUI_FONT_H18);
	else
		LCDUI_SetFont(LCDUI_FONT_H12);
	LCDUI_SetColor(COLOR_WHITE);
	LCDUI_SetBackColor(COLOR_BLACK);
	LCDUI_Clear();
	TGUI_Init();
	LCD_BackLight(1);

	
	srvMode = 0;

	// SPIFlash filesystem mounting
	if (f_mount(&SpiflFS,  SpiflPath, 1) != FR_OK)
	{
		LCDUI_DrawText((char*)"> !! Internal filesystem error, try to format...\n", 0, 0);
		HAL_Delay(10);
		if (f_mkfs(SpiflPath, 0, fbuff, sizeof(fbuff)) != FR_OK)
		{
			LCDUI_DrawText((char*)"> !! Failed to format internal filesystem\n", 0, 0);
		}
		else
		{
			LCDUI_DrawText((char*)"> Internal filesystem format success\n", 0, 0);
			if (f_mount(&SpiflFS,  SpiflPath, 1) != FR_OK)
			{
				LCDUI_DrawText((char*)"> Internal filesystem mount error\n", 0, 0);
			}
		}
		srvMode = 1;
	}
	
	// wait fot touch go ready
	HAL_Delay(100);
	// If touch pressed - service booting with update
	if (Touch_IsPressed())
	{
		TOUCH_POINT tp;
		Touch_GetCurrentCoordsRaw(&tp);
		if ( tp.xc > 430 && tp.yc < 50)
		{
			srvMode = 1;
		}
		else
		{
			cfgConfig.touch_cal[0] = cfgConfig.touch_cal[4] = 1;
			cfgConfig.touch_cal[1] = cfgConfig.touch_cal[2] = cfgConfig.touch_cal[3] = cfgConfig.touch_cal[5] = 0;
			CFG_SaveConfig();
			srvMode = 2;
		}
	}
	
	// ------------------ Service mode -------------------
	if (srvMode)
	{
		LCDUI_SetColor(COLOR_GREEN);
		LCDUI_DrawText((char*)"> Service mode\n", 0, 0);
		if (srvMode == 2)
		{
			srvMode = 1;
			LCDUI_DrawText((char*)"> Touch calibrating are reset to default.\n", 0, 0);
		}


		FATFS *fs;
		DWORD fre_clust, fre_sect, tot_sect = 0x1234;
		asm("rbit %0, %1" : "=r" (tot_sect) : "r" (tot_sect) );
		tot_sect++;

		if (f_getfree(SpiflPath, &fre_clust, &fs) != FR_OK)
		{
			LCDUI_DrawText((char*)"> Can't get free space of internal filesystem\n", 0, 0);
		}
		else
		{
			tot_sect = (fs->n_fatent - 2) * fs->csize * fs->ssize / 1024;
			fre_sect = fre_clust * fs->csize * fs->ssize / 1024;
			if (fre_sect == 0)
			{
				LCDUI_DrawText((char*)"> !! Internal filesystem error, try to format...\n", 0, 0);
				if (f_mkfs(SpiflPath, 0, fbuff, sizeof(fbuff)) != FR_OK)
				{
					LCDUI_DrawText((char*)"> !! Failed to format internal filesystem\n", 0, 0);
				}
				else
				{
					LCDUI_DrawText((char*)"> Internal filesystem format success\n", 0, 0);
					f_getfree(SpiflPath, &fre_clust, &fs);
					tot_sect = (fs->n_fatent - 2) * fs->csize * fs->ssize / 1024;
					fre_sect = fre_clust * fs->csize * fs->ssize / 1024;
				}
			}
				

			sprintf(msg, "> %u KB total internal filesystem,  %u KB available.\n", tot_sect, fre_sect);
			LCDUI_DrawText(msg, 0, 0);
		}
	}
	else
	{
		// Draw initial logo
		TGUI_DrawLogo();
		sprintf(msg, "v %d.%02d", (FW_VERSION >> 8) & 0xFF, FW_VERSION & 0xFF);
		LCDUI_SetColor(LCDUI_RGB(0x47C7DA));
		LCDUI_SetFont(LCDUI_FONT_H18);
		if (LCDUI_GetScreenWidth() == 480)
		{
			LCDUI_DrawText(msg, LCDUI_TEXT_TRANSBACK | LCDUI_TEXT_ALIGN_RIGHT, 260, 192, 356, -1);
		}
		else
		{
			LCDUI_DrawText(msg, LCDUI_TEXT_TRANSBACK | LCDUI_TEXT_ALIGN_RIGHT, 182, 158, 282, -1);
		}
		LCDUI_SetColor(LCDUI_RGB(0xBCBCBC));
		if (LCDUI_GetScreenWidth() == 480)
		{
			LCDUI_SetFont(LCDUI_FONT_H18BOLD);
			LCDUI_DrawText((char*)"PERIFERY INITIALIZATION...", LCDUI_TEXT_TRANSBACK | LCDUI_TEXT_ALIGN_CENTER, 10, 276, 470, -1);
		}
		else
		{
			LCDUI_SetFont(LCDUI_FONT_H18);
			LCDUI_DrawText((char*)"PERIFERY INITIALIZATION...", LCDUI_TEXT_TRANSBACK | LCDUI_TEXT_ALIGN_CENTER, 0, 210, 319, -1);
		}
		HAL_Delay(300);
	}
	
	// Clear CCRAM
	uint32_t	*ccr = (uint32_t*)0x10000000;
	for (uint32_t i = 0; i < 65536/4; i++)
	{
		*ccr = 0;
		ccr++;
	}
		
	RTC_Init();
	RTC_Enable(&hRTC);
	
#ifdef __MKSDLP_BOARD__
	// external clock
	while (cfgConfig.use_ext_clock)
	{
		uint8_t				ds_state;
		DATETIME_STRUCT 	dtime;
		memset(&dtime, 0, sizeof(DATETIME_STRUCT));

		// disable EEPROM pins
		EEPROM_Deinit();
		if (!srvMode)
		{
			_tgui_DrawFileCimg(FNAME_LOGO_STRING_BG, 10, 275);
			LCDUI_DrawText((char*)"EXTERNAL CLOCK INIT...", LCDUI_TEXT_TRANSBACK | LCDUI_TEXT_ALIGN_CENTER, 10, 276, 470, -1);
		}
		if ((ds_state = DS3231_Init()) == 0)
		{
			if (srvMode)
			{
				LCDUI_DrawText((char*)"> External clock not found\n");
			}
			else
			{
				_tgui_DrawFileCimg(FNAME_LOGO_STRING_BG, 10, 275);
				LCDUI_DrawText((char*)"EXTERNAL CLOCK NOT FOUND", LCDUI_TEXT_TRANSBACK | LCDUI_TEXT_ALIGN_CENTER, 10, 276, 470, -1);
				HAL_Delay(1000);
			}
			break;
		}
		if (!srvMode)
		{
			_tgui_DrawFileCimg(FNAME_LOGO_STRING_BG, 10, 275);
			LCDUI_DrawText((char*)"DATA READING FROM EXTERNAL CLOCK...", LCDUI_TEXT_TRANSBACK | LCDUI_TEXT_ALIGN_CENTER, 10, 276, 470, -1);
		}
		// if clock has stoped
		if (ds_state == 2)
		{
			if (DS3231_StartOsc() == 0)
			{
				if (srvMode)
				{
					LCDUI_DrawText((char*)"> External clock starting error\n");
				}
				else
				{
					_tgui_DrawFileCimg(FNAME_LOGO_STRING_BG, 10, 275);
					LCDUI_DrawText((char*)"EXTERNAL CLOCK ERROR...", LCDUI_TEXT_TRANSBACK | LCDUI_TEXT_ALIGN_CENTER, 10, 276, 470, -1);
					HAL_Delay(1000);
				}
				break;
			}
		}

		// read time
		if (DS3231_GetTime(&dtime) == 0)
		{
			if (srvMode)
			{
				LCDUI_DrawText((char*)"> External clock reading error\n");
			}
			else
			{
				_tgui_DrawFileCimg(FNAME_LOGO_STRING_BG, 10, 275);
				LCDUI_DrawText((char*)"EXTERNAL CLOCK READ TIME ERROR...", LCDUI_TEXT_TRANSBACK | LCDUI_TEXT_ALIGN_CENTER, 10, 276, 470, -1);
			}
			HAL_Delay(1000);
			break;
		}

		// set time in internal RTC
		DTIME_SetCurrentDatetime(&dtime);

		// disable external I2C pins
		DS3231_Deinit();
		// enable EEPROM pins
		EEPROM_Init();
		if (ds_state == 2)
		{
			if (srvMode)
			{
				LCDUI_DrawText((char*)"> External clock may be incorrect time\n");
			}
			else
			{
				_tgui_DrawFileCimg(FNAME_LOGO_STRING_BG, 10, 275);
				LCDUI_DrawText((char*)"EXTERNAL CLOCK MAY BE INCORRECT TIME", LCDUI_TEXT_TRANSBACK | LCDUI_TEXT_ALIGN_CENTER, 10, 276, 470, -1);
			}
		}
		else
		{
			if (srvMode)
			{
				LCDUI_DrawText((char*)"> External clock readed success\n");
			}
			else
			{
				_tgui_DrawFileCimg(FNAME_LOGO_STRING_BG, 10, 275);
				LCDUI_DrawText((char*)"EXTERNAL CLOCK READED SUCCESS", LCDUI_TEXT_TRANSBACK | LCDUI_TEXT_ALIGN_CENTER, 10, 276, 470, -1);
			}
		}
		HAL_Delay(1000);
		break;
	}
#endif  // __MKSDLP_BOARD__
	systemInfo.timerWorkTimeSecs = DTIME_GetCurrentUnixtime();
	
	LANG_SetLanguage(cfgConfig.language);

	// ZMotor
	ZMOTOR_Init();
	ZMOTOR_MotorDisable();
	SYSTIMER_SetCountDown(zHoldTimer, TIMER_DISABLE);
	SYSTIMER_SetCountDown(zDisTimer, TIMER_DISABLE);
	SYSTIMER_SetCountDown(tguiScreenTimer, cfgConfig.screensaver_time);
	
#ifdef __MKSDLP_BOARD__
	_cpld_bank2disp_enable(CLEAN_USED_BANK,0,0);
#endif  // __MKSDLP_BOARD__
#ifdef __CHITU_BOARD__
  #ifdef __CHITU_LV3__
  #endif  // __CHITU_LV3__
#endif  // __CHITU_BOARD__

	if (srvMode == 0)
	{
		_tgui_DrawFileCimg(FNAME_LOGO_STRING_BG, 10, 275);
		LCDUI_DrawText((char*)"UV DISPLAY CONFIGURING...", LCDUI_TEXT_TRANSBACK | LCDUI_TEXT_ALIGN_CENTER, 10, 276, 470, -1);
		UVD_Init();

		_tgui_DrawFileCimg(FNAME_LOGO_STRING_BG, 10, 275);
		LCDUI_DrawText((char*)"UV DISPLAY TO SLEEP...", LCDUI_TEXT_TRANSBACK | LCDUI_TEXT_ALIGN_CENTER, 10, 276, 470, -1);
		UVD_Sleep();

#ifdef __MKSDLP_BOARD__
		_cpld_bank2disp_enable(CLEAN_USED_BANK,0,0);
		_tgui_DrawFileCimg(FNAME_LOGO_STRING_BG, 10, 275);
		LCDUI_DrawText((char*)"UV DISPLAY PREPARING...", LCDUI_TEXT_TRANSBACK | LCDUI_TEXT_ALIGN_CENTER, 10, 276, 470, -1);
		_tgui_UVTestReadImage(0);
#endif  // __MKSDLP_BOARD__
	}
	
	// Disable USB power line
	if (srvMode == 0)
	{
		_tgui_DrawFileCimg(FNAME_LOGO_STRING_BG, 10, 275);
		LCDUI_DrawText((char*)"USB CONFIGURING...", LCDUI_TEXT_TRANSBACK | LCDUI_TEXT_ALIGN_CENTER, 10, 276, 470, -1);
	}
	USB_HOST_VbusFS(1);

	USB_HOST_Init();
	Appli_state_old = Appli_state;

	
	
	// Enable USB power line
	USB_HOST_VbusFS(0);

	// ------------------ Service mode -------------------
	if (!srvMode)
	{
		TGUI_ForceRepaint();
	}
	
	SYSTIMER_SetCountDown(tguiTimer, 25);

	while(1)
	{
		// save total printer on time
		if (SYSTIMER_GetCountDown(timerWorkTimeTimer) == 0 && systemInfo.print_is_printing == 0)
		{
			SYSTIMER_SetCountDown(timerWorkTimeTimer, TIMERS_SAVE_PERIOD * 1000);
			cfgTimers.total_on_time += DTIME_GetCurrentUnixtime() - systemInfo.timerWorkTimeSecs;
			systemInfo.timerWorkTimeSecs = DTIME_GetCurrentUnixtime();
			CFG_SaveTimers();
		}

		if (srvMode)
		{
			USB_HOST_Process();
			if (Appli_state_old != Appli_state)
			{
				Appli_state_old = Appli_state;
				switch (Appli_state)
				{
					case APPLICATION_IDLE:
						break;

					case APPLICATION_START:
						break;

					case APPLICATION_READY:
						if (f_mount(&UsbFS, UsbPath, 1) == FR_OK)
						{
							uint16_t		curr_x, curr_y;
							uint32_t		progress_count;
							char			progress_ind[4] = {'-', '\\', '|', '/'};
							UsbMounted = 1;
							LCDUI_DrawText((char*)"> +USB storage mounted\n", 0, 0);
							char *ext;

							// check for deleting all files
							tstrcpy(u_tfname, UsbPath);
							tstrcat_utf(u_tfname, SDIR_UPD);
							tstrcat_utf(u_tfname, (char*)"\\formatfilesystem");
							if (f_stat(u_tfname, NULL) == FR_OK)
							{
								LCDUI_DrawText((char*)"> Filesystem format command found\n", 0, 0);
								LCDUI_DrawText((char*)"> Formating internal filesystem... ", 0, 0);
								f_mount(NULL,  SpiflPath, 1);
								if (f_mkfs(SpiflPath, 0, fbuff, sizeof(fbuff)) != FR_OK)
								{
									LCDUI_DrawText((char*)"> !! Failed to format internal filesystem\n", 0, 0);
								}
								else
								{
									LCDUI_DrawText((char*)" success\n");
								}
								if (f_mount(&SpiflFS,  SpiflPath, 1) != FR_OK)
								{
									LCDUI_DrawText((char*)"> !! Internal filesystem error, try to format...\n", 0, 0);
									break;
								}
/*
								// rename file-command
								tstrcpy(s_tfname, UsbPath);
								tstrcat_utf(s_tfname, SDIR_UPD);
								tstrcat_utf(s_tfname, (char*)"/formatfilesystem.old");
								f_rename(u_tfname, s_tfname);
*/
							}

							// ---------- Check for UI update on USB disk -----------
							tstrcpy(u_tfname, UsbPath);
							tstrcat_utf(u_tfname, SDIR_UPD);
							tstrcat_utf(u_tfname, (char*)"\\");
							tstrcat_utf(u_tfname, SDIR_UI);
							uint8_t		files_copyed = 0;
							if (f_opendir(&dir, u_tfname) == FR_OK)
							{
								LCDUI_DrawText((char*)"> Firmware UI directory found\n", 0, 0);
								f_closedir(&dir);
								// ---------- 480x320 -----------
								tstrcat_utf(u_tfname, (char*)"\\");
								tstrcat_utf(u_tfname, SDIR_UI_480);
								if (f_opendir(&dir, u_tfname) == FR_OK)
								{
									LCDUI_DrawText((char*)"> Found 480x320 UI\n", 0, 0);
									// creating directory in internal flash
									tstrcpy(s_tfname, SpiflPath);
									tstrcat_utf(s_tfname, SDIR_UI);
									fres = f_mkdir(s_tfname);
									if (fres != FR_OK && fres != FR_EXIST)
									{
										LCDUI_DrawText((char*)">  !! Error creating UI directory in internal filesystem\n", 0, 0);
										HAL_Delay(500);
										break;
									}
									tstrcat_utf(s_tfname, (char*)"\\");
									tstrcat_utf(s_tfname, SDIR_UI_480);
									fres = f_mkdir(s_tfname);
									if (fres != FR_OK && fres != FR_EXIST)
									{
										LCDUI_DrawText((char*)">  !! Error creating 480x320 directory in internal filesystem\n", 0, 0);
										HAL_Delay(500);
										break;
									}
									while(1)
									{
										if (f_readdir(&dir, &finfo) != FR_OK)
											break;
										if (finfo.fname[0] == 0)
											break;
										
										if (!(finfo.fattrib & AM_DIR))
										{
											UnicodeToANSI_Str((char*)cfname, finfo.fname, sizeof(cfname));
											ext = FATFS_GetFileExtension(cfname);
											if ((strcmp(ext, (char*)"cimg") == 0 && finfo.fsize < 500000))
											{
												// Found update file, copying it into SPIFlash filesystem
												files_copyed = 1;
												if (LCDUI_GetCurrentCursorY() > (LCD_HEIGHT - LCDUI_GetCurrentFontHeight()))
												{
													LCDUI_Clear();
												}
												// Open file for read in USB
												tstrcpy(u_tfname, UsbPath);
												tstrcat_utf(u_tfname, SDIR_UPD);
												tstrcat_utf(u_tfname, (char*)"\\");
												tstrcat_utf(u_tfname, SDIR_UI);
												tstrcat_utf(u_tfname, (char*)"\\");
												tstrcat_utf(u_tfname, SDIR_UI_480);
												tstrcat_utf(u_tfname, (char*)"\\");
												tstrcat(u_tfname, finfo.fname);
												if (f_open(&ufile, u_tfname, FA_OPEN_EXISTING | FA_READ) != FR_OK)
												{
													LCDUI_DrawText((char*)">  !! Error opening file in USB filesystem\n", 0, 0);
													HAL_Delay(500);
													continue;
												}
														
												// Open file for write in SPIFlash
												tstrcpy(s_tfname, SpiflPath);
												tstrcat_utf(s_tfname, SDIR_UI);
												tstrcat_utf(s_tfname, (char*)"\\");
												tstrcat_utf(s_tfname, SDIR_UI_480);
												tstrcat_utf(s_tfname, (char*)"\\");
												tstrcat(s_tfname, finfo.fname);
												if (f_open(&sfile, s_tfname, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
												{
													sprintf(msg, ">  !! Error creating file <%s> in internal filesystem\n", cfname);
													LCDUI_DrawText(msg, 0, 0);
													f_close(&ufile);
													HAL_Delay(500);
													continue;
												}
												
												// Copying
												sprintf(msg, ">  Copying file <%s> ", cfname);
												LCDUI_DrawText(msg, 0, 0);
												curr_x = LCDUI_GetCurrentCursorX();
												curr_y = LCDUI_GetCurrentCursorY();
												progress_count = 1;
												LCDUI_DrawChar(progress_ind[0], 0, curr_x, curr_y);
												UINT 		readed = 0, writed = 0;
												uint8_t		iter = 0;
												do
												{
													if (f_read(&ufile, fbuff, sizeof(fbuff), &readed) != FR_OK)
													{
														sprintf(msg, "\n>  !! Error reading file <%s> from USB filesystem\n", cfname);
														LCDUI_DrawText(msg, 0, 0);
														f_close(&ufile);
														f_close(&sfile);
														HAL_Delay(500);
														continue;
													}
													if (f_write(&sfile, fbuff, readed, &writed) != FR_OK)
													{
														sprintf(msg, "\n>  !! Error writing file <%s> to internal filesystem\n", cfname);
														LCDUI_DrawText(msg, 0, 0);
														f_close(&ufile);
														f_close(&sfile);
														HAL_Delay(500);
														continue;
													}
													iter++;
													LCDUI_DrawChar(progress_ind[progress_count], 0, curr_x, curr_y);
													if (++progress_count >= sizeof(progress_ind))
														progress_count = 0;
												} while (readed == sizeof(fbuff));
												f_close(&ufile);
												f_close(&sfile);
												LCDUI_DrawText((char*)"   \n", 0, curr_x, curr_y);
											}
										}
										
									}
									f_close(&ufile);
									f_close(&sfile);
									f_closedir(&dir);
								}
								// ---------- 320x240 -----------
								tstrcpy(u_tfname, UsbPath);
								tstrcat_utf(u_tfname, SDIR_UPD);
								tstrcat_utf(u_tfname, (char*)"\\");
								tstrcat_utf(u_tfname, SDIR_UI);
								tstrcat_utf(u_tfname, (char*)"\\");
								tstrcat_utf(u_tfname, SDIR_UI_320);
								if (f_opendir(&dir, u_tfname) == FR_OK)
								{
									LCDUI_DrawText((char*)"> Found 320x240 UI\n", 0, 0);
									// creating directory in internal flash
									tstrcpy(s_tfname, SpiflPath);
									tstrcat_utf(s_tfname, SDIR_UI);
									fres = f_mkdir(s_tfname);
									if (fres != FR_OK && fres != FR_EXIST)
									{
										LCDUI_DrawText((char*)">  !! Error creating UI directory in internal filesystem\n", 0, 0);
										HAL_Delay(500);
										break;
									}
									tstrcat_utf(s_tfname, (char*)"\\");
									tstrcat_utf(s_tfname, SDIR_UI_320);
									fres = f_mkdir(s_tfname);
									if (fres != FR_OK && fres != FR_EXIST)
									{
										LCDUI_DrawText((char*)">  !! Error creating 320x240 directory in internal filesystem\n", 0, 0);
										HAL_Delay(500);
										break;
									}
									while(1)
									{
										if (f_readdir(&dir, &finfo) != FR_OK)
											break;
										if (finfo.fname[0] == 0)
											break;
										
										if (!(finfo.fattrib & AM_DIR))
										{
											UnicodeToANSI_Str((char*)cfname, finfo.fname, sizeof(cfname));
											ext = FATFS_GetFileExtension(cfname);
											if ((strcmp(ext, (char*)"cimg") == 0 && finfo.fsize < 500000))
											{
												// Found update file, copying it into SPIFlash filesystem
												files_copyed = 1;
												if (LCDUI_GetCurrentCursorY() > (LCD_HEIGHT - LCDUI_GetCurrentFontHeight()))
												{
													LCDUI_Clear();
												}
												// Open file for read in USB
												tstrcpy(u_tfname, UsbPath);
												tstrcat_utf(u_tfname, SDIR_UPD);
												tstrcat_utf(u_tfname, (char*)"\\");
												tstrcat_utf(u_tfname, SDIR_UI);
												tstrcat_utf(u_tfname, (char*)"\\");
												tstrcat_utf(u_tfname, SDIR_UI_320);
												tstrcat_utf(u_tfname, (char*)"\\");
												tstrcat(u_tfname, finfo.fname);
												if (f_open(&ufile, u_tfname, FA_OPEN_EXISTING | FA_READ) != FR_OK)
												{
													LCDUI_DrawText((char*)">  !! Error opening file in USB filesystem\n", 0, 0);
													HAL_Delay(500);
													continue;
												}
														
												// Open file for write in SPIFlash
												tstrcpy(s_tfname, SpiflPath);
												tstrcat_utf(s_tfname, SDIR_UI);
												tstrcat_utf(s_tfname, (char*)"\\");
												tstrcat_utf(s_tfname, SDIR_UI_320);
												tstrcat_utf(s_tfname, (char*)"\\");
												tstrcat(s_tfname, finfo.fname);
												if (f_open(&sfile, s_tfname, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
												{
													sprintf(msg, ">  !! Error creating file <%s> in internal filesystem\n", cfname);
													LCDUI_DrawText(msg, 0, 0);
													f_close(&ufile);
													HAL_Delay(500);
													continue;
												}
												
												// Copying
												sprintf(msg, ">  Copying file <%s> ", cfname);
												LCDUI_DrawText(msg, 0, 0);
												curr_x = LCDUI_GetCurrentCursorX();
												curr_y = LCDUI_GetCurrentCursorY();
												progress_count = 1;
												LCDUI_DrawChar(progress_ind[0], 0, curr_x, curr_y);
												UINT 		readed = 0, writed = 0;
												uint8_t		iter = 0;
												do
												{
													if (f_read(&ufile, fbuff, sizeof(fbuff), &readed) != FR_OK)
													{
														sprintf(msg, "\n>  !! Error reading file <%s> from USB filesystem\n", cfname);
														LCDUI_DrawText(msg, 0, 0);
														f_close(&ufile);
														f_close(&sfile);
														HAL_Delay(500);
														continue;
													}
													if (f_write(&sfile, fbuff, readed, &writed) != FR_OK)
													{
														sprintf(msg, "\n>  !! Error writing file <%s> to internal filesystem\n", cfname);
														LCDUI_DrawText(msg, 0, 0);
														f_close(&ufile);
														f_close(&sfile);
														HAL_Delay(500);
														continue;
													}
													iter++;
													LCDUI_DrawChar(progress_ind[progress_count], 0, curr_x, curr_y);
													if (++progress_count >= sizeof(progress_ind))
														progress_count = 0;
												} while (readed == sizeof(fbuff));
												f_close(&ufile);
												f_close(&sfile);
												LCDUI_DrawText((char*)"   \n", 0, curr_x, curr_y);
											}
										}
										
									}
									f_close(&ufile);
									f_close(&sfile);
									f_closedir(&dir);
								}
							}
							// ---------- Check for FILES update on USB disk -----------
							tstrcpy(u_tfname, UsbPath);
							tstrcat_utf(u_tfname, SDIR_UPD);
							tstrcat_utf(u_tfname, (char*)"\\");
							tstrcat_utf(u_tfname, SDIR_FILES);
							if (f_opendir(&dir, u_tfname) == FR_OK)
							{
								LCDUI_DrawText((char*)"> Firmware files directory found\n", 0, 0);
								// creating directory in internal flash
								tstrcpy(s_tfname, SpiflPath);
								tstrcat_utf(s_tfname, SDIR_FILES);
								fres = f_mkdir(s_tfname);
								if (fres != FR_OK && fres != FR_EXIST)
								{
									LCDUI_DrawText((char*)">  !! Error creating FILES directory in internal filesystem\n", 0, 0);
									HAL_Delay(500);
									break;
								}
								while(1)
								{
									if (f_readdir(&dir, &finfo) != FR_OK)
										break;
									if (finfo.fname[0] == 0)
										break;
									
									if (!(finfo.fattrib & AM_DIR))
									{
										UnicodeToANSI_Str((char*)cfname, finfo.fname, sizeof(cfname));
										ext = FATFS_GetFileExtension(cfname);
										if ((strcmp(ext, (char*)"cimg") == 0 && finfo.fsize < 500000))
										{
											// Found update file, copying it into SPIFlash filesystem
											files_copyed = 1;
											if (LCDUI_GetCurrentCursorY() > (LCD_HEIGHT - LCDUI_GetCurrentFontHeight()))
											{
												LCDUI_Clear();
											}
											// Open file for read in USB
											tstrcpy(u_tfname, UsbPath);
											tstrcat_utf(u_tfname, SDIR_UPD);
											tstrcat_utf(u_tfname, (char*)"\\");
											tstrcat_utf(u_tfname, SDIR_FILES);
											tstrcat_utf(u_tfname, (char*)"\\");
											tstrcat(u_tfname, finfo.fname);
											if (f_open(&ufile, u_tfname, FA_OPEN_EXISTING | FA_READ) != FR_OK)
											{
												LCDUI_DrawText((char*)">  !! Error opening file in USB filesystem\n", 0, 0);
												HAL_Delay(500);
												continue;
											}
													
											// Open file for write in SPIFlash
											tstrcpy(s_tfname, SpiflPath);
											tstrcat_utf(s_tfname, SDIR_FILES);
											tstrcat_utf(s_tfname, (char*)"\\");
											tstrcat(s_tfname, finfo.fname);
											if (f_open(&sfile, s_tfname, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
											{
												sprintf(msg, ">  !! Error creating file <%s> in internal filesystem\n", cfname);
												LCDUI_DrawText(msg, 0, 0);
												f_close(&ufile);
												HAL_Delay(500);
												continue;
											}
											
											// Copying
											sprintf(msg, ">  Copying file <%s> ", cfname);
											LCDUI_DrawText(msg, 0, 0);
											curr_x = LCDUI_GetCurrentCursorX();
											curr_y = LCDUI_GetCurrentCursorY();
											progress_count = 1;
											LCDUI_DrawChar(progress_ind[0], 0, curr_x, curr_y);
											UINT 		readed = 0, writed = 0;
											uint8_t		iter = 0;
											do
											{
												if (f_read(&ufile, fbuff, sizeof(fbuff), &readed) != FR_OK)
												{
													sprintf(msg, "\n>  !! Error reading file <%s> from USB filesystem\n", cfname);
													LCDUI_DrawText(msg, 0, 0);
													f_close(&ufile);
													f_close(&sfile);
													HAL_Delay(500);
													continue;
												}
												if (f_write(&sfile, fbuff, readed, &writed) != FR_OK)
												{
													sprintf(msg, "\n>  !! Error writing file <%s> to internal filesystem\n", cfname);
													LCDUI_DrawText(msg, 0, 0);
													f_close(&ufile);
													f_close(&sfile);
													HAL_Delay(500);
													continue;
												}
												iter++;
												LCDUI_DrawChar(progress_ind[progress_count], 0, curr_x, curr_y);
												if (++progress_count >= sizeof(progress_ind))
													progress_count = 0;
											} while (readed == sizeof(fbuff));
											f_close(&ufile);
											f_close(&sfile);
											LCDUI_DrawText((char*)"   \n", 0, curr_x, curr_y);
										}
									}
									
								}
								f_close(&ufile);
								f_close(&sfile);
								f_closedir(&dir);
							}
							if (files_copyed > 0)
							{
								LCDUI_DrawText((char*)"> Files copy completed. \n", 0, 0);
							}
						}
						break;

					case APPLICATION_DISCONNECT:
						f_mount(NULL, UsbPath, 1);
						memset(&UsbFS, 0, sizeof(FATFS));
						UsbMounted = 0;
						if (srvMode)
						{
							LCDUI_DrawText((char*)"> -USB storage removed\n", 0, 0);
						}
						break;

				}
			}
			continue;
		}
		
		
		if (SYSTIMER_GetCountDown(tguiTimer) == 0)
		{
			SYSTIMER_SetCountDown(tguiTimer, 25);
			TGUI_Process();
		}
		if (SYSTIMER_GetCountDown(zHoldTimer) == 0)
		{
			if (ZMOTOR_IsMotorEnabled())
			{
				ZMOTOR_SetHoldCurrent();
				SYSTIMER_SetCountDown(zHoldTimer, TIMER_DISABLE);
			}
		}
		if (SYSTIMER_GetCountDown(zDisTimer) == 0)
		{
			ZMOTOR_MotorDisable();
			SYSTIMER_SetCountDown(zDisTimer, TIMER_DISABLE);
			systemInfo.position_known = 0;
		}
		if (SYSTIMER_GetCountDown(tguiScreenTimer) == 0)
		{
			SYSTIMER_SetCountDown(tguiScreenTimer, TIMER_DISABLE);
			TGUI_ScreenSaverShow();
		}

		switch (systemInfo.printer_state)
		{
			case PST_FREEMOVING_UP:
			case PST_FREEMOVING_DOWN:
				if (ZMOTOR_IsMoving() == 0)
				{
					// min endstop
					if (ZMOTOR_GetEndstopState() != 0)
					{
						systemInfo.target_position = ZMOTOR_GetCurrentPosition();
						ZMOTOR_SetPosition(systemInfo.target_position);
					}
					systemInfo.printer_state = PST_IDLE;
					SYSTIMER_SetCountDown(zHoldTimer, cfgMotor.hold_time);
					SYSTIMER_SetCountDown(zDisTimer, cfgMotor.off_time);
				}
				break;

			case PST_HOMING_PREUP:
				if (ZMOTOR_IsMoving() == 0)
				{
					// endstop not opened after moving
					if (ZMOTOR_GetEndstopState() != 0)
					{
						ZMOTOR_SetPosition(0);
						systemInfo.target_position = 0;
						systemInfo.printer_state = PST_IDLE;
						SYSTIMER_SetCountDown(zHoldTimer, cfgMotor.hold_time);
						SYSTIMER_SetCountDown(zDisTimer, cfgMotor.off_time);
						if (systemInfo.print_is_homing)
						{
							systemInfo.print_is_homing = 0;
							TGUI_MessageBoxOk(LANG_GetString(LSTR_ERROR), LANG_GetString(LSTR_MSG_ENDSTOP_ERROR), TGUI_PrintScreenExit);
							BUZZ_TimerOn(cfgConfig.buzzer_msg);
						}
						else
						{
							TGUI_MessageBoxOk(LANG_GetString(LSTR_ERROR), LANG_GetString(LSTR_MSG_ENDSTOP_ERROR));
							BUZZ_TimerOn(cfgConfig.buzzer_msg);
						}
					}
					else
					{
						if (cfgMotor.home_dir == -1)
							systemInfo.target_position = cfgMotor.max_pos * (-1.0) - 2;
						else
							systemInfo.target_position = cfgMotor.max_pos + 2;
						ZMOTOR_MoveAbsolute(systemInfo.target_position, cfgMotor.homing_feedrate_fast);
						systemInfo.printer_state = PST_HOMING_FAST;
						if (systemInfo.print_is_homing)
						{
//							UVD_Init();
						}
					}
				}
				break;

			case PST_HOMING_FAST:
				if (ZMOTOR_IsMoving() == 0)
				{
					if (cfgMotor.home_dir == -1 && ZMOTOR_GetEndstopState() & (1 << Z_MIN))
					{
						// moved to min endstop
						ZMOTOR_SetPosition(cfgMotor.home_pos);
						systemInfo.target_position = cfgMotor.home_pos + 3;
						ZMOTOR_MoveAbsolute(systemInfo.target_position, cfgMotor.homing_feedrate_fast);
						systemInfo.printer_state = PST_HOMING_UP;
						if (systemInfo.print_is_homing)
							UVD_Wakeup();
					}
					else if (cfgMotor.home_dir == 1 && ZMOTOR_GetEndstopState() & (1 << Z_MAX))
					{
						// moved to max endstop
						ZMOTOR_SetPosition(cfgMotor.home_pos);
						systemInfo.target_position = cfgMotor.home_pos - 3;
						ZMOTOR_MoveAbsolute(systemInfo.target_position, cfgMotor.homing_feedrate_fast);
						systemInfo.printer_state = PST_HOMING_UP;
						if (systemInfo.print_is_homing)
							UVD_Wakeup();
					}
					// endstop not reached, error
					else
					{
						ZMOTOR_SetPosition(0);
						systemInfo.target_position = 0;
						systemInfo.printer_state = PST_IDLE;
						SYSTIMER_SetCountDown(zHoldTimer, cfgMotor.hold_time);
						SYSTIMER_SetCountDown(zDisTimer, cfgMotor.off_time);
						if (systemInfo.print_is_homing)
						{
							systemInfo.print_is_homing = 0;
							TGUI_MessageBoxOk(LANG_GetString(LSTR_ERROR), LANG_GetString(LSTR_MSG_ENDSTOP_ERROR), TGUI_PrintScreenExit);
							BUZZ_TimerOn(cfgConfig.buzzer_msg);
						}
						else
						{
							TGUI_MessageBoxOk(LANG_GetString(LSTR_ERROR), LANG_GetString(LSTR_MSG_ENDSTOP_ERROR));
							BUZZ_TimerOn(cfgConfig.buzzer_msg);
						}
					}
				}
				break;

			case PST_HOMING_UP:
				if (ZMOTOR_IsMoving() == 0)
				{
					// endstop not opened after moving
					if (ZMOTOR_GetEndstopState() != 0)
					{
						ZMOTOR_SetPosition(0);
						systemInfo.target_position = 0;
						systemInfo.printer_state = PST_IDLE;
						SYSTIMER_SetCountDown(zHoldTimer, cfgMotor.hold_time);
						SYSTIMER_SetCountDown(zDisTimer, cfgMotor.off_time);
						if (systemInfo.print_is_homing)
						{
							systemInfo.print_is_homing = 0;
							TGUI_MessageBoxOk(LANG_GetString(LSTR_ERROR), LANG_GetString(LSTR_MSG_ENDSTOP_ERROR), TGUI_PrintScreenExit);
							BUZZ_TimerOn(cfgConfig.buzzer_msg);
						}
						else
						{
							TGUI_MessageBoxOk(LANG_GetString(LSTR_ERROR), LANG_GetString(LSTR_MSG_ENDSTOP_ERROR));
							BUZZ_TimerOn(cfgConfig.buzzer_msg);
						}
					}
					else
					{
						if (cfgMotor.home_dir == -1)
						{
							systemInfo.target_position = -1;
							ZMOTOR_MoveAbsolute(systemInfo.target_position, cfgMotor.homing_feedrate_slow);
							systemInfo.printer_state = PST_HOMING_SLOW;
						}
						else
						{
							systemInfo.target_position = 1;
							ZMOTOR_MoveAbsolute(systemInfo.target_position, cfgMotor.homing_feedrate_slow);
							systemInfo.printer_state = PST_HOMING_SLOW;
						}
					}
				}
				break;

			case PST_HOMING_SLOW:
				if (ZMOTOR_IsMoving() == 0)
				{
					if (ZMOTOR_GetEndstopState() != 0)
					{
						// moved to endstop
						systemInfo.position_known = 1;
						_tgui_MovezUpdateHomed();
						ZMOTOR_SetPosition(cfgMotor.home_pos);
						systemInfo.target_position = cfgMotor.home_pos;
						ZMOTOR_DisableEndstops();
					}
					else
					{
						ZMOTOR_SetPosition(0);
						systemInfo.target_position = 0;
						if (systemInfo.print_is_homing)
						{
							systemInfo.print_is_homing = 0;
							BUZZ_TimerOn(cfgConfig.buzzer_msg);
							TGUI_MessageBoxOk(LANG_GetString(LSTR_ERROR), LANG_GetString(LSTR_MSG_ENDSTOP_ERROR), TGUI_PrintScreenExit);
						}
						else
						{
							BUZZ_TimerOn(cfgConfig.buzzer_msg);
							TGUI_MessageBoxOk(LANG_GetString(LSTR_ERROR), LANG_GetString(LSTR_MSG_ENDSTOP_ERROR));
						}
					}
					if (systemInfo.print_is_homing)
					{
						// TODO - file read error processing!
						PRINT_ReadLayerInfo();

						systemInfo.print_is_homing = 0;
						systemInfo.print_is_printing = 1;
						systemInfo.printer_state = PST_PRINT_MOVETOLAYER;
						systemInfo.target_position = cfgConfig.zero_pos + l_info.layer_position;
						systemInfo.print_current_height = systemInfo.target_position - cfgConfig.zero_pos;
						ZMOTOR_MoveAbsolute(systemInfo.target_position, cfgMotor.feedrate);
						UVFAN_On();
						// Motherboard fan
						if (cfgConfig.mb_fan_mode == MBFAN_PRINTING_ON)
							MBFAN_On();

						// TODO - file read error processing!
						PRINT_ReadLayerBegin();
					}
					else
					{
						systemInfo.printer_state = PST_IDLE;
						SYSTIMER_SetCountDown(zHoldTimer, cfgMotor.hold_time);
						SYSTIMER_SetCountDown(zDisTimer, cfgMotor.off_time);
					}
				}
				break;

			case PST_PRINT_MOVETOLAYER:
				if (ZMOTOR_IsMoving() == 0)
				{
					systemInfo.printer_state = PST_PRINT_LIGHTPAUSE;
					UVPAUSE_TimerOn((uint32_t)(l_info.lightoff_time * 1000));
				}
				break;

			case PST_PRINT_LIGHTPAUSE:
				if (UVPAUSE_TimerState() == 0)
				{
					systemInfo.printer_state = PST_PRINT_LIGHT;
					// linear decrement light time from base to normal
					if (systemInfo.print_use_ind_params == 0 && cfgConfig.light_down_layers > 0)
					{
						// first normal layer, calaulate decrement time
						if (systemInfo.print_current_layer == systemInfo.print_base_layers_count)
						{
							systemInfo.print_time_base_layers = PFILE_GetLightBottom();
							systemInfo.print_time_normal_layers = PFILE_GetLightLayer();
							systemInfo.print_time_decrement = (systemInfo.print_time_base_layers - systemInfo.print_time_normal_layers) / (float)cfgConfig.light_down_layers;
						}
						// decrement light time until it is no less then normal layers time
						if (systemInfo.print_time_decrement > 0)
						{
							// decrement time
							systemInfo.print_time_base_layers -= systemInfo.print_time_decrement;
							// set result time to current layer time
							l_info.light_time = systemInfo.print_time_base_layers;
							// check if result no less than normal layer time
							if (l_info.light_time < systemInfo.print_time_normal_layers)
							{
								l_info.light_time = systemInfo.print_time_normal_layers;
								systemInfo.print_time_decrement = 0;
							}
						}
					}
					UVLED_TimerOn((uint32_t)(l_info.light_time * 1000) / PFILE_GetAntialiasing());
					PRINT_DrawLayerPreview();
				}
				break;

			case PST_PRINT_LIGHT:
				if (UVLED_TimerState() == 0)
				{
					systemInfo.print_light_time_total += l_info.light_time / PFILE_GetAntialiasing();
					if (fv_filetype == FTYPE_PWS || fv_filetype == FTYPE_PHOTON)
					{
						systemInfo.print_current_sublayer++;
						if (PFILE_GetAntialiasing() > systemInfo.print_current_sublayer)
						{
							PRINT_ClearLayerPreview();
							// TODO - file read error processing!
							PRINT_ReadSublayerContinue();
							UVLED_TimerOn((uint32_t)(l_info.light_time * 1000) / PFILE_GetAntialiasing());
							PRINT_DrawLayerPreview();
							break;
						}
					}
					
					systemInfo.print_current_sublayer = 0;
					systemInfo.print_current_layer++;
					if (systemInfo.print_current_layer >= PFILE_GetTotalLayers() || systemInfo.target_position > cfgMotor.max_pos)
					{
						systemInfo.print_current_layer--;
						PRINT_Complete();
						break;
					}
					// TODO - file read error processing!
					PRINT_ReadLayerInfo();
					systemInfo.printer_state = PST_PRINT_LIFT;
					systemInfo.target_position = cfgConfig.zero_pos + l_info.layer_position;
					systemInfo.print_current_height = systemInfo.target_position - cfgConfig.zero_pos;
					systemInfo.target_position += l_info.lift_height;

					// slow accelerate and fast decelerate moving
					ZMOTOR_MoveAbsolute(systemInfo.target_position - (l_info.lift_height / 3), l_info.lift_speed);
					systemInfo.print_is_printing = 0;
					ZMOTOR_MoveAbsolute(systemInfo.target_position, l_info.lift_speed);
					systemInfo.print_is_printing = 1;

					// check pause
					if (systemInfo.print_is_paused)
					{
						systemInfo.printer_state = PST_PRINT_PAUSELIFT;
						systemInfo.target_position += cfgConfig.pause_lift;
						if (systemInfo.target_position > cfgMotor.max_pos)
							systemInfo.target_position = cfgMotor.max_pos;
						systemInfo.print_is_printing = 0;
						ZMOTOR_MoveAbsolute(systemInfo.target_position, cfgMotor.travel_feedrate / 3);
						systemInfo.print_is_printing = 1;
					}
					PRINT_ClearLayerPreview();

					// save work timers
					uint32_t	unix_time = DTIME_GetCurrentUnixtime();
					if ((unix_time - systemInfo.print_timer_secs) > TIMERS_SAVE_PERIOD)
					{
						cfgTimers.led_time += (uint32_t)systemInfo.print_light_time_total;
						cfgTimers.disp_time += (uint32_t)systemInfo.print_light_time_total;
						cfgTimers.fan_time += unix_time - systemInfo.print_timer_secs;
						cfgTimers.total_print_time += unix_time - systemInfo.print_timer_secs;
						cfgTimers.total_on_time += unix_time - systemInfo.timerWorkTimeSecs;

						systemInfo.print_light_time_total = 0;
						systemInfo.print_timer_secs = unix_time;
						systemInfo.timerWorkTimeSecs = unix_time;
						CFG_SaveTimers();
					}
					
					// TODO - file read error processing!
					PRINT_ReadLayerBegin();

				}
				break;

			case PST_PRINT_LIFT:
				if (systemInfo.print_is_paused)
				{
					systemInfo.printer_state = PST_PRINT_PAUSELIFT;
					systemInfo.target_position += cfgConfig.pause_lift;
					if (systemInfo.target_position > cfgMotor.max_pos)
						systemInfo.target_position = cfgMotor.max_pos;
					ZMOTOR_MoveAbsolute(systemInfo.target_position, cfgMotor.travel_feedrate / 3);
				}
				if (ZMOTOR_IsMoving() == 0 && systemInfo.print_is_paused == 0)
				{
					systemInfo.printer_state = PST_PRINT_DROP;
					systemInfo.target_position = cfgConfig.zero_pos + l_info.layer_position;

					// fast accelerate and slow decelerate moving
					systemInfo.print_is_printing = 0;
					ZMOTOR_MoveAbsolute(systemInfo.target_position + (l_info.lift_height / 1.5), l_info.drop_speed);
					systemInfo.print_is_printing = 1;
					ZMOTOR_MoveAbsolute(systemInfo.target_position, l_info.drop_speed);

				}
				break;

			case PST_PRINT_PAUSELIFT:
				if (ZMOTOR_IsMoving() == 0)
				{
					if (systemInfo.print_is_paused == 0)
					{
						systemInfo.printer_state = PST_PRINT_DROP;
						systemInfo.target_position = cfgConfig.zero_pos + l_info.layer_position;
						systemInfo.target_position += l_info.lift_height;
						systemInfo.print_is_printing = 0;
						ZMOTOR_MoveAbsolute(systemInfo.target_position, cfgMotor.travel_feedrate / 3);
						systemInfo.print_is_printing = 1;
						systemInfo.target_position = cfgConfig.zero_pos + l_info.layer_position;
						ZMOTOR_MoveAbsolute(systemInfo.target_position, l_info.drop_speed);
						UVFAN_On();
						if (cfgConfig.mb_fan_mode == MBFAN_PRINTING_ON)
							MBFAN_On();
					}
					else
					{
						UVFAN_Off();
						if (cfgConfig.mb_fan_mode == MBFAN_PRINTING_ON)
							MBFAN_Off();
					}
				}
				break;

			case PST_PRINT_DROP:
				if (ZMOTOR_IsMoving() == 0)
				{
					systemInfo.printer_state = PST_PRINT_LIGHTPAUSE;
					UVPAUSE_TimerOn((uint32_t)(l_info.lightoff_time * 1000));
				}
				break;

			case PST_PRINT_LASTLAYERLIFT:
				if (ZMOTOR_IsMoving() == 0)
				{
					systemInfo.printer_state = PST_IDLE;
					if (systemInfo.print_is_canceled == 0)
					{
						uint32_t	ut = DTIME_GetCurrentUnixtime() - systemInfo.print_time_begin - systemInfo.print_pause_time;
						uint32_t	ut_h = ut / 3600;
						uint32_t	ut_m = (ut - (ut_h * 3600)) / 60;
						char		*t = LANG_GetString(LSTR_MSG_PRINT_COMPLETED);
						sprintf(msg, t, ut_h, ut_m, ut % 60); 
						TGUI_MessageBoxOk(LANG_GetString(LSTR_COMPLETED), msg, TGUI_PrintScreenExit);
					}
					else
					{
						if (tguiActiveScreen == (TG_SCREEN*)&tguiMsgBox)
						{
							tguiActiveScreen = (TG_SCREEN*)((TG_MSGBOX*)tguiActiveScreen)->prevscreen;
						}
						TGUI_MessageBoxOk(LANG_GetString(LSTR_COMPLETED), LANG_GetString(LSTR_MSG_PRINT_CANCELED), TGUI_PrintScreenExit);
						systemInfo.print_is_canceled = 0;
					}
					BUZZ_TimerOn(cfgConfig.buzzer_msg);
					SYSTIMER_SetCountDown(zHoldTimer, cfgMotor.hold_time);
					SYSTIMER_SetCountDown(zDisTimer, cfgMotor.off_time);
				}
				break;

		}

		USB_HOST_Process();
		if (Appli_state_old != Appli_state)
		{
			Appli_state_old = Appli_state;
			// ------------------ Service mode -------------------
			switch (Appli_state)
			{
				case APPLICATION_IDLE:
					break;

				case APPLICATION_START:
					break;

				case APPLICATION_READY:
					if (f_mount(&UsbFS, UsbPath, 1) == FR_OK)
					{
						UsbMounted = 1;
					}
					TGUI_USBStateChanged();
					break;

				case APPLICATION_DISCONNECT:
					f_mount(NULL, UsbPath, 1);
					memset(&UsbFS, 0, sizeof(FATFS));
					UsbMounted = 0;
					TGUI_USBStateChanged();
					break;

			}
		}
	}
	
}
//==============================================================================



void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

#ifdef __MKSDLP_BOARD__
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 4;
	RCC_OscInitStruct.PLL.PLLN = 168;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
#endif  // __MKSDLP_BOARD__
#ifdef __CHITU_BOARD__
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 10;
	RCC_OscInitStruct.PLL.PLLN = 168;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
#endif  // __CHITU_BOARD__

	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
						  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
		Error_Handler();
	}
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
#ifdef __MKSDLP_BOARD__
	// external rezonator
	PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
#endif  // __MKSDLP_BOARD__
#ifdef __CHITU_BOARD__
	// external rezonator missing, so using external clock 20 MHz divided by 20
	PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_HSE_DIV20;
#endif  // __CHITU_BOARD__

	PWR->CR |= PWR_CR_DBP;
    while((PWR->CR & PWR_CR_DBP) == 0);
	RCC->BDCR |= RCC_BDCR_BDRST;
	RCC->BDCR &= ~RCC_BDCR_BDRST;
	__HAL_RCC_RTC_CONFIG(PeriphClkInitStruct.RTCClockSelection);
	RCC->BDCR |= RCC_BDCR_RTCEN;
}
//==============================================================================



void SystemClock_SwitchToLow()
{
}
//==============================================================================



void SystemClock_SwitchToHigh()
{
}
//==============================================================================



void Error_Handler(void)
{
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{ 
}
//==============================================================================
#endif /* USE_FULL_ASSERT */

