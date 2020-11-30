
local M = {}

local flash_size = {
	['4'] = 16,
	['6'] = 32,
	['8'] = 64,
	['B'] = 128,
	['C'] = 256,
	['D'] = 384,
	['E'] = 512,
	['F'] = 768,
	['G'] = 1024,
}

local chip_type = {
	['100'] = {
		['4'] = {4,'B'},
		['6'] = {4,'B'},
		['8'] = {8,'B'},
		['B'] = {8,'B'},
		['C'] = {24,'E'},
		['D'] = {32,'E'},
		['E'] = {32,'E'},
	},
	['101'] = {
		['4'] = {4,'6'},
		['6'] = {6,'6'},
		['8'] = {10,'B'},
		['B'] = {16,'B'},
		['C'] = {32,'E'},
		['D'] = {48,'E'},
		['E'] = {48,'E'},
		['F'] = {80,'G'},
		['G'] = {80,'G'},
 	},
 	['102'] = {
 		['4'] = {4,'6'},
 		['6'] = {6,'6'},
 		['8'] = {10,'B'},
 		['B'] = {16,'B'},
 	},
 	['103'] = {
 		['4'] = {6, '6'},
 		['6'] = {10,'6'},
 		['8'] = {20,'B'},
 		['B'] = {20,'B'},
 		['C'] = {48,'E'},
 		['D'] = {54,'E'},
 		['E'] = {54,'E'},
 		['F'] = {96,'G'},
 		['G'] = {96,'G'},
 	},
 	['105'] = {
 		['8'] = {64,'C'},
 		['B'] = {64,'C'},
 		['C'] = {64,'C'},
 	},
 	['107'] = {
 		['B'] = {64,'C'},
 		['C'] = {64,'C'},
 	},
 	['405'] = {
 		['G'] = {128,'x'},
 	},
 	['407'] = {
 		['E'] = {128,'x'},
 		['G'] = {128,'x'},
 	}
}

local f4_flags = {
	c = {
		'-mthumb',
		'-fno-builtin',
		'-mcpu=cortex-m4', 
		'-mfpu=fpv4-sp-d16', 
		'-mfloat-abi=hard', 
		'-Wall',
		'-std=gnu99',
		'-ffunction-sections',
		'-fdata-sections',
		'-fomit-frame-pointer',
		'-mabi=aapcs',
		'-fno-unroll-loops',
		'-ffast-math',
		'-ftree-vectorize',
		'-fno-pic'
	},
	cxx = {
		'-mthumb',
		'-fno-builtin',
		'-mcpu=cortex-m4',
		'-mfpu=fpv4-sp-d16', 
		'-mfloat-abi=hard', 
		'-Wall',
		'-std=c++11',
		'-ffunction-sections',
		'-fdata-sections',
		'-fomit-frame-pointer',
		'-mabi=aapcs',
		'-fno-unroll-loops', 
		'-ffast-math', 
		'-ftree-vectorize',
		'-fno-pic',
		'-finline-small-functions',
		'-finline-functions-called-once',
		'-fdelete-null-pointer-checks',
	},
	asm = {
		'-mthumb', 
		'-mcpu=cortex-m4',
		'-mfpu=fpv4-sp-d16',
		'-mfloat-abi=hard',
		'-x assembler-with-cpp'
	},
	ld = {
		'-Wl,--gc-sections',
		'-mthumb',
		'-mcpu=cortex-m4',
		'-mabi=aapcs',
		'-mfpu=fpv4-sp-d16',
		'-mfloat-abi=hard',
		'-fno-pic',
		'-Wl,--print-memory-usage',
		'-Wl,--relax',
	},
}
local f1_flags = {
	c = {
		'-mthumb',
		'-fno-builtin',
		'-mcpu=cortex-m3',
		'-Wall',
		'-std=gnu99',
		'-ffunction-sections',
		'-fdata-sections',
		'-fomit-frame-pointer',
		'-mabi=aapcs',
		'-fno-unroll-loops',
		'-fno-pic'
	},
	cxx = {
		'-mthumb',
		'-fno-builtin',
		'-mcpu=cortex-m3',
		'-Wall',
		'-std=c++11',
		'-ffunction-sections',
		'-fdata-sections',
		'-fomit-frame-pointer',
		'-mabi=aapcs',
		'-fno-unroll-loops',
		'-fno-pic',
		'-finline-small-functions',
		'-finline-functions-called-once',
		'-fdelete-null-pointer-checks',
	},
	asm = {
		'-mthumb',
		'-mcpu=cortex-m3',
		'-x','assembler-with-cpp'
	},
	ld = {
		--'--gc-sections',
		--'-nostdlib',
		'-Wl,--gc-sections',
		'-mthumb',
		'-mcpu=cortex-m3',
		'-mabi=aapcs',
		'-fno-pic',
		'-Wl,--print-memory-usage',
		'-Wl,--relax',
	},
}
local f4_ccram = {
	ORIGIN = '0x10000000',
	SIZE = '64K',
	SECTIONS = [[
		.ccm (NOLOAD):
	    {
	        PROVIDE(_cmm_start = .);
	        . = ALIGN(4);
	        *(.ccm)
	        . = ALIGN(4);
	        *(.ccm.*)
	        . = ALIGN(4);
	        PROVIDE(_cmm_end = .);
	    } > CCRAM
	]]
}

