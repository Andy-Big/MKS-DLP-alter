#include "ext_i2c.h"



#define	AT24C16_ADDR	(uint8_t)0xA0
#define FLAG_MASK		((uint32_t)0x00FFFFFF)


I2C_HandleTypeDef		hExtI2C;

uint8_t				exti2c_timer;


void		EXTI2C_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	EXT_I2C_CLK_ENABLE();

	hExtI2C.Instance = EXT_I2C;
	hExtI2C.Init.ClockSpeed = 100000;
	hExtI2C.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hExtI2C.Init.OwnAddress1 = 1;
	hExtI2C.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hExtI2C.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hExtI2C.Init.OwnAddress2 = 0;
	hExtI2C.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hExtI2C.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	HAL_I2C_Init(&hExtI2C);

//	HAL_I2CEx_ConfigAnalogFilter(&hExtI2C, I2C_ANALOGFILTER_ENABLE);
	
	GPIO_InitStruct.Pin = EXT_I2C_SCL_Pin | EXT_I2C_SDA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
	HAL_GPIO_Init(EXT_I2C_GPIO_Port, &GPIO_InitStruct);

	__HAL_I2C_ENABLE(&hExtI2C);
	
	exti2c_timer = SYSTIMER_NewCountDown(0);
}
//==============================================================================




void		EXTI2C_Deinit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_I2C_DISABLE(&hExtI2C);
	HAL_I2C_DeInit(&hExtI2C);

	GPIO_InitStruct.Pin = EXT_I2C_SCL_Pin | EXT_I2C_SDA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(EXT_I2C_GPIO_Port, &GPIO_InitStruct);
}
//==============================================================================




uint32_t 	_EXTI2C_get_flag_status(uint32_t flag)
{
	return __HAL_I2C_GET_FLAG(&hExtI2C, flag);
}
//==============================================================================





uint32_t 	_EXTI2C_check_errors()
{
	return (hExtI2C.Instance->SR1 & (_I2C_FLAG_SMBALERT | _I2C_FLAG_TIMEOUT | _I2C_FLAG_PECERR | _I2C_FLAG_OVR | _I2C_FLAG_AF | _I2C_FLAG_ARLO | _I2C_FLAG_BERR));
}
//==============================================================================





uint8_t		_EXTI2C_CheckEvent(uint32_t I2C_EVENT)
{
  uint32_t lastevent = 0;
  uint32_t flag1 = 0, flag2 = 0;

  flag1 = hExtI2C.Instance->SR1;
  flag2 = hExtI2C.Instance->SR2;
  flag2 = flag2 << 16;

  lastevent = (flag1 | flag2) & FLAG_MASK;

  if ((lastevent & I2C_EVENT) == I2C_EVENT)
  {
	  return 1;
  }
  else
  {
	  return 0;
  }
}
//==============================================================================





void		_EXTI2C_Send7bitAddress(uint8_t Address, uint8_t I2C_Direction)
{
  if (I2C_Direction != 1)
  {
    /* Set the address bit0 for read */
    Address |= I2C_OAR1_ADD0;
  }
  else
  {
    /* Reset the address bit0 for write */
    Address &= (uint8_t)~((uint8_t)I2C_OAR1_ADD0);
  }
  hExtI2C.Instance->DR = Address;
}
//==============================================================================





void 	_EXTI2C_abort()
{
	if (_EXTI2C_get_flag_status(_I2C_FLAG_BUSY))
		hExtI2C.Instance->CR1 |= I2C_CR1_STOP;
	hExtI2C.Instance->CR1 = 1;
	hExtI2C.Instance->SR1 = hExtI2C.Instance->SR1 & ~(_I2C_FLAG_SMBALERT | _I2C_FLAG_TIMEOUT | _I2C_FLAG_PECERR | _I2C_FLAG_OVR | _I2C_FLAG_AF | _I2C_FLAG_ARLO | _I2C_FLAG_BERR);
}
//==============================================================================





uint8_t 	_EXTI2C_generate_start()
{
	hExtI2C.Instance->CR1 |= I2C_CR1_START;
	SYSTIMER_SetCountDown(exti2c_timer, 100);
	while (!_EXTI2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT) && SYSTIMER_GetCountDown(exti2c_timer))
	{
		if (_EXTI2C_check_errors())
			SYSTIMER_SetCountDown(exti2c_timer, 0);
	}
	if (SYSTIMER_GetCountDown(exti2c_timer) == 0)
	{
		_EXTI2C_abort();
		return 0;
	}
	
	return 1;
}
//==============================================================================





uint8_t 	_EXTI2C_generate_stop()
{
	hExtI2C.Instance->CR1 |= I2C_CR1_STOP;
	return 1;
}
//==============================================================================





uint8_t 	_EXTI2C_send_addr_send(uint8_t addr)
{
    addr &= (uint8_t)~((uint8_t)I2C_OAR1_ADD0);
	hExtI2C.Instance->DR = addr;
	SYSTIMER_SetCountDown(exti2c_timer, 100);
	while (!_EXTI2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && SYSTIMER_GetCountDown(exti2c_timer))
	{
		if (_EXTI2C_check_errors())
			SYSTIMER_SetCountDown(exti2c_timer, 0);
	}
	if (SYSTIMER_GetCountDown(exti2c_timer) == 0)
	{
		HAL_Delay(1);
		_EXTI2C_abort();
		return 0;
	}
	
	return 1;
}
//==============================================================================





uint8_t 	_EXTI2C_send_addr_receive(uint8_t addr)
{
    addr |= I2C_OAR1_ADD0;
	hExtI2C.Instance->DR = addr;
	SYSTIMER_SetCountDown(exti2c_timer, 100);
	while (!_EXTI2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) && SYSTIMER_GetCountDown(exti2c_timer))
	{
		HAL_Delay(1);
		if (_EXTI2C_check_errors())
			SYSTIMER_SetCountDown(exti2c_timer, 0);
	}
	if (SYSTIMER_GetCountDown(exti2c_timer) == 0)
	{
		HAL_Delay(1);
		_EXTI2C_abort();
		return 0;
	}
	return 1;
}
//==============================================================================





uint8_t 	_EXTI2C_send_memaddr_send(uint16_t addr)
{
	uint8_t addr_h = (uint8_t)((addr & 0xFF00) >> 7) & 0x0E;
	uint8_t addr_l = (uint8_t)(addr & 0x00FF);
	
	addr_h |= AT24C16_ADDR;
	hExtI2C.Instance->DR = addr_h;
	SYSTIMER_SetCountDown(exti2c_timer, 100);
	while (!_EXTI2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && SYSTIMER_GetCountDown(exti2c_timer))
	{
		if (_EXTI2C_check_errors())
			SYSTIMER_SetCountDown(exti2c_timer, 0);
	}
	if (SYSTIMER_GetCountDown(exti2c_timer) == 0)
	{
		_EXTI2C_abort();
		return 0;
	}
	hExtI2C.Instance->DR = addr_l;
	SYSTIMER_SetCountDown(exti2c_timer, 100);
	while (!_EXTI2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED) && SYSTIMER_GetCountDown(exti2c_timer))
	{
		if (_EXTI2C_check_errors())
			SYSTIMER_SetCountDown(exti2c_timer, 0);
	}
	if (SYSTIMER_GetCountDown(exti2c_timer) == 0)
	{
		_EXTI2C_abort();
		return 0;
	}
	
	return 1;
}
//==============================================================================





uint8_t 	_EXTI2C_send_memaddr_receive(uint16_t addr)
{
	uint8_t addr_h = (uint8_t)((addr & 0xFF00) >> 7) & 0x0E;
	uint8_t addr_l = (uint8_t)(addr & 0x00FF);
	
	addr_h |= (AT24C16_ADDR | 0x01);
	
	hExtI2C.Instance->DR = addr_h;
	SYSTIMER_SetCountDown(exti2c_timer, 100);
	while (!_EXTI2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) && SYSTIMER_GetCountDown(exti2c_timer))
	{
		if (_EXTI2C_check_errors())
			SYSTIMER_SetCountDown(exti2c_timer, 0);
	}
	if (SYSTIMER_GetCountDown(exti2c_timer) == 0)
	{
		_EXTI2C_abort();
		return 0;
	}
	hExtI2C.Instance->DR = addr_l;
	SYSTIMER_SetCountDown(exti2c_timer, 100);
	while (!_EXTI2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED) && SYSTIMER_GetCountDown(exti2c_timer))
	{
		if (_EXTI2C_check_errors())
			SYSTIMER_SetCountDown(exti2c_timer, 0);
	}
	if (SYSTIMER_GetCountDown(exti2c_timer) == 0)
	{
		_EXTI2C_abort();
		return 0;
	}
	
	return 1;
}
//==============================================================================





uint8_t 	_EXTI2C_send_data(uint8_t data)
{
	hExtI2C.Instance->DR = data;
	SYSTIMER_SetCountDown(exti2c_timer, 100);
	while (!_EXTI2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED) && SYSTIMER_GetCountDown(exti2c_timer))
	{
		if (_EXTI2C_check_errors())
			SYSTIMER_SetCountDown(exti2c_timer, 0);
	}
	if (SYSTIMER_GetCountDown(exti2c_timer) == 0)
	{
		_EXTI2C_abort();
		return 0;
	}
	return 1;
}
//==============================================================================





// попытка чтения регистра из устройства на шине, возвращает 0 если попытка удалась или 1 если устройство не отвечает
uint8_t 	EXTI2C_CheckRead(uint8_t addr, uint8_t reg, uint8_t *result)
{
	if (exti2c_timer == INVALID_TIMER)
		return 0;
	
	SYSTIMER_SetCountDown(exti2c_timer, 100);
	while (_EXTI2C_get_flag_status(I2C_FLAG_BUSY) && SYSTIMER_GetCountDown(exti2c_timer));
	if (SYSTIMER_GetCountDown(exti2c_timer) == 0)
	{
		_EXTI2C_abort();
		return 0;
	}
	
	hExtI2C.Instance->CR1 |= I2C_CR1_ACK; // Enable I2C acknowledge
	if (!_EXTI2C_generate_start())
	{
		return 0;
	}

	if (!_EXTI2C_send_addr_send(addr))
	{
		return 0;
	}

	if (!_EXTI2C_send_data(reg))
	{
		return 0;
	}
	
	if (!_EXTI2C_generate_start())
	{
		return 0;
	}

	if (!_EXTI2C_send_addr_receive(addr))
	{
		return 0;
	}

	SYSTIMER_SetCountDown(exti2c_timer, 100);
	while (!_EXTI2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED) && SYSTIMER_GetCountDown(exti2c_timer))
	{
		if (_EXTI2C_check_errors())
			SYSTIMER_SetCountDown(exti2c_timer, 0);
	}
	if (SYSTIMER_GetCountDown(exti2c_timer) == 0)
	{
		_EXTI2C_abort();
		return 0;
	}
	
	uint8_t r = (uint8_t)hExtI2C.Instance->DR; // Receive ChipID
	if (result != 0)
		*result = r;
	hExtI2C.Instance->CR1 &= (uint16_t)~((uint16_t)I2C_CR1_ACK); // Disable I2C acknowledgment
	_EXTI2C_generate_stop(); // Send STOP condition
	
	return 1;
}
//==============================================================================	




uint8_t		EXTI2C_ChangeSpeed(uint32_t spd)
{
	if (exti2c_timer == INVALID_TIMER)
		return 0;

	SYSTIMER_SetCountDown(exti2c_timer, 100);
	while (_EXTI2C_get_flag_status(I2C_FLAG_BUSY) && SYSTIMER_GetCountDown(exti2c_timer));
	if (SYSTIMER_GetCountDown(exti2c_timer) == 0)
		return 0;

	__HAL_I2C_DISABLE(&hExtI2C);
	hExtI2C.Init.ClockSpeed = spd;
	HAL_I2C_Init(&hExtI2C);
	__HAL_I2C_ENABLE(&hExtI2C);
	
	SYSTIMER_SetCountDown(exti2c_timer, 100);
	while (_EXTI2C_get_flag_status(I2C_FLAG_BUSY) && SYSTIMER_GetCountDown(exti2c_timer));
	if (SYSTIMER_GetCountDown(exti2c_timer) == 0)
		return 0;

	spd /= 1000 + 100;
	while(spd > 0)
		spd--;
	
	return 1;
}
//==============================================================================	




uint8_t	EXTI2C_WriteReg(uint8_t addr, uint8_t reg, uint8_t value)
{
	if (exti2c_timer == INVALID_TIMER)
		return 0;
	
	SYSTIMER_SetCountDown(exti2c_timer, 100);
	while (_EXTI2C_get_flag_status(I2C_FLAG_BUSY) && SYSTIMER_GetCountDown(exti2c_timer));
	if (SYSTIMER_GetCountDown(exti2c_timer) == 0)
		return 0;

	if (!_EXTI2C_generate_start())
	{
		return 0;
	}
	
	if (!_EXTI2C_send_addr_send(addr))
	{
		return 0;
	}

	
	if (!_EXTI2C_send_data(reg))
	{
		return 0;
	}
	
	if (!_EXTI2C_send_data(value))
	{
		return 0;
	}
	
	_EXTI2C_generate_stop();
	
	return 1;
}
//==============================================================================	




