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
#include "z_stepper.h"
#include "uvdisplay.h"
#include "eeprom.h"
#include "tgui_movezscreenfuncs.h"
#include "tgui_printscreenfuncs.h"
#include "tgui_screensaverfuncs.h"
#include "printing.h"
#include "tgui_uvtestscreenfuncs.h"


#define	SDIR_IMAGES			(char*)"alterupd\\images"




extern USBH_HandleTypeDef		hUsbHostFS;
extern ApplicationTypeDef		Appli_state;
ApplicationTypeDef				Appli_state_old;

extern TCHAR					UsbPath[4];   /* USBH logical drive path */
extern FATFS					UsbFS;    /* File system object for USBH logical drive */
uint8_t							UsbMounted;
extern TCHAR					SpiflPath[4];
extern FATFS					SpiflFS;

extern TOUCH_INFO				touchInfo;

extern PRINT_STATE				prtState;
extern FILES_TYPE				fv_filetype;

TOUCH_POINT						touchCoord;
TOUCH_STATES					touchState;

uint8_t							tguiTimer;
uint8_t							zHoldTimer;
uint8_t							zDisTimer;
uint8_t							tguiScreenTimer;

uint8_t							srvMode;
	


__no_init uint8_t 				fbuff[16384] @ "CCMRAM";
		
		
__no_init FIL					ufile @ "CCMRAM";
__no_init FIL					sfile;
TCHAR							u_tfname[512];
TCHAR							s_tfname[512];
char							cfname[1024];
char							msg[512];
FRESULT							fres;
DIR								dir = {0};
FILINFO							finfo = {0};

extern FPWS_LAYERSINFO			l_info;


void		SystemClock_Config(void);


