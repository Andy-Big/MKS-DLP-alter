/**
  ******************************************************************************
  * File Name          : TIM.h
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
#ifndef __tim_H
#define __tim_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"

	 
#define STEPPER_TIMER_PRESCALE		105
	 
	 

extern TIM_HandleTypeDef	hStepperTim;
extern TIM_HandleTypeDef	hZRefTim;



void TIM_ZStepper_Init(void);
#ifdef __MKSDLP_BOARD__
void TIM_ZRef_Init(void);
#endif  // __MKSDLP_BOARD__
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);



#ifdef __cplusplus
}
#endif
#endif /*__ tim_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
