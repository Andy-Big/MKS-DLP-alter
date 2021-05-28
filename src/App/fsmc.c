#include "fsmc.h"
#include "gpio.h"


SRAM_HandleTypeDef hsram1;

/* FSMC initialization function */
void MX_FSMC_Init(void)
{
  FSMC_NORSRAM_TimingTypeDef Timing = {0};
//  FSMC_NORSRAM_TimingTypeDef ExtTiming = {0};

  hsram1.Instance = FSMC_NORSRAM_DEVICE;
  hsram1.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
  /* hsram1.Init */
#ifdef __MKSDLP_BOARD__
  hsram1.Init.NSBank = FSMC_NORSRAM_BANK1;
#endif
#ifdef __CHITU_BOARD__
  hsram1.Init.NSBank = FSMC_NORSRAM_BANK4;
#endif
  hsram1.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
  hsram1.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;
  hsram1.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
  hsram1.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
  hsram1.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
  hsram1.Init.WrapMode = FSMC_WRAP_MODE_DISABLE;
  hsram1.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
  hsram1.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
  hsram1.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
  hsram1.Init.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
  hsram1.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
  hsram1.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
  hsram1.Init.PageSize = FSMC_PAGE_SIZE_NONE;

/*
readWriteTiming.FSMC_AddressSetupTime = 0XF; //Address setup time (ADDSET) is 16 HCLKs 1/168M=6ns*16=96ns
readWriteTiming.FSMC_AddressHoldTime = 0x00; //Address hold time (ADDHLD) mode A is not used
readWriteTiming.FSMC_DataSetupTime = 60; //Data saving time is 60 HCLK = 6*60=360ns
readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
readWriteTiming.FSMC_CLKDivision = 0x00;
readWriteTiming.FSMC_DataLatency = 0x00;
readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A; //Mode A


writeTiming.FSMC_AddressSetupTime =9; //Address setup time (ADDSET) is 9 HCLK =54ns
writeTiming.FSMC_AddressHoldTime = 0x00; //Address hold time (A
writeTiming.FSMC_DataSetupTime = 8; //Data save time is 6ns*9 HCLK=54ns
writeTiming.FSMC_BusTurnAroundDuration = 0x00;
writeTiming.FSMC_CLKDivision = 0x00;
writeTiming.FSMC_DataLatency = 0x00;
writeTiming.FSMC_AccessMode = FSMC_AccessMode_A; //Mode A


	// Old (original) timing

  // Timing
  Timing.AddressSetupTime = 15;
  Timing.AddressHoldTime = 15;
  Timing.DataSetupTime = 255;
  Timing.BusTurnAroundDuration = 15;
  Timing.CLKDivision = 16;
  Timing.DataLatency = 17;
  Timing.AccessMode = FSMC_ACCESS_MODE_A;
  // ExtTiming
  ExtTiming.AddressSetupTime = 15;
  ExtTiming.AddressHoldTime = 15;
  ExtTiming.DataSetupTime = 255;
  ExtTiming.BusTurnAroundDuration = 15;
  ExtTiming.CLKDivision = 16;
  ExtTiming.DataLatency = 17;
  ExtTiming.AccessMode = FSMC_ACCESS_MODE_A;
*/
  
  /* Timing */
/*
  // CHITU original settings
  // 0x00203C0F
  Timing.AddressSetupTime = 15;
  Timing.AddressHoldTime = 0;
  Timing.DataSetupTime = 60;
  Timing.BusTurnAroundDuration = 0;
  Timing.CLKDivision = 2;
  Timing.DataLatency = 0;
  Timing.AccessMode = FSMC_ACCESS_MODE_A;
*/
  Timing.AddressSetupTime = 5;
  Timing.AddressHoldTime = 0;
  Timing.DataSetupTime = 4;
  Timing.BusTurnAroundDuration = 0;
  Timing.CLKDivision = 0;
  Timing.DataLatency = 0;
  Timing.AccessMode = FSMC_ACCESS_MODE_A;

  if (HAL_SRAM_Init(&hsram1, &Timing,  &Timing) != HAL_OK)
  {
    Error_Handler( );
  }

}

static uint32_t FSMC_Initialized = 0;

void HAL_FSMC_MspInit(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if (FSMC_Initialized)
	{
		return;
	}
	FSMC_Initialized = 1;

	/* Peripheral clock enable */
	__HAL_RCC_FSMC_CLK_ENABLE();

	/* GPIO_InitStruct */
	GPIO_InitStruct.Pin = FSMC_Pins1;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;
	HAL_GPIO_Init(FSMC_GPIO1, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = FSMC_Pins2;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;
	HAL_GPIO_Init(FSMC_GPIO2, &GPIO_InitStruct);

	if (FSMC_GPIO3 != 0)
	{
		GPIO_InitStruct.Pin = FSMC_Pins3;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;
		HAL_GPIO_Init(FSMC_GPIO3, &GPIO_InitStruct);
	}
}

void HAL_SRAM_MspInit(SRAM_HandleTypeDef* sramHandle)
{
	HAL_FSMC_MspInit();
}

static uint32_t FSMC_DeInitialized = 0;

static void HAL_FSMC_MspDeInit(void)
{
	if (FSMC_DeInitialized)
	{
		return;
	}
	FSMC_DeInitialized = 1;
	/* Peripheral clock enable */
	__HAL_RCC_FSMC_CLK_DISABLE();

	HAL_GPIO_DeInit(FSMC_GPIO1, FSMC_Pins1);
	HAL_GPIO_DeInit(FSMC_GPIO2, FSMC_Pins2);
	if (FSMC_GPIO3 != 0)
	{
		HAL_GPIO_DeInit(FSMC_GPIO3, FSMC_Pins3);
	}
}

void HAL_SRAM_MspDeInit(SRAM_HandleTypeDef* sramHandle){
  /* USER CODE BEGIN SRAM_MspDeInit 0 */

  /* USER CODE END SRAM_MspDeInit 0 */
  HAL_FSMC_MspDeInit();
  /* USER CODE BEGIN SRAM_MspDeInit 1 */

  /* USER CODE END SRAM_MspDeInit 1 */
}
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
