/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f4xx_hal.h"
#include "includes.h"

	

#define TFT_LIGHT_Pin			GPIO_PIN_12
#define TFT_LIGHT_GPIO_Port		GPIOB

#define TFT_RESET_Pin			GPIO_PIN_13
#define TFT_RESET_GPIO_Port		GPIOD

#define ZE_MIN_Pin				GPIO_PIN_0
#define ZE_MIN_GPIO_Port		GPIOC
#define ZE_MIN_EXTI_IRQn		EXTI0_IRQn
	
#define ZE_MAX_Pin				GPIO_PIN_2
#define ZE_MAX_GPIO_Port		GPIOC
#define ZE_MAX_EXTI_IRQn		EXTI2_IRQn

#define Z_DIR_Pin				GPIO_PIN_1
#define Z_DIR_GPIO_Port			GPIOC

#define Z_STEP_Pin				GPIO_PIN_1
#define Z_STEP_GPIO_Port		GPIOA

#define Z_ENA_Pin				GPIO_PIN_2
#define Z_ENA_GPIO_Port			GPIOA


#define SPI1_CS_CPLD_Pin		GPIO_PIN_7
#define SPI1_CS_CPLD_GPIO_Port	GPIOA

#define SPI1_CS_SSDA_Pin		GPIO_PIN_3
#define SPI1_CS_SSDA_GPIO_Port	GPIOC

#define SPI1_CS_SSDB_Pin		GPIO_PIN_3
#define SPI1_CS_SSDB_GPIO_Port	GPIOA

#define MIPI_CE_Pin				GPIO_PIN_4
#define MIPI_CE_GPIO_Port		GPIOA

#define MIPI_EN_PWR_Pin			GPIO_PIN_5
#define MIPI_EN_PWR_GPIO_Port	GPIOA

#define SPI1_SDC_Pin			GPIO_PIN_4
#define SPI1_SDC_GPIO_Port		GPIOC

#define EPM1270T_IO1_Pin		GPIO_PIN_5
#define EPM1270T_IO1_GPIO_Port 	GPIOC

#define CPLD_CRC_Pin			GPIO_PIN_0
#define CPLD_CRC_GPIO_Port		GPIOB

#define CPLD_RST_Pin			GPIO_PIN_1
#define CPLD_RST_GPIO_Port		GPIOB

#define FLSPI_CS_Pin			GPIO_PIN_15
#define FLSPI_CS_GPIO_Port		GPIOA

#define USBH_VBUS_Pin			GPIO_PIN_3
#define USBH_VBUS_GPIO_Port		GPIOD

#define LED_UV_Pin				GPIO_PIN_2
#define LED_UV_GPIO_Port		GPIOE

#define BUZZER_Pin				GPIO_PIN_13
#define BUZZER_GPIO_Port		GPIOB

#define FAN_LED_Pin				GPIO_PIN_4
#define FAN_LED_GPIO_Port		GPIOE

#define FAN_MB_Pin				GPIO_PIN_3
#define FAN_MB_GPIO_Port		GPIOE



#define FAN_MB_On()				FAN_MB_GPIO_Port->BSRR = FAN_MB_Pin						// Set pin
#define FAN_MB_Off()			FAN_MB_GPIO_Port->BSRR = (uint32_t)FAN_MB_Pin << 16U	// Reset pin




void Error_Handler(void);








#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
