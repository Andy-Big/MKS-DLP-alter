
stm32_chip_name = 'STM32F407VET6'
include 'main'

stm32_solution('MKSDLP_Alter')



stm32_project('MKSDLP_Alter',{
		STM32_MIN_STACK_SIZE = '0x5000',
		STM32_FLASH_ORIGIN = '0x08010000',
		STM32_FLASH_SIZE = '(0x0807FFFF-0x08010000)',
	})

	defines {
		--'__DEBUG1__',
        'USE_HAL_DRIVER',
        'USE_HAL_SPI_REGISTER_CALLBACKS=1U',
        'MKS_DLP_BOARD',
	}
	includedirs{ 
		
		'src/App',
        'src/App/tft',
        'src/App/tgui',
        'src/App/stepper',
        'src/App/uvdisplay',
        'src/App/prtfiles',
        'src/FATFS/Target',
        'src/FATFS/App',
        'src/USB_HOST/App',
        'src/USB_HOST/Target',
        'src/Drivers/STM32F4xx_HAL_Driver/Inc',
        'src/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy',
        'src/Middlewares/Third_Party/FatFs/src',
        'src/Middlewares/ST/STM32_USB_Host_Library/Core/Inc',
        'src/Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc',
        'src/Drivers/CMSIS/Device/ST/STM32F4xx/Include',
        'src/Drivers/CMSIS/Include',
        'src/System',
	}
	
	files {
		
		'src/App/*.cpp',
		'src/App/*.h',
		'src/App/fst_spi.c',
		'src/App/spi_flash.c',
		'src/App/sys_timer.c',
		'src/App/tim.c',
		'src/App/gpio.c',
		'src/App/rtc.c',
		'src/App/fsmc.c',

		'src/App/stepper/*.h',
		'src/App/stepper/*.cpp',

		'src/App/tgui/*.h',
		'src/App/tgui/*.cpp',

		'src/App/tft/*.h',
		'src/App/tft/*.cpp',
		'src/App/tft/lcd_low.c',
		'src/App/tft/touch.c',
		'src/App/tft/fonts/*.c',

		'src/App/uvdisplay/*.h',
		'src/App/uvdisplay/*.cpp',

		'src/App/prtfiles/*.h',
		'src/App/prtfiles/*.cpp',

		'src/FATFS/App/*.h',
		'src/FATFS/App/*.c',
		'src/FATFS/Target/*.h',
		'src/FATFS/Target/*.c',

		'src/USB_HOST/App/*.h',
		'src/USB_HOST/App/*.c',
		'src/USB_HOST/Target/*.h',
		'src/USB_HOST/Target/*.c',

		'src/System/*.c',
		'src/System/gcc/*.s',
		'src/System/gcc/*.c',
		'src/System/*.h',

		-- ff
		'src/Middlewares/Third_Party/FatFs/src/*.h',
		'src/Middlewares/Third_Party/FatFs/src/*.c',
		-- usb
		'src/Middlewares/ST/STM32_USB_Host_Library/Core/Inc/*.h',
		'src/Middlewares/ST/STM32_USB_Host_Library/Core/Src/*.c',
		'src/Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc/*.h',
		'src/Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/*.c',
		
		-- cmsis
		path.join('src/Drivers/CMSIS/Include/*.h'),
		path.join('src/Drivers/CMSIS/STM32' .. stm32_chip.f .. '/Include/*.h'),
		path.join('src/Drivers/CMSIS/STM32' .. stm32_chip.f .. '/src/*.c'),
		-- hal
		path.join('src/Drivers/STM32' .. stm32_chip.f .. '_HAL_Driver/Inc/*.h'),
		path.join('src/Drivers/STM32' .. stm32_chip.f .. '_HAL_Driver/Src/*.c'),
		
	}

	removefiles {
		'src/Middlewares/ST/STM32_USB_Host_Library/Core/Src/*_template.c',
		path.join('src/Drivers/STM32' .. stm32_chip.f .. '_HAL_Driver/Src/*_template.c'),
	}

	links {
		'm'
	}

