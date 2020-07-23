#include "touch_spi.h"
#include "lcd_low.h"




#define TOUCH_PRESSED()		(touch_point.xc || touch_point.yc)
#define	_touch_CS_Enable()	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET)
#define _touch_CS_Disable()	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET)


SPI_HandleTypeDef	hTouch;
DMA_HandleTypeDef	hTouchDmaRx;
DMA_HandleTypeDef	hTouchDmaTx;

uint8_t			dmabuff[TOUCH_BUFF_SIZE];
TOUCH_INFO		touch_info;
TOUCH_POINT		touch_point;





void		_touch_ReadCoords()
{
	if (hTouchDmaRx.State != HAL_DMA_STATE_READY)
	{
		touch_point.xc = touch_point.yc = 0;
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
	if (vavg < touch_info.x_min || vavg > touch_info.x_max)
		touch_point.xc = 0;
	else
		touch_point.xc = LCD_WIDTH - (vavg - touch_info.x_min) * LCD_WIDTH / (touch_info.x_max - touch_info.x_min);

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
	if (vavg < touch_info.y_min || vavg > touch_info.y_max)
		touch_point.yc = 0;
	else
		touch_point.yc = (vavg - touch_info.y_min) * LCD_HEIGHT / (touch_info.y_max - touch_info.y_min);
	
	
	// Start new read
	_touch_CS_Enable();
	memset(dmabuff, 0, TOUCH_BUFF_SIZE);
	
	for (uint8_t i = 0; i < TOUCH_BUFF_SIZE; i+=6)
		dmabuff[i] = 0x90;
	for (uint8_t i = 3; i < TOUCH_BUFF_SIZE; i+=6)
		dmabuff[i] = 0xD0;

	HAL_SPI_TransmitReceive_DMA(&hTouch, dmabuff, dmabuff, sizeof(dmabuff));
	
	_touch_RefreshState();
	return;
}
//==============================================================================




void		_touch_RefreshState()
{
	switch (touch_info.state)
	{
		case TS_WORKED:
			if (!TOUCH_PRESSED())
			{
				touch_info.state = TS_FREE;
			}
			break;
		case TS_FREE:
		case TS_SRELEASED:
		case TS_LRELEASED:
			if (TOUCH_PRESSED())
			{
				touch_info.state = TS_PREPRESSED;
				touch_info.time = 0;
			}
			break;
		case TS_PREPRESSED:
			if (TOUCH_PRESSED())
			{
				if (touch_info.time > 6)
					touch_info.state = TS_SPRESSED;
				else
					touch_info.time++;
			}
			else
			{
				touch_info.state = TS_WORKED;
				touch_info.time = 0;
			}
			break;
		case TS_SPRESSED:
			if (TOUCH_PRESSED())
			{
				if (touch_info.time > 150)
				{
					touch_info.state = TS_LPRESSED;
				}
				else
					touch_info.time++;
			}
			else
			{
				touch_info.state = TS_SRELEASED;
				touch_info.time = 0;
			}
			break;
		case TS_LPRESSED:
			if (!TOUCH_PRESSED())
			{
				touch_info.state = TS_LRELEASED;
				touch_info.time = 0;
			}
			else
				touch_info.time++;
			break;
	}
}
//==============================================================================












void		HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi->Instance == hTouch.Instance)
		_touch_CS_Disable();
}
//==============================================================================






void		Touch_Init(void)
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

	touch_info.x_min = 1500;
	touch_info.x_max = 31300;
	touch_info.y_min = 1000;
	touch_info.y_max = 31200;
	touch_info.state = TS_FREE;
	touch_info.time = 0;

	touch_point.xc = 0;
	touch_point.yc = 0;
}
//==============================================================================




void		Touch_Enable()
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




void		Touch_Disable()
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




TOUCH_STATES	Touch_GetState()
{
	return touch_info.state;
}
//==============================================================================




void		Touch_SetState(TOUCH_STATES newstate)
{
	touch_info.state = newstate;
}
//==============================================================================




void		Touch_GetCoords(TOUCH_POINT *pt)
{
	pt->xc = touch_point.xc;
	pt->yc = touch_point.yc;
}
//==============================================================================




