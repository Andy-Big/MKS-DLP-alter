#include "fst_spi.h"
#include "lcd_low.h"




SPI_HandleTypeDef	hTouchSpi;
DMA_HandleTypeDef	hTouchDmaRx;
DMA_HandleTypeDef	hTouchDmaTx;
uint8_t				touch_buff[TOUCH_BUFF_SIZE];

SPI_HandleTypeDef	hFlashSpi;
DMA_HandleTypeDef	hFlashSpiDmaRx;
DMA_HandleTypeDef	hFlashSpiDmaTx;






void		HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi->Instance == hTouchSpi.Instance)
		_touch_CS_Disable();
}
//==============================================================================














void		TOUCH_SPIInit(void)
{
	
	hTouchSpi.Instance = SPI2;
	hTouchSpi.Init.Mode = SPI_MODE_MASTER;
	hTouchSpi.Init.Direction = SPI_DIRECTION_2LINES;
	hTouchSpi.Init.DataSize = SPI_DATASIZE_8BIT;
	hTouchSpi.Init.CLKPolarity = SPI_POLARITY_LOW;
	hTouchSpi.Init.CLKPhase = SPI_PHASE_1EDGE;
	hTouchSpi.Init.NSS = SPI_NSS_SOFT;
	hTouchSpi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
	hTouchSpi.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hTouchSpi.Init.TIMode = SPI_TIMODE_DISABLE;
	hTouchSpi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hTouchSpi.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hTouchSpi) != HAL_OK)
	{
		Error_Handler();
	}
}
//==============================================================================




void		TOUCH_SPIEnable()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_SPI2_CLK_ENABLE();

	__HAL_RCC_GPIOB_CLK_ENABLE();
	// SPI2 GPIO Configuration    
	// PB10     ------> SPI2_SCK
	// PB14     ------> SPI2_MISO
	// PB15     ------> SPI2_MOSI 
	GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_14|GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	// Touch CS
	// Configure GPIO pin : PD11
	__HAL_RCC_GPIOD_CLK_ENABLE();
	_touch_CS_Disable();
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	// DMA
	__HAL_RCC_DMA1_CLK_ENABLE();
    
	hTouchDmaRx.Instance = DMA1_Stream3;
    hTouchDmaRx.Init.Channel = DMA_CHANNEL_0;
    hTouchDmaRx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hTouchDmaRx.Init.PeriphInc = DMA_PINC_DISABLE;
    hTouchDmaRx.Init.MemInc = DMA_MINC_ENABLE;
    hTouchDmaRx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hTouchDmaRx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hTouchDmaRx.Init.Mode = DMA_NORMAL;
    hTouchDmaRx.Init.Priority = DMA_PRIORITY_LOW;
    hTouchDmaRx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	HAL_DMA_Init(&hTouchDmaRx);   
    __HAL_LINKDMA(&hTouchSpi, hdmarx, hTouchDmaRx);

    hTouchDmaTx.Instance = DMA1_Stream4;
    hTouchDmaTx.Init.Channel = DMA_CHANNEL_0;
    hTouchDmaTx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hTouchDmaTx.Init.PeriphInc = DMA_PINC_DISABLE;
    hTouchDmaTx.Init.MemInc = DMA_MINC_ENABLE;
    hTouchDmaTx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hTouchDmaTx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hTouchDmaTx.Init.Mode = DMA_NORMAL;
    hTouchDmaTx.Init.Priority = DMA_PRIORITY_LOW;
    hTouchDmaTx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	HAL_DMA_Init(&hTouchDmaTx);
    __HAL_LINKDMA(&hTouchSpi, hdmatx, hTouchDmaTx);

	// DMA interrupt init
	// DMA1_Stream3_IRQn interrupt configuration
	HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
	
	// DMA1_Stream4_IRQn interrupt configuration
	HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);

}
//==============================================================================




void		TOUCH_SPIDisable()
{
	// Peripheral clock disable
	__HAL_RCC_SPI2_CLK_DISABLE();

	// SPI2 GPIO Configuration    
	// PB10     ------> SPI2_SCK
	// PB14     ------> SPI2_MISO
	// PB15     ------> SPI2_MOSI
	// PD11     ------> SPI2_CS
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_14|GPIO_PIN_15);
	HAL_GPIO_DeInit(GPIOD, GPIO_PIN_11);

    HAL_DMA_DeInit(hTouchSpi.hdmarx);
    HAL_DMA_DeInit(hTouchSpi.hdmatx);
}
//==============================================================================




void		TOUCH_SPIStartRead()
{
	_touch_CS_Enable();
	memset(touch_buff, 0, TOUCH_BUFF_SIZE);
	
	for (uint8_t i = 0; i < TOUCH_BUFF_SIZE; i+=6)
		touch_buff[i] = 0x90;
	for (uint8_t i = 3; i < TOUCH_BUFF_SIZE; i+=6)
		touch_buff[i] = 0xD0;

	HAL_SPI_TransmitReceive_DMA(&hTouchSpi, touch_buff, touch_buff, sizeof(touch_buff));
}
//==============================================================================




HAL_SPI_StateTypeDef	TOUCH_SPIGetState()
{
	return hTouchSpi.State;
}
//==============================================================================


















void		FLASH_SPIInit(void)
{
	
	hFlashSpi.Instance = SPI2;
	hFlashSpi.Init.Mode = SPI_MODE_MASTER;
	hFlashSpi.Init.Direction = SPI_DIRECTION_2LINES;
	hFlashSpi.Init.DataSize = SPI_DATASIZE_8BIT;
	hFlashSpi.Init.CLKPolarity = SPI_POLARITY_LOW;
	hFlashSpi.Init.CLKPhase = SPI_PHASE_1EDGE;
	hFlashSpi.Init.NSS = SPI_NSS_SOFT;
	hFlashSpi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
	hFlashSpi.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hFlashSpi.Init.TIMode = SPI_TIMODE_DISABLE;
	hFlashSpi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hFlashSpi.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hFlashSpi) != HAL_OK)
	{
		Error_Handler();
	}
}
//==============================================================================




void		FLASH_SPIEnable()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_SPI1_CLK_ENABLE();

	__HAL_RCC_GPIOB_CLK_ENABLE();
	// SPI1 GPIO Configuration    
	// PB3     ------> SPI1_SCK
	// PB4     ------> SPI1_MISO
	// PB5     ------> SPI1_MOSI 
	GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	// PA15    ------> FLASH CS
	// PA3     ------> SSD_B CS
	// PA7     ------> CPLD CS
	__HAL_RCC_GPIOA_CLK_ENABLE();
	_flash_CS_Disable();
	_ssdb_CS_Disable();
	_ssdb_CS_Disable();
	GPIO_InitStruct.Pin = GPIO_PIN_15 | GPIO_PIN_3 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	// PC3     ------> SSD_A CS
	__HAL_RCC_GPIOC_CLK_ENABLE();
	_ssda_CS_Disable();
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	// DMA
	hFlashSpiDmaRx.Instance = DMA2_Stream2;
	hFlashSpiDmaRx.Init.Channel = DMA_CHANNEL_3;
	hFlashSpiDmaRx.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hFlashSpiDmaRx.Init.PeriphInc = DMA_PINC_DISABLE;
	hFlashSpiDmaRx.Init.MemInc = DMA_MINC_ENABLE;
	hFlashSpiDmaRx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hFlashSpiDmaRx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	hFlashSpiDmaRx.Init.Mode = DMA_NORMAL;
	hFlashSpiDmaRx.Init.Priority = DMA_PRIORITY_LOW;
	hFlashSpiDmaRx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	if (HAL_DMA_Init(&hFlashSpiDmaRx) != HAL_OK)
	{
		Error_Handler();
	}

	__HAL_LINKDMA(&hFlashSpi, hdmarx, hFlashSpiDmaRx);

	// SPI1_TX Init
	hFlashSpiDmaTx.Instance = DMA2_Stream5;
	hFlashSpiDmaTx.Init.Channel = DMA_CHANNEL_3;
	hFlashSpiDmaTx.Init.Direction = DMA_MEMORY_TO_PERIPH;
	hFlashSpiDmaTx.Init.PeriphInc = DMA_PINC_DISABLE;
	hFlashSpiDmaTx.Init.MemInc = DMA_MINC_ENABLE;
	hFlashSpiDmaTx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hFlashSpiDmaTx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	hFlashSpiDmaTx.Init.Mode = DMA_NORMAL;
	hFlashSpiDmaTx.Init.Priority = DMA_PRIORITY_LOW;
	hFlashSpiDmaTx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	if (HAL_DMA_Init(&hFlashSpiDmaTx) != HAL_OK)
	{
		Error_Handler();
	}

	__HAL_LINKDMA(&hFlashSpi, hdmatx, hFlashSpiDmaTx);

	// DMA interrupt init
	// DMA2_Stream2_IRQn interrupt configuration
	HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
	// DMA2_Stream5_IRQn interrupt configuration
	HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);

}
//==============================================================================




void		FLASH_SPIDisable()
{
	// Peripheral clock disable
	__HAL_RCC_SPI1_CLK_DISABLE();

	// PB3     ------> SPI1_SCK
	// PB4     ------> SPI1_MISO
	// PB5     ------> SPI1_MOSI 
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
	// PA15    ------> FLASH CS
	// PA3     ------> SSD_B CS
	// PA7     ------> CPLD CS
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_15 | GPIO_PIN_3 | GPIO_PIN_7);
	// PC3     ------> SSD_A CS
	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_3);

	/* SPI1 DMA DeInit */
	HAL_DMA_DeInit(hFlashSpi.hdmarx);
	HAL_DMA_DeInit(hFlashSpi.hdmatx);
}
//==============================================================================




