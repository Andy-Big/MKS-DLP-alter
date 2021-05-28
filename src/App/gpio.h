#ifndef __gpio_H
#define __gpio_H
#ifdef __cplusplus
 extern "C" {
#endif
#include "main.h"





#ifdef __MKSDLP_BOARD__
  #ifdef __MKSDLP_V11__
	// --------- FSMC (TFT) ----------
	#define FSMC_Pins1				GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15
	#define FSMC_GPIO1				GPIOD
	#define FSMC_Pins2				GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15
	#define FSMC_GPIO2				GPIOE
	#define FSMC_Pins3				0
	#define FSMC_GPIO3				0

	// --------- TFT ----------
	#define TFT_LIGHT_Pin			GPIO_PIN_12
	#define TFT_LIGHT_GPIO			GPIOB

	#define TFT_RESET_Pin			GPIO_PIN_13
	#define TFT_RESET_GPIO			GPIOD

	// --------- SPI2 (TOUCH) ----------
	#define TOUCH_SPI					SPI2
	#define TOUCH_SPI_MISO_Pin			GPIO_PIN_14
	#define TOUCH_SPI_MISO_GPIO			GPIOB
	#define TOUCH_SPI_MOSI_Pin			GPIO_PIN_15
	#define TOUCH_SPI_MOSI_GPIO			GPIOB
	#define TOUCH_SPI_SCK_Pin			GPIO_PIN_10
	#define TOUCH_SPI_SCK_GPIO			GPIOB
	#define TOUCH_SPI_GPIO_ALTERNATE	GPIO_AF5_SPI2

	#define TOUCH_CS_Pin				GPIO_PIN_11
	#define TOUCH_CS_GPIO				GPIOD

	#define TOUCH_SPI_DMA_STREAM1		DMA1_Stream3
	#define TOUCH_SPI_DMA_CHANNEL1		DMA_CHANNEL_0
	#define TOUCH_SPI_DMA_STREAM1_IRQ	DMA1_Stream3_IRQn
	#define TOUCH_SPI_DMA_STREAM2		DMA1_Stream4
	#define TOUCH_SPI_DMA_CHANNEL2		DMA_CHANNEL_0
	#define TOUCH_SPI_DMA_STREAM2_IRQ	DMA1_Stream4_IRQn

	#define TOUCH_SPI_CLK_ENABLE()			__HAL_RCC_SPI2_CLK_ENABLE()
	#define TOUCH_SPI_CLK_DISABLE()			__HAL_RCC_SPI2_CLK_DISABLE()
	#define TOUCH_SPI_DMA_CLK_ENABLE()		__HAL_RCC_DMA1_CLK_ENABLE()
	#define TOUCH_SPI_DMA_CLK_DISABLE()		__HAL_RCC_DMA1_CLK_DISABLE()

	// --------- ENDSTOPS ----------
	#define ZE_MIN_Pin				GPIO_PIN_0
	#define ZE_MIN_GPIO				GPIOC
	#define ZE_MIN_EXTI_IRQn		EXTI0_IRQn
		
	#define ZE_MAX_Pin				GPIO_PIN_2
	#define ZE_MAX_GPIO				GPIOC
	#define ZE_MAX_EXTI_IRQn		EXTI2_IRQn

	// --------- STEPPER ----------
	#define Z_DIR_Pin				GPIO_PIN_1
	#define Z_DIR_GPIO				GPIOC

	#define Z_STEP_Pin				GPIO_PIN_1
	#define Z_STEP_GPIO				GPIOA

	#define Z_ENA_Pin				GPIO_PIN_2
	#define Z_ENA_GPIO				GPIOA

	#define Z_REF_Pin				GPIO_PIN_0
	#define Z_REF_GPIO				GPIOA
	#define Z_REF_ALTERNATE			GPIO_AF2_TIM5

	#define Z_REF_TIMER				TIM5
	#define Z_REF_TIMER_IRQ			TIM5_IRQn

	#define Z_REF_TIMER_CLK_ENABLE()		__HAL_RCC_TIM5_CLK_ENABLE()
	#define Z_REF_TIMER_CLK_DISABLE()		__HAL_RCC_TIM5_CLK_DISABLE()


	// --------- SPI1 (FLASH/CPLD/SSD2828) ----------
	#define FST_SPI					SPI1
	#define FST_SPI_MISO_Pin		GPIO_PIN_4
	#define FST_SPI_MISO_GPIO		GPIOB
	#define FST_SPI_MOSI_Pin		GPIO_PIN_5
	#define FST_SPI_MOSI_GPIO		GPIOB
	#define FST_SPI_SCK_Pin			GPIO_PIN_3
	#define FST_SPI_SCK_GPIO		GPIOB
	#define FST_SPI_GPIO_ALTERNATE	GPIO_AF5_SPI1

	#define FST_SPI_DMA_STREAM1		DMA2_Stream2
	#define FST_SPI_DMA_CHANNEL1	DMA_CHANNEL_3
	#define FST_SPI_DMA_STREAM1_IRQ	DMA2_Stream2_IRQn
	#define FST_SPI_DMA_STREAM2		DMA2_Stream5
	#define FST_SPI_DMA_CHANNEL2	DMA_CHANNEL_3
	#define FST_SPI_DMA_STREAM2_IRQ	DMA2_Stream5_IRQn

	#define FST_SPI_CLK_ENABLE()			__HAL_RCC_SPI1_CLK_ENABLE()
	#define FST_SPI_CLK_DISABLE()			__HAL_RCC_SPI1_CLK_DISABLE()
	#define FST_SPI_DMA_CLK_ENABLE()		__HAL_RCC_DMA2_CLK_ENABLE()
	#define FST_SPI_DMA_CLK_DISABLE()		__HAL_RCC_DMA2_CLK_DISABLE()

	#define FST_SPI_CLOCKSPEED		SPI_BAUDRATEPRESCALER_4

	// --------- CPLD/SSD2828 ----------
	#define FST_SPI_CS_CPLD_Pin			GPIO_PIN_7
	#define FST_SPI_CS_CPLD_GPIO		GPIOA

	#define FST_SPI_CS_SSDA_Pin			GPIO_PIN_3
	#define FST_SPI_CS_SSDA_GPIO		GPIOC

	#define FST_SPI_CS_SSDB_Pin			GPIO_PIN_3
	#define FST_SPI_CS_SSDB_GPIO		GPIOA

	#define FST_SPI_SDC_Pin				GPIO_PIN_4
	#define FST_SPI_SDC_GPIO			GPIOC

	#define MIPI_CE_Pin					GPIO_PIN_4
	#define MIPI_CE_GPIO				GPIOA

	#define MIPI_EN_PWR_Pin				GPIO_PIN_5
	#define MIPI_EN_PWR_GPIO			GPIOA

	#define CPLD_IO1_Pin				GPIO_PIN_5
	#define CPLD_IO1_GPIO	 			GPIOC

	#define CPLD_CRC_Pin				GPIO_PIN_0
	#define CPLD_CRC_GPIO				GPIOB

	#define CPLD_RST_Pin				GPIO_PIN_1
	#define CPLD_RST_GPIO				GPIOB

	// --------- FLASH ----------
	#define FST_SPI_CS_FLASH_Pin		GPIO_PIN_15
	#define FST_SPI_CS_FLASH_GPIO		GPIOA

	// --------- USB FS ----------
	#define USBH_VBUS_Pin			GPIO_PIN_3
	#define USBH_VBUS_GPIO			GPIOD

	#define USBH_DATA_N_Pin			GPIO_PIN_11
	#define USBH_DATA_N_GPIO		GPIOA
	#define USBH_DATA_P_Pin			GPIO_PIN_12
	#define USBH_DATA_P_GPIO		GPIOA
		
	// --------- LED/FANS ----------
	#define LED_UV_Pin				GPIO_PIN_2
	#define LED_UV_GPIO				GPIOE

	#define FAN_LED_Pin				GPIO_PIN_4
	#define FAN_LED_GPIO			GPIOE

	#define FAN_MB_Pin				GPIO_PIN_3
	#define FAN_MB_GPIO				GPIOE

	#define FAN_MB_On()				FAN_MB_GPIO->BSRR = FAN_MB_Pin						// Set pin
	#define FAN_MB_Off()			FAN_MB_GPIO->BSRR = (uint32_t)FAN_MB_Pin << 16U	// Reset pin

	// --------- BUZZER ----------
	#define BUZZER_Pin				GPIO_PIN_13
	#define BUZZER_GPIO				GPIOB

	// --------- I2C EEPROM ----------
	#define EEPROM_I2C				I2C1

	#define EEPROM_SCL_Pin			GPIO_PIN_6
	#define EEPROM_SCL_GPIO			GPIOB
	#define EEPROM_SDA_Pin			GPIO_PIN_7
	#define EEPROM_SDA_GPIO			GPIOB

	#define EEPROM_CLK_ENABLE()		__HAL_RCC_I2C1_CLK_ENABLE()
	#define EEPROM_CLK_DISABLE()	__HAL_RCC_I2C1_CLK_DISABLE()

	// --------- I2C DS3231 ----------
	#define DS3231_I2C				I2C1

	#define DS3231_I2C_SCL_Pin		GPIO_PIN_8
	#define DS3231_I2C_SCL_GPIO		GPIOB
	#define DS3231_I2C_SDA_Pin		GPIO_PIN_9
	#define DS3231_I2C_SDA_GPIO		GPIOB

	#define DS3231_CLK_ENABLE()		__HAL_RCC_I2C1_CLK_ENABLE()
	#define DS3231_CLK_DISABLE()	__HAL_RCC_I2C1_CLK_DISABLE()

	// --------- I2C EXTENSION PORT ----------
	#define EXT_I2C					I2C1

	#define EXT_I2C_SCL_Pin			GPIO_PIN_8
	#define EXT_I2C_SCL_GPIO		GPIOB
	#define EXT_I2C_SDA_Pin			GPIO_PIN_9
	#define EXT_I2C_SDA_GPIO		GPIOB

	#define EXT_I2C_CLK_ENABLE()	__HAL_RCC_I2C1_CLK_ENABLE()
	#define EXT_I2C_CLK_DISABLE()	__HAL_RCC_I2C1_CLK_DISABLE()
  #endif
  #ifdef __MKSDLP_V20__
	// --------- FSMC (TFT) ----------
	#define FSMC_Pins1				GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15
	#define FSMC_GPIO1				GPIOD
	#define FSMC_Pins2				GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15
	#define FSMC_GPIO2				GPIOE
	#define FSMC_Pins3				0
	#define FSMC_GPIO3				0

	// --------- TFT ----------
	#define TFT_LIGHT_Pin			GPIO_PIN_12
	#define TFT_LIGHT_GPIO			GPIOB

	#define TFT_RESET_Pin			GPIO_PIN_13
	#define TFT_RESET_GPIO			GPIOD

	// --------- SPI2 (TOUCH) ----------
	#define TOUCH_SPI					SPI2
	#define TOUCH_SPI_MISO_Pin			GPIO_PIN_14
	#define TOUCH_SPI_MISO_GPIO			GPIOB
	#define TOUCH_SPI_MOSI_Pin			GPIO_PIN_15
	#define TOUCH_SPI_MOSI_GPIO			GPIOB
	#define TOUCH_SPI_SCK_Pin			GPIO_PIN_10
	#define TOUCH_SPI_SCK_GPIO			GPIOB
	#define TOUCH_SPI_GPIO_ALTERNATE	GPIO_AF5_SPI2

	#define TOUCH_CS_Pin				GPIO_PIN_11
	#define TOUCH_CS_GPIO				GPIOD

	#define TOUCH_SPI_DMA_STREAM1		DMA1_Stream3
	#define TOUCH_SPI_DMA_CHANNEL1		DMA_CHANNEL_0
	#define TOUCH_SPI_DMA_STREAM1_IRQ	DMA1_Stream3_IRQn
	#define TOUCH_SPI_DMA_STREAM2		DMA1_Stream4
	#define TOUCH_SPI_DMA_CHANNEL2		DMA_CHANNEL_0
	#define TOUCH_SPI_DMA_STREAM2_IRQ	DMA1_Stream4_IRQn

	#define TOUCH_SPI_CLK_ENABLE()			__HAL_RCC_SPI2_CLK_ENABLE()
	#define TOUCH_SPI_CLK_DISABLE()			__HAL_RCC_SPI2_CLK_DISABLE()
	#define TOUCH_SPI_DMA_CLK_ENABLE()		__HAL_RCC_DMA1_CLK_ENABLE()
	#define TOUCH_SPI_DMA_CLK_DISABLE()		__HAL_RCC_DMA1_CLK_DISABLE()

	// --------- ENDSTOPS ----------
	#define ZE_MIN_Pin				GPIO_PIN_0
	#define ZE_MIN_GPIO				GPIOC
	#define ZE_MIN_EXTI_IRQn		EXTI0_IRQn
		
	#define ZE_MAX_Pin				GPIO_PIN_2
	#define ZE_MAX_GPIO				GPIOC
	#define ZE_MAX_EXTI_IRQn		EXTI2_IRQn

	// --------- STEPPER ----------
	#define Z_DIR_Pin				GPIO_PIN_1
	#define Z_DIR_GPIO				GPIOC

	#define Z_STEP_Pin				GPIO_PIN_1
	#define Z_STEP_GPIO				GPIOA

	#define Z_ENA_Pin				GPIO_PIN_2
	#define Z_ENA_GPIO				GPIOA

	#define Z_REF_Pin				GPIO_PIN_0
	#define Z_REF_GPIO				GPIOA
	#define Z_REF_ALTERNATE			GPIO_AF2_TIM5

	#define Z_REF_TIMER				TIM5
	#define Z_REF_TIMER_IRQ			TIM5_IRQn

	#define Z_REF_TIMER_CLK_ENABLE()		__HAL_RCC_TIM5_CLK_ENABLE()
	#define Z_REF_TIMER_CLK_DISABLE()		__HAL_RCC_TIM5_CLK_DISABLE()


	// --------- SPI1 (FLASH/CPLD/SSD2828) ----------
	#define FST_SPI					SPI1
	#define FST_SPI_MISO_Pin		GPIO_PIN_4
	#define FST_SPI_MISO_GPIO		GPIOB
	#define FST_SPI_MOSI_Pin		GPIO_PIN_5
	#define FST_SPI_MOSI_GPIO		GPIOB
	#define FST_SPI_SCK_Pin			GPIO_PIN_3
	#define FST_SPI_SCK_GPIO		GPIOB
	#define FST_SPI_GPIO_ALTERNATE	GPIO_AF5_SPI1

	#define FST_SPI_DMA_STREAM1		DMA2_Stream2
	#define FST_SPI_DMA_CHANNEL1	DMA_CHANNEL_3
	#define FST_SPI_DMA_STREAM1_IRQ	DMA2_Stream2_IRQn
	#define FST_SPI_DMA_STREAM2		DMA2_Stream5
	#define FST_SPI_DMA_CHANNEL2	DMA_CHANNEL_3
	#define FST_SPI_DMA_STREAM2_IRQ	DMA2_Stream5_IRQn

	#define FST_SPI_CLK_ENABLE()			__HAL_RCC_SPI1_CLK_ENABLE()
	#define FST_SPI_CLK_DISABLE()			__HAL_RCC_SPI1_CLK_DISABLE()
	#define FST_SPI_DMA_CLK_ENABLE()		__HAL_RCC_DMA2_CLK_ENABLE()
	#define FST_SPI_DMA_CLK_DISABLE()		__HAL_RCC_DMA2_CLK_DISABLE()

	#define FST_SPI_CLOCKSPEED		SPI_BAUDRATEPRESCALER_4

	// --------- CPLD/SSD2828 ----------
	#define FST_SPI_CS_CPLD_Pin			GPIO_PIN_7
	#define FST_SPI_CS_CPLD_GPIO		GPIOA

	#define FST_SPI_CS_SSDA_Pin			GPIO_PIN_3
	#define FST_SPI_CS_SSDA_GPIO		GPIOC

	#define FST_SPI_CS_SSDB_Pin			GPIO_PIN_3
	#define FST_SPI_CS_SSDB_GPIO		GPIOA

	#define FST_SPI_SDC_Pin				GPIO_PIN_4
	#define FST_SPI_SDC_GPIO			GPIOC

	#define MIPI_CE_Pin					GPIO_PIN_4
	#define MIPI_CE_GPIO				GPIOA

	#define MIPI_EN_PWR_Pin				GPIO_PIN_5
	#define MIPI_EN_PWR_GPIO			GPIOA

	#define CPLD_IO1_Pin				GPIO_PIN_5
	#define CPLD_IO1_GPIO	 			GPIOC

	#define CPLD_CRC_Pin				GPIO_PIN_0
	#define CPLD_CRC_GPIO				GPIOB

	#define CPLD_RST_Pin				GPIO_PIN_1
	#define CPLD_RST_GPIO				GPIOB

	// --------- FLASH ----------
	#define FST_SPI_CS_FLASH_Pin		GPIO_PIN_15
	#define FST_SPI_CS_FLASH_GPIO		GPIOA

	// --------- USB FS ----------
	#define USBH_VBUS_Pin			GPIO_PIN_3
	#define USBH_VBUS_GPIO			GPIOD

	#define USBH_DATA_N_Pin			GPIO_PIN_11
	#define USBH_DATA_N_GPIO		GPIOA
	#define USBH_DATA_P_Pin			GPIO_PIN_12
	#define USBH_DATA_P_GPIO		GPIOA
		
	// --------- LED/FANS ----------
	#define LED_UV_Pin				GPIO_PIN_6
	#define LED_UV_GPIO				GPIOA

	#define FAN_LED_Pin				GPIO_PIN_4
	#define FAN_LED_GPIO			GPIOE

	#define FAN_MB_Pin				GPIO_PIN_3
	#define FAN_MB_GPIO				GPIOE

	#define FAN_MB_On()				FAN_MB_GPIO->BSRR = FAN_MB_Pin						// Set pin
	#define FAN_MB_Off()			FAN_MB_GPIO->BSRR = (uint32_t)FAN_MB_Pin << 16U	// Reset pin

	// --------- BUZZER ----------
	#define BUZZER_Pin				GPIO_PIN_13
	#define BUZZER_GPIO				GPIOB

	// --------- I2C EEPROM ----------
	#define EEPROM_I2C				I2C1

	#define EEPROM_SCL_Pin			GPIO_PIN_6
	#define EEPROM_SCL_GPIO			GPIOB
	#define EEPROM_SDA_Pin			GPIO_PIN_7
	#define EEPROM_SDA_GPIO			GPIOB

	#define EEPROM_CLK_ENABLE()		__HAL_RCC_I2C1_CLK_ENABLE()
	#define EEPROM_CLK_DISABLE()	__HAL_RCC_I2C1_CLK_DISABLE()

	// --------- I2C DS3231 ----------
	#define DS3231_I2C				I2C1

	#define DS3231_I2C_SCL_Pin		GPIO_PIN_8
	#define DS3231_I2C_SCL_GPIO		GPIOB
	#define DS3231_I2C_SDA_Pin		GPIO_PIN_9
	#define DS3231_I2C_SDA_GPIO		GPIOB

	#define DS3231_CLK_ENABLE()		__HAL_RCC_I2C1_CLK_ENABLE()
	#define DS3231_CLK_DISABLE()	__HAL_RCC_I2C1_CLK_DISABLE()

	// --------- I2C EXTENSION PORT ----------
	#define EXT_I2C					I2C1

	#define EXT_I2C_SCL_Pin			GPIO_PIN_8
	#define EXT_I2C_SCL_GPIO		GPIOB
	#define EXT_I2C_SDA_Pin			GPIO_PIN_9
	#define EXT_I2C_SDA_GPIO		GPIOB

	#define EXT_I2C_CLK_ENABLE()	__HAL_RCC_I2C1_CLK_ENABLE()
	#define EXT_I2C_CLK_DISABLE()	__HAL_RCC_I2C1_CLK_DISABLE()
  #endif
#endif
#ifdef __CHITU_BOARD__
  #ifdef __CHITU_LV3__
	// --------- FSMC (TFT) ----------
	#define FSMC_Pins1				GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15
	#define FSMC_GPIO1				GPIOD
	#define FSMC_Pins2				GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15
	#define FSMC_GPIO2				GPIOE
	#define FSMC_Pins3				GPIO_PIN_2 | GPIO_PIN_12
	#define FSMC_GPIO3				GPIOG

	// --------- TFT ----------
	#define TFT_LIGHT_Pin			GPIO_PIN_8
	#define TFT_LIGHT_GPIO			GPIOG

	#define TFT_RESET_Pin			GPIO_PIN_12
	#define TFT_RESET_GPIO			GPIOB

	// --------- SPI (TOUCH) ----------
	#define TOUCH_SPI					SPI2
	#define TOUCH_SPI_MISO_Pin			GPIO_PIN_14
	#define TOUCH_SPI_MISO_GPIO			GPIOB
	#define TOUCH_SPI_MOSI_Pin			GPIO_PIN_15
	#define TOUCH_SPI_MOSI_GPIO			GPIOB
	#define TOUCH_SPI_SCK_Pin			GPIO_PIN_13
	#define TOUCH_SPI_SCK_GPIO			GPIOB
	#define TOUCH_SPI_GPIO_ALTERNATE	GPIO_AF5_SPI2

	#define TOUCH_CS_Pin				GPIO_PIN_11
	#define TOUCH_CS_GPIO				GPIOD

	#define TOUCH_SPI_DMA_STREAM1		DMA1_Stream3
	#define TOUCH_SPI_DMA_CHANNEL1		DMA_CHANNEL_0
	#define TOUCH_SPI_DMA_STREAM1_IRQ	DMA1_Stream3_IRQn
	#define TOUCH_SPI_DMA_STREAM2		DMA1_Stream4
	#define TOUCH_SPI_DMA_CHANNEL2		DMA_CHANNEL_0
	#define TOUCH_SPI_DMA_STREAM2_IRQ	DMA1_Stream4_IRQn

	#define TOUCH_SPI_CLK_ENABLE()			__HAL_RCC_SPI2_CLK_ENABLE()
	#define TOUCH_SPI_CLK_DISABLE()			__HAL_RCC_SPI2_CLK_DISABLE()
	#define TOUCH_SPI_DMA_CLK_ENABLE()		__HAL_RCC_DMA1_CLK_ENABLE()
	#define TOUCH_SPI_DMA_CLK_DISABLE()		__HAL_RCC_DMA1_CLK_DISABLE()

	// --------- ENDSTOPS ----------
	#define ZE_MIN_Pin				GPIO_PIN_13
	#define ZE_MIN_GPIO				GPIOC
	#define ZE_MIN_EXTI_IRQn		EXTI15_10_IRQn
		
	#define ZE_MAX_Pin				GPIO_PIN_11
	#define ZE_MAX_GPIO				GPIOF
	#define ZE_MAX_EXTI_IRQn		EXTI15_10_IRQn

	// --------- STEPPER ----------
	#define Z_DIR_Pin				GPIO_PIN_15
	#define Z_DIR_GPIO				GPIOF

	#define Z_STEP_Pin				GPIO_PIN_6
	#define Z_STEP_GPIO				GPIOA

	#define Z_ENA_Pin				GPIO_PIN_5
	#define Z_ENA_GPIO				GPIOA

	// --------- SPI (FLASH) ----------
	#define FST_SPI					SPI1
	#define FST_SPI_MISO_Pin		GPIO_PIN_4
	#define FST_SPI_MISO_GPIO		GPIOB
	#define FST_SPI_MOSI_Pin		GPIO_PIN_5
	#define FST_SPI_MOSI_GPIO		GPIOB
	#define FST_SPI_SCK_Pin			GPIO_PIN_3
	#define FST_SPI_SCK_GPIO		GPIOB
	#define FST_SPI_GPIO_ALTERNATE	GPIO_AF5_SPI1

	#define FST_SPI_DMA_STREAM1		DMA2_Stream2
	#define FST_SPI_DMA_CHANNEL1	DMA_CHANNEL_3
	#define FST_SPI_DMA_STREAM1_IRQ	DMA2_Stream2_IRQn
	#define FST_SPI_DMA_STREAM2		DMA2_Stream5
	#define FST_SPI_DMA_CHANNEL2	DMA_CHANNEL_3
	#define FST_SPI_DMA_STREAM2_IRQ	DMA2_Stream5_IRQn

	#define FST_SPI_CLK_ENABLE()			__HAL_RCC_SPI1_CLK_ENABLE()
	#define FST_SPI_CLK_DISABLE()			__HAL_RCC_SPI1_CLK_DISABLE()
	#define FST_SPI_DMA_CLK_ENABLE()		__HAL_RCC_DMA2_CLK_ENABLE()
	#define FST_SPI_DMA_CLK_DISABLE()		__HAL_RCC_DMA2_CLK_DISABLE()

	#define FST_SPI_CLOCKSPEED		SPI_BAUDRATEPRESCALER_4

	// --------- CPLD ----------
	#define CPLD_CS_Pin				GPIO_PIN_15
	#define CPLD_CS_GPIO			GPIOA

	#define CPLD_CLK_Pin			GPIO_PIN_0
	#define CPLD_CLK_GPIO			GPIOA

	#define CPLD_MOSI_Pin			GPIO_PIN_4
	#define CPLD_MOSI_GPIO			GPIOA

	#define CPLD_MISO_Pin			GPIO_PIN_3
	#define CPLD_MISO_GPIO			GPIOA

/*
	#define FST_SPI_CS_SSDB_Pin			GPIO_PIN_3
	#define FST_SPI_CS_SSDB_GPIO	GPIOA

	#define FST_SPI_SDC_Pin				GPIO_PIN_4
	#define FST_SPI_SDC_GPIO		GPIOC

	#define MIPI_CE_Pin					GPIO_PIN_4
	#define MIPI_CE_GPIO			GPIOA

	#define MIPI_EN_PWR_Pin				GPIO_PIN_5
	#define MIPI_EN_PWR_GPIO		GPIOA

	#define CPLD_IO1_Pin				GPIO_PIN_5
	#define CPLD_IO1_GPIO 			GPIOC

	#define CPLD_CRC_Pin				GPIO_PIN_0
	#define CPLD_CRC_GPIO			GPIOB

	#define CPLD_RST_Pin				GPIO_PIN_1
	#define CPLD_RST_GPIO			GPIOB
*/
	// --------- FLASH ----------
	#define FST_SPI_CS_FLASH_Pin		GPIO_PIN_15
	#define FST_SPI_CS_FLASH_GPIO		GPIOG

	// --------- USB FS ----------
	#define USBH_VBUS_Pin			GPIO_PIN_6
	#define USBH_VBUS_GPIO			GPIOD

	#define USBH_DATA_N_Pin			GPIO_PIN_11
	#define USBH_DATA_N_GPIO		GPIOA
	#define USBH_DATA_P_Pin			GPIO_PIN_12
	#define USBH_DATA_P_GPIO		GPIOA

	// --------- LED/FANS ----------
	#define LED_UV_Pin				GPIO_PIN_6
	#define LED_UV_GPIO				GPIOB

	#define FAN_LED_Pin				GPIO_PIN_0
	#define FAN_LED_GPIO			GPIOG

	#define FAN_MB_Pin				GPIO_PIN_7
	#define FAN_MB_GPIO				GPIOD

	#define FAN_MB_On()				FAN_MB_GPIO->BSRR = FAN_MB_Pin						// Set pin
	#define FAN_MB_Off()			FAN_MB_GPIO->BSRR = (uint32_t)FAN_MB_Pin << 16U	// Reset pin

	// --------- BUZZER ----------
	#define BUZZER_Pin				GPIO_PIN_8
	#define BUZZER_GPIO				GPIOA

	// --------- I2C EEPROM ----------
	#define EEPROM_I2C				I2C1

	#define EEPROM_SCL_Pin			GPIO_PIN_8
	#define EEPROM_SCL_GPIO			GPIOB
	#define EEPROM_SDA_Pin			GPIO_PIN_9
	#define EEPROM_SDA_GPIO			GPIOB
	#define EEPROM_WP_Pin			GPIO_PIN_7
	#define EEPROM_WP_GPIO			GPIOB

	#define EEPROM_CLK_ENABLE()		__HAL_RCC_I2C1_CLK_ENABLE()
	#define EEPROM_CLK_DISABLE()	__HAL_RCC_I2C1_CLK_DISABLE()

/*
	// --------- I2C DS3231 ----------
	#define DS3231_I2C				I2C1

	#define DS3231_I2C_SCL_Pin		GPIO_PIN_8
	#define DS3231_I2C_SCL_GPIO		GPIOB
	#define DS3231_I2C_SDA_Pin		GPIO_PIN_9
	#define DS3231_I2C_SDA_GPIO		GPIOB

	#define DS3231_CLK_ENABLE()		__HAL_RCC_I2C1_CLK_ENABLE()
	#define DS3231_CLK_DISABLE()	__HAL_RCC_I2C1_CLK_DISABLE()

	// --------- I2C EXTENSION PORT ----------
	#define EXT_I2C					I2C1

	#define EXT_I2C_SCL_Pin			GPIO_PIN_8
	#define EXT_I2C_SCL_GPIO		GPIOB
	#define EXT_I2C_SDA_Pin			GPIO_PIN_9
	#define EXT_I2C_SDA_GPIO		GPIOB

	#define EXT_I2C_CLK_ENABLE()	__HAL_RCC_I2C1_CLK_ENABLE()
	#define EXT_I2C_CLK_DISABLE()	__HAL_RCC_I2C1_CLK_DISABLE()
*/
  #endif
#endif






void GPIO_Init(void);


#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */

