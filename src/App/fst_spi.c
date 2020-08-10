#include "fst_spi.h"
#include "lcd_low.h"




SPI_HandleTypeDef	hTouchSpi;
DMA_HandleTypeDef	hTouchSpiDmaRx;
DMA_HandleTypeDef	hTouchSpiDmaTx;
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
	hTouchSpi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	hTouchSpi.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hTouchSpi.Init.TIMode = SPI_TIMODE_DISABLE;
	hTouchSpi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hTouchSpi.Init.CRCPolynomial = 10;
	__HAL_RCC_SPI2_CLK_ENABLE();
	HAL_SPI_Init(&hTouchSpi);
	__HAL_RCC_SPI2_CLK_DISABLE();
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
    
	hTouchSpiDmaRx.Instance = DMA1_Stream3;
    hTouchSpiDmaRx.Init.Channel = DMA_CHANNEL_0;
    hTouchSpiDmaRx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hTouchSpiDmaRx.Init.PeriphInc = DMA_PINC_DISABLE;
    hTouchSpiDmaRx.Init.MemInc = DMA_MINC_ENABLE;
    hTouchSpiDmaRx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hTouchSpiDmaRx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hTouchSpiDmaRx.Init.Mode = DMA_NORMAL;
    hTouchSpiDmaRx.Init.Priority = DMA_PRIORITY_LOW;
    hTouchSpiDmaRx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	HAL_DMA_Init(&hTouchSpiDmaRx);   
    __HAL_LINKDMA(&hTouchSpi, hdmarx, hTouchSpiDmaRx);

    hTouchSpiDmaTx.Instance = DMA1_Stream4;
    hTouchSpiDmaTx.Init.Channel = DMA_CHANNEL_0;
    hTouchSpiDmaTx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hTouchSpiDmaTx.Init.PeriphInc = DMA_PINC_DISABLE;
    hTouchSpiDmaTx.Init.MemInc = DMA_MINC_ENABLE;
    hTouchSpiDmaTx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hTouchSpiDmaTx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hTouchSpiDmaTx.Init.Mode = DMA_NORMAL;
    hTouchSpiDmaTx.Init.Priority = DMA_PRIORITY_LOW;
    hTouchSpiDmaTx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	HAL_DMA_Init(&hTouchSpiDmaTx);
    __HAL_LINKDMA(&hTouchSpi, hdmatx, hTouchSpiDmaTx);

	// DMA interrupt init
	// DMA1_Stream3_IRQn interrupt configuration
	HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
	
	// DMA1_Stream4_IRQn interrupt configuration
	HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);

	hTouchSpi.Instance->CR1 |= SPI_CR1_SPE;
}
//==============================================================================




void		TOUCH_SPIDisable()
{
	hTouchSpi.Instance->CR1 &= ~SPI_CR1_SPE;
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


















uint16_t	_flash_SPIGetFlags()
{
	return hFlashSpi.Instance->SR;
}
//==============================================================================




void		FLASH_SPIInit(void)
{
	
	hFlashSpi.Instance = SPI1;
	hFlashSpi.Init.Mode = SPI_MODE_MASTER;
	hFlashSpi.Init.Direction = SPI_DIRECTION_2LINES;
	hFlashSpi.Init.DataSize = SPI_DATASIZE_8BIT;
	hFlashSpi.Init.CLKPolarity = SPI_POLARITY_HIGH;
	hFlashSpi.Init.CLKPhase = SPI_PHASE_2EDGE;
	hFlashSpi.Init.NSS = SPI_NSS_SOFT;
	hFlashSpi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	hFlashSpi.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hFlashSpi.Init.TIMode = SPI_TIMODE_DISABLE;
	hFlashSpi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hFlashSpi.Init.CRCPolynomial = 10;
	__HAL_RCC_SPI1_CLK_ENABLE();
	HAL_SPI_Init(&hFlashSpi);
	__HAL_RCC_SPI1_CLK_DISABLE();
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
	__HAL_RCC_DMA2_CLK_ENABLE();
	
	// SPI1_RX Init
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
	HAL_DMA_Init(&hFlashSpiDmaRx);
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
	HAL_DMA_Init(&hFlashSpiDmaTx);
	__HAL_LINKDMA(&hFlashSpi, hdmatx, hFlashSpiDmaTx);

	// DMA interrupt init
	// DMA2_Stream2_IRQn interrupt configuration
	HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
	// DMA2_Stream5_IRQn interrupt configuration
	HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);

	hFlashSpi.Instance->CR1 |= SPI_CR1_SPE;
}
//==============================================================================




void		FLASH_SPIDisable()
{
	hFlashSpi.Instance->CR1 &= ~SPI_CR1_SPE;
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




// prescaler - SPI_BAUDRATEPRESCALER_2/4/8/16/32/64/128/256
// APB2 clock is 84Mhz
void		FLASH_SPISetSpeed(uint16_t prescaler)
{
	// Clear baudrate bits
	hFlashSpi.Instance->CR1 &= ~(SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0);
	// Set new baudrate bits
	hFlashSpi.Instance->CR1 |= prescaler;	// Set SPI1 speed
}
//==============================================================================




// return baudrate bits value - SPI_BAUDRATEPRESCALER_2/4/8/16/32/64/128/256
uint16_t		FLASH_SPIGetSpeed()
{
	return hFlashSpi.Instance->CR1 & (SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0);
}
//==============================================================================




uint8_t		FLASH_SPIWriteReadByte(uint8_t txval)
{
	uint16_t res = 0;

	while ((_flash_SPIGetFlags() & SPI_FLAG_TXE) == 0);
	hFlashSpi.Instance->DR = txval;
	while ((_flash_SPIGetFlags() & SPI_FLAG_RXNE) ==  0);
	res = hFlashSpi.Instance->DR;
/*
	while ((_flash_SPIGetFlags() & SPI_FLAG_BSY) || !(_flash_SPIGetFlags() & SPI_FLAG_TXE));
	hFlashSpi.Instance->DR = txval;
	while ((_flash_SPIGetFlags() & SPI_FLAG_RXNE) ==  0);
	res = hFlashSpi.Instance->DR;
*/
	return (uint8_t)res;
}
//==============================================================================




void		FLASH_SPIReadBuff(uint32_t dlen, uint8_t *dbuff)
{
	uint32_t	rlen = 0;
	uint8_t		*rbuff = dbuff;

	while ((_flash_SPIGetFlags() & SPI_FLAG_BSY) || !(_flash_SPIGetFlags() & SPI_FLAG_TXE));
	*rbuff = hFlashSpi.Instance->DR & 0x00FF;
	while (rlen < dlen)
	{
		hFlashSpi.Instance->DR = 0;
		while (!(_flash_SPIGetFlags() & SPI_FLAG_RXNE));
		*rbuff = hFlashSpi.Instance->DR & 0x00FF;
		rbuff++;
		rlen++;
	}
	
	return;
}
//==============================================================================




void		FLASH_SPIWriteBuff(uint32_t dlen, uint8_t *dbuff)
{
	uint32_t	rlen = 0;
	uint8_t		*rbuff = dbuff;

	while ((_flash_SPIGetFlags() & SPI_FLAG_BSY) || !(_flash_SPIGetFlags() & SPI_FLAG_TXE));
	*rbuff = hFlashSpi.Instance->DR & 0x00FF;
	while (rlen < dlen)
	{
		hFlashSpi.Instance->DR = *rbuff;
		while (!(_flash_SPIGetFlags() & SPI_FLAG_TXE));
		rbuff++;
		rlen++;
	}
	
	return;
}
//==============================================================================




void		FLASH_SPIReadBuffDMA(uint32_t dlen, uint8_t *dbuff)
{
	while ((_flash_SPIGetFlags() & SPI_FLAG_BSY) || (_flash_SPIGetFlags() & SPI_FLAG_TXE) == 0 || hFlashSpi.State != HAL_SPI_STATE_READY);
	HAL_SPI_Receive_DMA(&hFlashSpi, dbuff, dlen);
	while (hFlashSpi.State != HAL_SPI_STATE_READY);
	return;
}
//==============================================================================




void		FLASH_SPIWriteBuffDMA(uint32_t dlen, uint8_t *dbuff)
{
	while ((_flash_SPIGetFlags() & SPI_FLAG_BSY) || (_flash_SPIGetFlags() & SPI_FLAG_TXE) == 0 || hFlashSpi.State != HAL_SPI_STATE_READY);
	HAL_SPI_Transmit_DMA(&hFlashSpi, dbuff, dlen);
	while (hFlashSpi.State != HAL_SPI_STATE_READY);
	return;
}
//==============================================================================




uint8_t		FLASH_IsDMAReady()
{
	return (hFlashSpi.State == HAL_SPI_STATE_READY);
}
//==============================================================================




