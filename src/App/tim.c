/**
  ******************************************************************************
  * File Name          : TIM.c
  * Description        : This file provides code for the configuration
  *                      of the TIM instances.
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
#include "tim.h"

TIM_HandleTypeDef	hStepperTim;
TIM_HandleTypeDef	hZRefTim;

// TIM2 init function - Stepper timer
void TIM_ZStepper_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	hStepperTim.Instance = TIM2;
	hStepperTim.Init.Prescaler = STEPPER_TIMER_PRESCALE;
	hStepperTim.Init.CounterMode = TIM_COUNTERMODE_UP;
	hStepperTim.Init.Period = 1000;
	hStepperTim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	hStepperTim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&hStepperTim) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&hStepperTim, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&hStepperTim, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}

}


// TIM5 init function - Z Vref output
void TIM_ZRef_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};

	hZRefTim.Instance = Z_REF_TIMER;
	hZRefTim.Init.Prescaler = 2;
	hZRefTim.Init.CounterMode = TIM_COUNTERMODE_UP;
	hZRefTim.Init.Period = 749;
	hZRefTim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	hZRefTim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&hZRefTim) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&hZRefTim, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&hZRefTim) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&hZRefTim, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&hZRefTim, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}
	HAL_TIM_MspPostInit(&hZRefTim);

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{
	if(tim_baseHandle->Instance==TIM2)
	{
		/* TIM2 clock enable */
		__HAL_RCC_TIM2_CLK_ENABLE();

		/* TIM2 interrupt Init */
		HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(TIM2_IRQn);
	}

	if(tim_baseHandle->Instance == Z_REF_TIMER)
	{
		/* TIM5 clock enable */
		Z_REF_TIMER_CLK_ENABLE();

	}
}
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	if(timHandle->Instance == Z_REF_TIMER)
	{
		/**TIM5 GPIO Configuration    
		PA0-WKUP     ------> TIM5_CH1 
		*/
		GPIO_InitStruct.Pin = Z_REF_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = Z_REF_ALTERNATE;
		HAL_GPIO_Init(Z_REF_GPIO, &GPIO_InitStruct);

	}

}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{
	if(tim_baseHandle->Instance==TIM2)
	{
		/* Peripheral clock disable */
		__HAL_RCC_TIM2_CLK_DISABLE();

		/* TIM2 interrupt Deinit */
		HAL_NVIC_DisableIRQ(TIM2_IRQn);
	}

	if(tim_baseHandle->Instance == Z_REF_TIMER)
	{
		/* Peripheral clock disable */
		Z_REF_TIMER_CLK_DISABLE();

		/* TIM5 interrupt Deinit */
		HAL_NVIC_DisableIRQ(Z_REF_TIMER_IRQ);
	}
} 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
