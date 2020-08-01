/**
  ******************************************************************************
  * File Name          : RTC.c
  * Description        : This file provides code for the configuration
  *                      of the RTC instances.
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
#include "rtc.h"



RTC_HandleTypeDef hRTC;



/* RTC init function */
void RTC_Init(void)
{
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};

	/** Initialize RTC Only 
	*/
	hRTC.Instance = RTC;
	hRTC.Init.HourFormat = RTC_HOURFORMAT_24;
	hRTC.Init.AsynchPrediv = 127;
	hRTC.Init.SynchPrediv = 255;
	hRTC.Init.OutPut = RTC_OUTPUT_DISABLE;
	hRTC.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	hRTC.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	
	if ((RTC->DR & 0x00FF0000) == 0)
	{
	
		if (HAL_RTC_Init(&hRTC) != HAL_OK)
		{
			Error_Handler();
		}

		/** Initialize RTC and set the Time and Date 
		*/
		sTime.Hours = 18;
		sTime.Minutes = 0;
		sTime.Seconds = 0;
		sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
		sTime.StoreOperation = RTC_STOREOPERATION_RESET;
		if (HAL_RTC_SetTime(&hRTC, &sTime, RTC_FORMAT_BIN) != HAL_OK)
		{
			Error_Handler();
		}
		sDate.WeekDay = RTC_WEEKDAY_MONDAY;
		sDate.Month = RTC_MONTH_JULY;
		sDate.Date = 20;
		sDate.Year = 21;

		if (HAL_RTC_SetDate(&hRTC, &sDate, RTC_FORMAT_BIN) != HAL_OK)
		{
			Error_Handler();
		}
	}
}

void RTC_Enable(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();
  }
}

void RTC_Disable(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();
  }
} 

void RTC_BKUPWrite(uint32_t BackupRegister, uint32_t Data)
{
  uint32_t tmp = 0U;

  tmp = (uint32_t)&(RTC->BKP0R);
  tmp += (BackupRegister * 4U);

  *(__IO uint32_t *)tmp = (uint32_t)Data;
}

uint32_t RTC_BKUPRead(uint32_t BackupRegister)
{
  uint32_t tmp = 0U;

  tmp = (uint32_t)&(RTC->BKP0R);
  tmp += (BackupRegister * 4U);

  return (*(__IO uint32_t *)tmp);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
