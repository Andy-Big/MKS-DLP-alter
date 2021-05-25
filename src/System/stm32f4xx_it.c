/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32f4xx_it.h"

/* External variables --------------------------------------------------------*/

extern HCD_HandleTypeDef 		hhcd_USB_OTG_FS;

extern DMA_HandleTypeDef		hTouchSpiDmaRx;
extern DMA_HandleTypeDef		hTouchSpiDmaTx;

extern DMA_HandleTypeDef		hLcdDma;

extern DMA_HandleTypeDef		hFlashSpiDmaRx;
extern DMA_HandleTypeDef		hFlashSpiDmaTx;

extern TIM_HandleTypeDef		hStepperTim;

extern void 					IsrStepperHandler();

extern void						EndstopsInterrupt();
//extern void (*IsrStepperHandler)();

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */ 
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief SPI2 RX DMA interrupt.
  */
void DMA1_Stream3_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hTouchSpiDmaRx);
}

/**
  * @brief SPI2 TX DMA interrupt.
  */
void DMA1_Stream4_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hTouchSpiDmaTx);
}

/**
  * @brief This function handles USB On The Go FS global interrupt.
  */
void OTG_FS_IRQHandler(void)
{
  HAL_HCD_IRQHandler(&hhcd_USB_OTG_FS);
}

/**
  * @brief This function handles DMA2 stream7 global interrupt - mem to mem
  */
void DMA2_Stream7_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hLcdDma);
}

/**
  * @brief SPI1 RX DMA interrupt.
  */
void DMA2_Stream2_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hFlashSpiDmaRx);
}

/**
  * @brief SPI1 TX DMA interrupt.
  */
void DMA2_Stream5_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hFlashSpiDmaTx);
}

/**
  * @brief This function handles TIM2 global interrupt - Stepper steps generation timer
  */
void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&hStepperTim);
	IsrStepperHandler();
}




#ifdef __MKSDLP_BOARD__
  #ifdef __MKSDLP_V11__
void EXTI0_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(ZE_MIN_Pin);
  EndstopsInterrupt();
}

void EXTI2_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(ZE_MAX_Pin);
  EndstopsInterrupt();
}
  #endif  // __MKSDLP_V11__
  #ifdef __MKSDLP_V20__
void EXTI0_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(ZE_MIN_Pin);
  EndstopsInterrupt();
}

void EXTI2_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(ZE_MAX_Pin);
  EndstopsInterrupt();
}
  #endif  // __MKSDLP_V20__
#endif  // __MKSDLP_BOARD__


#ifdef __CHITU_BOARD__
  #ifdef __CHITU_LV3__
void EXTI15_10_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(ZE_MIN_Pin);
  HAL_GPIO_EXTI_IRQHandler(ZE_MAX_Pin);
  EndstopsInterrupt();
}

  #endif  // __CHITU_LV3__
#endif  // __CHITU_BOARD__

void WWDG_IRQHandler(void)
{
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