int main()
{
	HAL_Init();
	SystemClock_Config();
	SCB->VTOR = FLASH_BASE | 0x10000;

	GPIO_Init();

	// FatFS init
	FATFS_Init();
	// Touch interface init
	Touch_Init();
	Touch_Enable();

	SYSTIMER_Init();

	tguiTimer = SYSTIMER_NewCountDown(TIMER_DISABLE);
	zHoldTimer = SYSTIMER_NewCountDown(TIMER_DISABLE);
	zDisTimer = SYSTIMER_NewCountDown(TIMER_DISABLE);
	tguiScreenTimer = SYSTIMER_NewCountDown(TIMER_DISABLE);
	
	EEPROM_Init();

	CFG_Init();
	
	LANG_SetLanguage(0);
	
	// LCD init
	LCD_Initializtion();
	LCDUI_Init();
	// SPI flash interface init
	FLASH_SPIInit();
	FLASH_SPIEnable();
	FLASH_SPISetSpeed(SPI_BAUDRATEPRESCALER_8);
	// SPI flash logic init
	SPIFL_Init();
	_touch_ReadCoords();
	_touch_ReadCoords();

	LCDUI_SetFont(LCDUI_FONT_H18);
	LCDUI_SetColor(COLOR_WHITE);
	LCDUI_SetBackColor(COLOR_BLACK);
	LCDUI_Clear();
	TGUI_Init();
	
	// SPIFlash filesystem mounting
	if (f_mount(&SpiflFS,  SpiflPath, 1) != FR_OK)
	{
		LCDUI_DrawText((char*)"> !! Internal filesystem error, try to format...\n");
		if (f_mkfs(SpiflPath, 0, fbuff, sizeof(fbuff)) != FR_OK)
		{
			LCDUI_DrawText((char*)"> !! Failed to formate internal filesystem\n");
		}
		else
		{
			LCDUI_DrawText((char*)"> Internal filesystem format success\n");
		}
//		while(1);
	}
	
	LCD_BackLight(1);
	
	// If touch pressed - service booting with update
	srvMode = 0;
	if (Touch_IsPressed())
	{
		TOUCH_POINT tp;
		Touch_GetCoords(&tp);
		if ( tp.xc > 430 && tp.yc < 50)
		{
			srvMode = 1;
		}
	}
	
	// ------------------ Service mode -------------------
	if (srvMode)
	{
		LCDUI_SetColor(COLOR_GREEN);
		LCDUI_DrawText((char*)"> Service mode\n");


		FATFS *fs;
		DWORD fre_clust, fre_sect, tot_sect;

		if (f_getfree(SpiflPath, &fre_clust, &fs) != FR_OK)
		{
			LCDUI_DrawText((char*)"> Can't get free space of internal filesystem\n");
		}
		else
		{
			tot_sect = (fs->n_fatent - 2) * fs->csize * fs->ssize / 1024;
			fre_sect = fre_clust * fs->csize * fs->ssize / 1024;
			if (fre_sect == 0)
			{
				LCDUI_DrawText((char*)"> !! Internal filesystem error, try to format...\n");
				if (f_mkfs(SpiflPath, 0, fbuff, sizeof(fbuff)) != FR_OK)
				{
					LCDUI_DrawText((char*)"> !! Failed to formate internal filesystem\n");
				}
				else
				{
					LCDUI_DrawText((char*)"> Internal filesystem format success\n");
					f_getfree(SpiflPath, &fre_clust, &fs);
					tot_sect = (fs->n_fatent - 2) * fs->csize * fs->ssize / 1024;
					fre_sect = fre_clust * fs->csize * fs->ssize / 1024;
				}
			}
				

			sprintf(msg, "> %u KB total internal filesystem,  %u KB available.\n", tot_sect, fre_sect);
			LCDUI_DrawText(msg);
		}
/*
		LCDUI_DrawText((char*)"> Formating internal filesystem...\n");
		f_mount(NULL,  SpiflPath, 1);
		if (f_mkfs(SpiflPath, 0, fbuff, sizeof(fbuff)) != FR_OK)
		{
			LCDUI_DrawText((char*)"> !! Failed to formate internal filesystem\n");
		}
		else
		{
			LCDUI_DrawText((char*)"> Internal filesystem formated succes\n");
		}
		if (f_mount(&SpiflFS,  SpiflPath, 1) != FR_OK)
		{
			LCDUI_DrawText((char*)"> !! Failed to mount internal filesystem\n");
		}
		else
		{
			LCDUI_DrawText((char*)"> Internal filesystem mounted succes\n");
		}
*/
	}
	else
	{
		// Draw initial logo
		TGUI_DrawLogo();
		sprintf(msg, "v %d.%02d", (FW_VERSION >> 8) & 0xFF, FW_VERSION & 0xFF);
		LCDUI_SetColor(LCDUI_RGB(0x47C7DA));
		LCDUI_DrawText(msg, LCDUI_TEXT_TRANSBACK | LCDUI_TEXT_ALIGN_RIGHT, 260, 192, 356, -1);
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
	
	LANG_SetLanguage(cfgConfig.language);

	// ZMotor
	ZMOTOR_Init();
	ZMOTOR_MotorDisable();
	SYSTIMER_SetCountDown(zHoldTimer, TIMER_DISABLE);
	SYSTIMER_SetCountDown(zDisTimer, TIMER_DISABLE);
	SYSTIMER_SetCountDown(tguiScreenTimer, cfgConfig.screensaver_time);
	
	_cpld_bank2disp_enable(CLEAN_USED_BANK,0,0);

	UVD_Init();
/*
	UVD_ExposSetCircle();
	UVD_Wakeup();
	UVLED_On();
	HAL_Delay(1000);
	UVLED_Off();
*/
	UVD_Sleep();
	_cpld_bank2disp_enable(CLEAN_USED_BANK,0,0);
	_tgui_UVTestReadImage(0);
	
	// Disable USB power line
	USB_HOST_VbusFS(1);

	USB_HOST_Init();
	Appli_state_old = Appli_state;

	
	
	// Enable USB power line
	USB_HOST_VbusFS(0);

	// ------------------ Service mode -------------------
	if (!srvMode)
	{
		HAL_Delay(500);
		TGUI_ForceRepaint();
	}
	
	SYSTIMER_SetCountDown(tguiTimer, 25);

	while(1)
	{
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
							UsbMounted = 1;
							LCDUI_DrawText((char*)"> +USB storage mounted\n");
							// Check for images update on USB disk
							tstrcpy(u_tfname, UsbPath);
							tstrcat_utf(u_tfname, SDIR_IMAGES);
							// Found update directory
							char *ext;
							if (f_opendir(&dir, u_tfname) == FR_OK)
							{
								LCDUI_DrawText((char*)"> Firmware images directory found\n");
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
										if (
												(strcmp(ext, (char*)"cimg") == 0 && finfo.fsize < 320000) 
											)
										{
											// Found update file, copying it into SPIFlash filesystem
											
											if (LCDUI_GetCurrentCursorY() > (LCD_HEIGHT - LCDUI_GetCurrentFontHeight()))
											{
												LCDUI_Clear();
											}
											// Open file for read in USB
											tstrcpy(u_tfname, UsbPath);
											tstrcat_utf(u_tfname, SDIR_IMAGES);
											tstrcat_utf(u_tfname, (char*)"\\");
											tstrcat(u_tfname, finfo.fname);
											if (f_open(&ufile, u_tfname, FA_OPEN_EXISTING | FA_READ) != FR_OK)
											{
												LCDUI_DrawText((char*)"> !! Error opening file in USB filesystem\n");
												break;
											}
													
											// Open file for write in SPIFlash
											tstrcpy(s_tfname, SpiflPath);
											tstrcat(s_tfname, finfo.fname);
											if (f_open(&sfile, s_tfname, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
											{
												LCDUI_DrawText((char*)"> !! Error creating file in internal filesystem\n");
												break;
											}
											
											// Copying
											sprintf(msg, "> Copying file \"%s\" ", cfname);
											LCDUI_DrawText(msg);
											DWORD 		readed = 0, writed = 0;
											uint8_t		iter = 0;
											do
											{
												if (f_read(&ufile, fbuff, sizeof(fbuff), &readed) != FR_OK)
												{
													LCDUI_DrawText((char*)"> !! Error reading file from USB filesystem\n");
													break;
												}
												if (f_write(&sfile, fbuff, readed, &writed) != FR_OK)
												{
													LCDUI_DrawText((char*)"> !! Error writing file to internal filesystem\n");
													break;
												}
												iter++;
	//												if ((iter % 2) == 0)
													LCDUI_DrawText((char*)".");
											} while (readed == sizeof(fbuff));
											f_close(&ufile);
											f_close(&sfile);
											LCDUI_DrawText((char*)" success\n");
											LCDUI_SetCursorCoord(0, -1);
										}
									}
									
								}
								f_close(&ufile);
								f_close(&sfile);
								f_closedir(&dir);
							}
							else
							{
								LCDUI_DrawText((char*)"> Update directory not found\n", LCDUI_TEXT_TRANSBACK);
							}
						}
						break;

					case APPLICATION_DISCONNECT:
						f_mount(NULL, UsbPath, 1);
						memset(&UsbFS, 0, sizeof(FATFS));
						UsbMounted = 0;
						if (srvMode)
						{
							LCDUI_DrawText((char*)"> -USB storage removed\n");
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
					SYSTIMER_SetCountDown(zHoldTimer, cfgzMotor.hold_time);
					SYSTIMER_SetCountDown(zDisTimer, cfgzMotor.off_time);
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
						SYSTIMER_SetCountDown(zHoldTimer, cfgzMotor.hold_time);
						SYSTIMER_SetCountDown(zDisTimer, cfgzMotor.off_time);
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
						if (cfgzMotor.home_dir == -1)
							systemInfo.target_position = cfgzMotor.max_pos * (-1.0);
						else
							systemInfo.target_position = cfgzMotor.max_pos;
						ZMOTOR_MoveAbsolute(systemInfo.target_position, cfgzMotor.homing_feedrate_fast);
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
					if (cfgzMotor.home_dir == -1 && ZMOTOR_GetEndstopState() & (1 << Z_MIN))
					{
						// moved to min endstop
						ZMOTOR_SetPosition(cfgzMotor.home_pos);
						systemInfo.target_position = cfgzMotor.home_pos + 3;
						ZMOTOR_MoveAbsolute(systemInfo.target_position, cfgzMotor.homing_feedrate_fast);
						systemInfo.printer_state = PST_HOMING_UP;
						if (systemInfo.print_is_homing)
							UVD_Wakeup();
					}
					else if (cfgzMotor.home_dir == 1 && ZMOTOR_GetEndstopState() & (1 << Z_MAX))
					{
						// moved to max endstop
						ZMOTOR_SetPosition(cfgzMotor.home_pos);
						systemInfo.target_position = cfgzMotor.home_pos - 3;
						ZMOTOR_MoveAbsolute(systemInfo.target_position, cfgzMotor.homing_feedrate_fast);
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
						SYSTIMER_SetCountDown(zHoldTimer, cfgzMotor.hold_time);
						SYSTIMER_SetCountDown(zDisTimer, cfgzMotor.off_time);
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
						SYSTIMER_SetCountDown(zHoldTimer, cfgzMotor.hold_time);
						SYSTIMER_SetCountDown(zDisTimer, cfgzMotor.off_time);
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
						if (cfgzMotor.home_dir == -1)
						{
							systemInfo.target_position = -1;
							ZMOTOR_MoveAbsolute(systemInfo.target_position, cfgzMotor.homing_feedrate_slow);
							systemInfo.printer_state = PST_HOMING_SLOW;
						}
						else
						{
							systemInfo.target_position = 1;
							ZMOTOR_MoveAbsolute(systemInfo.target_position, cfgzMotor.homing_feedrate_slow);
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
						ZMOTOR_SetPosition(cfgzMotor.home_pos);
						systemInfo.target_position = cfgzMotor.home_pos;
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
						systemInfo.target_position = cfgConfig.zero_pos + (systemInfo.print_current_layer + 1) * PFILE_GetLayerThickness();
						systemInfo.print_current_height = systemInfo.target_position - cfgConfig.zero_pos;
						ZMOTOR_MoveAbsolute(systemInfo.target_position, cfgzMotor.feedrate);
						UVFAN_On();

						// TODO - file read error processing!
						PRINT_ReadLayerBegin();
					}
					else
					{
						systemInfo.printer_state = PST_IDLE;
						SYSTIMER_SetCountDown(zHoldTimer, cfgzMotor.hold_time);
						SYSTIMER_SetCountDown(zDisTimer, cfgzMotor.off_time);
					}
				}
				break;

			case PST_PRINT_MOVETOLAYER:
				if (ZMOTOR_IsMoving() == 0)
				{
					systemInfo.printer_state = PST_PRINT_LIGHTPAUSE;
					UVPAUSE_TimerOn((uint32_t)(PFILE_GetLightPause() * 1000));
				}
				break;

			case PST_PRINT_LIGHTPAUSE:
				if (UVPAUSE_TimerState() == 0)
				{
					systemInfo.printer_state = PST_PRINT_LIGHT;
					UVLED_TimerOn((uint32_t)(l_info.light_time * 1000) / PFILE_GetAntialiasing());
					systemInfo.print_light_time_total += l_info.light_time / PFILE_GetAntialiasing();
					PRINT_DrawLayerPreview();
				}
				break;

			case PST_PRINT_LIGHT:
				if (UVLED_TimerState() == 0)
				{
					if (fv_filetype == FTYPE_PWS)
					{
						systemInfo.print_current_sublayer++;
						if (PFILE_GetAntialiasing() > systemInfo.print_current_sublayer)
						{
							PRINT_ClearLayerPreview();
							// TODO - file read error processing!
							PRINT_ReadSublayerContinue();
							UVLED_TimerOn((uint32_t)(l_info.light_time * 1000) / PFILE_GetAntialiasing());
							systemInfo.print_light_time_total += l_info.light_time / PFILE_GetAntialiasing();
							PRINT_DrawLayerPreview();
							break;
						}
					}
					
					systemInfo.print_current_sublayer = 0;
					systemInfo.print_current_layer++;
					// TODO - file read error processing!
					PRINT_ReadLayerInfo();
					systemInfo.printer_state = PST_PRINT_LIFT;
					systemInfo.target_position = cfgConfig.zero_pos + (systemInfo.print_current_layer + 1) * PFILE_GetLayerThickness();
					systemInfo.print_current_height = systemInfo.target_position - cfgConfig.zero_pos;
					systemInfo.target_position += l_info.lift_height;
					if (systemInfo.print_current_layer >= PFILE_GetTotalLayers() || systemInfo.target_position > cfgzMotor.max_pos)
					{
						systemInfo.print_current_layer--;
						PRINT_Complete();
						break;
					}

					ZMOTOR_MoveAbsolute(systemInfo.target_position, l_info.lift_speed);
					// check pause
					if (systemInfo.print_is_paused)
					{
						systemInfo.printer_state = PST_PRINT_PAUSELIFT;
						systemInfo.target_position += cfgConfig.pause_lift;
						if (systemInfo.target_position > cfgzMotor.max_pos)
							systemInfo.target_position = cfgzMotor.max_pos;
						ZMOTOR_MoveAbsolute(systemInfo.target_position, cfgzMotor.travel_feedrate / 3);
					}
					PRINT_ClearLayerPreview();
					// TODO - file read error processing!
					PRINT_ReadLayerBegin();

				}
				break;

			case PST_PRINT_LIFT:
				if (systemInfo.print_is_paused)
				{
					systemInfo.printer_state = PST_PRINT_PAUSELIFT;
					systemInfo.target_position += cfgConfig.pause_lift;
					if (systemInfo.target_position > cfgzMotor.max_pos)
						systemInfo.target_position = cfgzMotor.max_pos;
					ZMOTOR_MoveAbsolute(systemInfo.target_position, cfgzMotor.travel_feedrate / 3);
				}
				if (ZMOTOR_IsMoving() == 0 && systemInfo.print_is_paused == 0)
				{
					systemInfo.printer_state = PST_PRINT_DROP;
					systemInfo.target_position = cfgConfig.zero_pos + (systemInfo.print_current_layer + 1) * PFILE_GetLayerThickness();
					ZMOTOR_MoveAbsolute(systemInfo.target_position, PFILE_GetDropSpeed());
				}
				break;

			case PST_PRINT_PAUSELIFT:
				if (ZMOTOR_IsMoving() == 0 && systemInfo.print_is_paused == 0)
				{
					systemInfo.printer_state = PST_PRINT_DROP;
					systemInfo.target_position = cfgConfig.zero_pos + (systemInfo.print_current_layer + 1) * PFILE_GetLayerThickness();
					systemInfo.target_position += l_info.lift_height;
					ZMOTOR_MoveAbsolute(systemInfo.target_position, cfgzMotor.travel_feedrate / 3);
					systemInfo.target_position = cfgConfig.zero_pos + (systemInfo.print_current_layer + 1) * PFILE_GetLayerThickness();
					ZMOTOR_MoveAbsolute(systemInfo.target_position, PFILE_GetDropSpeed());
				}
				break;

			case PST_PRINT_DROP:
				if (ZMOTOR_IsMoving() == 0)
				{
					systemInfo.printer_state = PST_PRINT_LIGHTPAUSE;
					UVPAUSE_TimerOn((uint32_t)(PFILE_GetLightPause() * 1000));
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
					SYSTIMER_SetCountDown(zHoldTimer, cfgzMotor.hold_time);
					SYSTIMER_SetCountDown(zDisTimer, cfgzMotor.off_time);
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

