/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

	  
	  
void GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();

	HAL_GPIO_WritePin(LED_UV_GPIO, LED_UV_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(Z_DIR_GPIO, Z_DIR_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(Z_STEP_GPIO, Z_STEP_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(Z_ENA_GPIO, Z_ENA_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(FST_SPI_CS_SSDA_GPIO, FST_SPI_CS_SSDA_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(FST_SPI_SDC_GPIO, FST_SPI_SDC_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(CPLD_IO1_GPIO, CPLD_IO1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(FST_SPI_CS_SSDB_GPIO, FST_SPI_CS_SSDB_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(FST_SPI_CS_CPLD_GPIO, FST_SPI_CS_CPLD_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(FST_SPI_CS_FLASH_GPIO, FST_SPI_CS_FLASH_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MIPI_CE_GPIO, MIPI_CE_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MIPI_EN_PWR_GPIO, MIPI_EN_PWR_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(CPLD_RST_GPIO, CPLD_RST_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(TFT_LIGHT_GPIO, TFT_LIGHT_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(BUZZER_GPIO, BUZZER_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(FAN_LED_GPIO, FAN_LED_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(FAN_MB_GPIO, FAN_MB_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(TFT_RESET_GPIO, TFT_RESET_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(USBH_VBUS_GPIO, USBH_VBUS_Pin, GPIO_PIN_RESET);


// set not used pins in analog mode
#ifdef __MKSDLP11__
	GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6 
							|GPIO_PIN_0|GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8 
							|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_6 
							|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_2|GPIO_PIN_6;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
#endif
#ifdef __MKSDLP20__
	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6 
							|GPIO_PIN_0|GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8 
							|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_6 
							|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_2|GPIO_PIN_6;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
#endif
#ifdef __LV3_BOARD__
	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6 
							|GPIO_PIN_0|GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8 
							|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_6 
							|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_2|GPIO_PIN_6;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
#endif

	GPIO_InitStruct.Pin = LED_UV_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED_UV_GPIO, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = ZE_MIN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(ZE_MIN_GPIO, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = ZE_MAX_Pin;
	HAL_GPIO_Init(ZE_MAX_GPIO, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = Z_DIR_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(Z_DIR_GPIO, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FST_SPI_SDC_Pin;
	HAL_GPIO_Init(FST_SPI_SDC_GPIO, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = CPLD_IO1_Pin;
	HAL_GPIO_Init(CPLD_IO1_GPIO, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = Z_STEP_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(Z_STEP_GPIO, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = Z_ENA_Pin;
	HAL_GPIO_Init(Z_ENA_GPIO, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = MIPI_CE_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(MIPI_CE_GPIO, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = MIPI_EN_PWR_Pin;
	HAL_GPIO_Init(MIPI_EN_PWR_GPIO, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = CPLD_CRC_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(CPLD_CRC_GPIO, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = CPLD_RST_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(CPLD_RST_GPIO, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = TFT_LIGHT_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(TFT_LIGHT_GPIO, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = BUZZER_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(BUZZER_GPIO, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = FAN_LED_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(FAN_LED_GPIO, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = FAN_MB_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(FAN_MB_GPIO, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = TFT_RESET_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(TFT_RESET_GPIO, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = USBH_VBUS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(USBH_VBUS_GPIO, &GPIO_InitStruct);

}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