local chip_series = {
	['100'] = {'F1xx',f1_flags},
	['101'] = {'F1xx',f1_flags},
	['103'] = {'F1xx',f1_flags},
	['105'] = {'F1xx',f1_flags},
	['107'] = {'F1xx',f1_flags},
	['405'] = {'F4xx',f4_flags,f4_ccram},
	['407'] = {'F4xx',f4_flags,f4_ccram},
}

-- SET(CMAKE_C_FLAGS "-mthumb -fno-builtin -mcpu=cortex-m3 -Wall -std=gnu99 -ffunction-sections -fdata-sections -fomit-frame-pointer -mabi=aapcs -fno-unroll-loops -ffast-math -ftree-vectorize" CACHE INTERNAL "c compiler flags")
-- SET(CMAKE_CXX_FLAGS "-mthumb -fno-builtin -mcpu=cortex-m3 -Wall -std=c++11 -ffunction-sections -fdata-sections -fomit-frame-pointer -mabi=aapcs -fno-unroll-loops -ffast-math -ftree-vectorize" CACHE INTERNAL "cxx compiler flags")
-- SET(CMAKE_ASM_FLAGS "-mthumb -mcpu=cortex-m3 -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")

-- SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections -mthumb -mcpu=cortex-m3 -mabi=aapcs" CACHE INTERNAL "executable linker flags")
-- SET(CMAKE_MODULE_LINKER_FLAGS "-mthumb -mcpu=cortex-m3 -mabi=aapcs" CACHE INTERNAL "module linker flags")
-- SET(CMAKE_SHARED_LINKER_FLAGS "-mthumb -mcpu=cortex-m3 -mabi=aapcs" CACHE INTERNAL "shared linker flags")


function M.parse( chip )
	local chip_f1,chip_f2,chip_o = string.match(chip,'^[sS][tT][mM]32[fF]([14])(0[012357]).([468BCDEFG]).+$')
	assert(chip_f1 and chip_f2,'invalid chip')
	local chip_serie = chip_f1 .. chip_f2 
	local chip_i = assert((chip_type[chip_serie] or {})[chip_o],'uncnown chip ' .. chip .. ' ' .. chip_serie .. '/' .. chip_o)

	local res = {
		flash = assert(flash_size[chip_o],'unknown flash size code "' .. chip_o .. '"'),
		ram = chip_i[1],
		defs = {'-DSTM32F'..chip_f1,'-DSTM32F' .. chip_serie .. 'x' .. chip_i[2]},
		flags = chip_series[chip_serie][2],
		f = chip_series[chip_serie][1],
		low_name = string.lower('f'..chip_serie..'x'..chip_i[2]),
	}
	local ccram = chip_series[chip_serie][3]
	res.ld_config = {
			STM32_FLASH_ORIGIN = '0x08000000',
			STM32_RAM_ORIGIN = '0x20000000',
			STM32_MIN_STACK_SIZE = '0x200',
			STM32_MIN_HEAP_SIZE = '0',
			STM32_CCRAM_ORIGIN = ccram and ccram.ORIGIN,
			STM32_CCRAM_SIZE = ccram and ccram.SIZE,
			STM32_FLASH_SIZE = tostring(res.flash) .. 'K',
			STM32_RAM_SIZE = tostring(res.ram) .. 'K',
			STM32_CCRAM_SECTION = ccram and ccram.SECTIONS,
			--ccram = ccram
		}

	return res
end

return M