uint8_t 	EXTI2C_ReadReg(uint8_t addr, uint8_t reg)
{
	if (exti2c_timer == INVALID_TIMER)
		return 0;
	
	uint8_t value = 0;

	SYSTIMER_SetCountDown(exti2c_timer, 100);
	while (_EXTI2C_get_flag_status(I2C_FLAG_BUSY) && SYSTIMER_GetCountDown(exti2c_timer));
	if (SYSTIMER_GetCountDown(exti2c_timer) == 0)
		return 0;

	hExtI2C.Instance->CR1 |= I2C_CR1_ACK; // Enable I2C acknowledge
	if (!_EXTI2C_generate_start())
	{
		return 0;
	}
	
	if (!_EXTI2C_send_addr_send(addr))
	{
		return 0;
	}

	if (!_EXTI2C_send_data(reg))
	{
		return 0;
	}
	
	if (!_EXTI2C_generate_start())
	{
		return 0;
	}
	
	
	if (!_EXTI2C_send_addr_receive(addr))
	{
		return 0;
	}

	SYSTIMER_SetCountDown(exti2c_timer, 100);
	while (!_EXTI2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED) && SYSTIMER_GetCountDown(exti2c_timer))
	{
		if (_EXTI2C_check_errors())
			SYSTIMER_SetCountDown(exti2c_timer, 0);
	}
	if (SYSTIMER_GetCountDown(exti2c_timer) == 0)
	{
		_EXTI2C_abort();
		return 0;
	}
	value = (uint8_t)hExtI2C.Instance->DR; // Receive ChipID
	hExtI2C.Instance->CR1 &= (uint16_t)~((uint16_t)I2C_CR1_ACK); // Disable I2C acknowledgment
	_EXTI2C_generate_stop(); // Send STOP condition

	return value;
}
//==============================================================================	




uint8_t		EXTI2C_ReadBuff(uint8_t addr, uint8_t reg, uint8_t *buff, uint16_t count)
{
	if (exti2c_timer == INVALID_TIMER)
		return 0;
	
	if (count == 0)
		return 0;
	
	SYSTIMER_SetCountDown(exti2c_timer, 100);
	while (_EXTI2C_get_flag_status(I2C_FLAG_BUSY) && SYSTIMER_GetCountDown(exti2c_timer));
	if (SYSTIMER_GetCountDown(exti2c_timer) == 0)
		return 0;

	hExtI2C.Instance->CR1 |= I2C_CR1_ACK; // Enable I2C acknowledge
	if (!_EXTI2C_generate_start())
	{
		return 0;
	}
	
	if (!_EXTI2C_send_addr_send(addr))
	{
		return 0;
	}

	if (!_EXTI2C_send_data(reg))
	{
		return 0;
	}
	
	if (!_EXTI2C_generate_start())
	{
		return 0;
	}

	if (!_EXTI2C_send_addr_receive(addr))
	{
		return 0;
	}

	while (count > 0)
	{
		SYSTIMER_SetCountDown(exti2c_timer, 100);
		while (!_EXTI2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED) && SYSTIMER_GetCountDown(exti2c_timer))
		{
			if (_EXTI2C_check_errors())
				SYSTIMER_SetCountDown(exti2c_timer, 0);
		}
		if (SYSTIMER_GetCountDown(exti2c_timer) == 0)
		{
			_EXTI2C_abort();
			return 0;
		}
		*buff = (uint8_t)hExtI2C.Instance->DR;
		buff++;
		count--;
	}
	
	hExtI2C.Instance->CR1 &= (uint16_t)~((uint16_t)I2C_CR1_ACK); // Disable I2C acknowledgment
	_EXTI2C_generate_stop(); // Send STOP condition

	return 1;
}
//==============================================================================	




uint8_t		EXTI2C_WriteBuff(uint8_t addr, uint8_t reg, uint8_t *buff, uint16_t count)
{
	if (exti2c_timer == INVALID_TIMER)
		return 0;
	
	if (count == 0)
		return 0;
	
	SYSTIMER_SetCountDown(exti2c_timer, 100);
	while (_EXTI2C_get_flag_status(I2C_FLAG_BUSY) && SYSTIMER_GetCountDown(exti2c_timer));
	if (SYSTIMER_GetCountDown(exti2c_timer) == 0)
		return 0;

	hExtI2C.Instance->CR1 |= I2C_CR1_ACK; // Enable I2C acknowledge
	if (!_EXTI2C_generate_start())
	{
		return 0;
	}
	
	
	if (!_EXTI2C_send_addr_send(addr))
	{
		return 0;
	}

	if (!_EXTI2C_send_data(reg))
	{
		return 0;
	}
	
	while (count > 0)
	{
		if (!_EXTI2C_send_data(*buff))
		{
			return 0;
		}
		count--;
		buff++;
	}
	
	hExtI2C.Instance->CR1 &= (uint16_t)~((uint16_t)I2C_CR1_ACK); // Disable I2C acknowledgment
	_EXTI2C_generate_stop();
	
	return 1;
}
//==============================================================================	




