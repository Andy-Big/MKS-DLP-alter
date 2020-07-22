#include "touch_spi.h"
#include "lcd_low.h"

/* SPI2 init function */

SPI_HandleTypeDef	hTouch;
DMA_HandleTypeDef	hTouchDmaRx;
DMA_HandleTypeDef	hTouchDmaTx;
uint16_t			tchx_min = 1500, tchx_max = 31300, tchy_min = 1000, tchy_max = 31200;
uint16_t			tchx = 0, tchy = 0;

uint8_t				dmabuff[TOUCH_BUFF_SIZE];



void	Touch_CS_Enable()
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET);
}
//==============================================================================




void	Touch_CS_Disable()
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET);
}
//==============================================================================




void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi->Instance == hTouch.Instance)
		Touch_CS_Disable();
}
//==============================================================================






void Touch_Init(void)
{
	
	hTouch.Instance = SPI2;
	hTouch.Init.Mode = SPI_MODE_MASTER;
	hTouch.Init.Direction = SPI_DIRECTION_2LINES;
	hTouch.Init.DataSize = SPI_DATASIZE_8BIT;
	hTouch.Init.CLKPolarity = SPI_POLARITY_LOW;
	hTouch.Init.CLKPhase = SPI_PHASE_1EDGE;
	hTouch.Init.NSS = SPI_NSS_SOFT;
	hTouch.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
	hTouch.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hTouch.Init.TIMode = SPI_TIMODE_DISABLE;
	hTouch.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hTouch.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hTouch) != HAL_OK)
	{
		Error_Handler();
	}
}
//==============================================================================




void Touch_Enable()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* SPI2 clock enable */
	__HAL_RCC_SPI2_CLK_ENABLE();

	__HAL_RCC_GPIOB_CLK_ENABLE();
	/**SPI2 GPIO Configuration    
	PB10     ------> SPI2_SCK
	PB14     ------> SPI2_MISO
	PB15     ------> SPI2_MOSI 
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_14|GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	// Touch CS
	/*Configure GPIO pin : PD11 */
	__HAL_RCC_GPIOD_CLK_ENABLE();
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
    __HAL_LINKDMA(&hTouch, hdmarx, hTouchDmaRx);

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
    __HAL_LINKDMA(&hTouch, hdmatx, hTouchDmaTx);

	/* DMA interrupt init */
	/* DMA1_Stream3_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
	
	/* DMA1_Stream4_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);

//	HAL_DMA_RegisterCallback(&hTouchDmaRx, HAL_DMA_XFER_M1CPLT_CB_ID, TouchDMACallBack);
}
//==============================================================================




void Touch_Disable()
{
	/* Peripheral clock disable */
	__HAL_RCC_SPI2_CLK_DISABLE();

	/**SPI2 GPIO Configuration    
	PB10     ------> SPI2_SCK
	PB14     ------> SPI2_MISO
	PB15     ------> SPI2_MOSI 
	*/
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_14|GPIO_PIN_15);
	HAL_GPIO_DeInit(GPIOD, GPIO_PIN_11);

    HAL_DMA_DeInit(hTouch.hdmarx);
    HAL_DMA_DeInit(hTouch.hdmatx);
}
//==============================================================================




void	Touch_ReadCoords()
{
	if (hTouchDmaRx.State != HAL_DMA_STATE_READY)
	{
		tchx = tchy = 0;
		return;
	}
	
	uint32_t	vavg = 0;
	uint8_t		*buff = dmabuff+1;

	// Calculate X coord
	for (uint8_t i = 0; i < TOUCH_READS; i++)
	{
		vavg += (uint16_t)(*buff)<<8;
		buff++;
		vavg += *buff;
		buff += 5;
	}
	vavg = vavg / TOUCH_READS;
	if (vavg < tchx_min || vavg > tchx_max)
		tchx = 0;
	else
		tchx = LCD_WIDTH - (vavg - tchx_min) * LCD_WIDTH / (tchx_max - tchx_min);

	// Calculate X coord
	vavg = 0;
	buff = dmabuff+4;

	for (uint8_t i = 0; i < TOUCH_READS; i++)
	{
		vavg += (uint16_t)(*buff)<<8;
		buff++;
		vavg += *buff;
		buff += 5;
	}
	vavg = vavg / TOUCH_READS;
	if (vavg < tchy_min || vavg > tchy_max)
		tchy = 0;
	else
		tchy = (vavg - tchy_min) * LCD_HEIGHT / (tchy_max - tchy_min);
	
	
	// Start new read
	Touch_CS_Enable();
	memset(dmabuff, 0, TOUCH_BUFF_SIZE);
	
	for (uint8_t i = 0; i < TOUCH_BUFF_SIZE; i+=6)
		dmabuff[i] = 0x90;
	for (uint8_t i = 3; i < TOUCH_BUFF_SIZE; i+=6)
		dmabuff[i] = 0xD0;

	HAL_SPI_TransmitReceive_DMA(&hTouch, dmabuff, dmabuff, sizeof(dmabuff));
	
	dmabuff[0] = 0x90;
	return;
}
//==============================================================================




uint16_t	Touch_GetX()
{
	return tchx;
}
//==============================================================================




uint16_t	Touch_GetY()
{
	return tchy;
}
//==============================================================================




