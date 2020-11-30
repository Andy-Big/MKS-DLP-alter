
local chip_name = stm32_chip_name or _OPTIONS.chip
assert(chip_name,'use --chip')

local chip_config = require 'chip_config'
local chip = chip_config.parse(chip_name)
assert(chip,'invalid chip')

stm32_chip = chip

print('flash:',chip.flash .. 'K')
print('ram:',chip.ram..'K')



local _linker_script = require 'linker_script'

local toolchain_abs = nil
if toolchain_path then
	toolchain_abs = path.rebase(_OPTIONS.toolchain_path,_WORKING_DIR,'build')
end



function stm32_solution( name )
	solution(name)
		toolset 'arm_gcc'
		location( path.join(_WORKING_DIR,'build') )
		configurations { 'debug', 'release' }

		

		configuration('debug')
			defines{'DEBUG'}

		configuration{}
end

function stm32_project( name , prj_ld_config )
	project(name)
		kind 'firmware'
		targetdir(path.join(_WORKING_DIR,'bin'))
		targetextension( '.elf' )
	
		local gcc = premake.tools.arm_gcc 

		postbuildcommands {
			gcc.gettoolname(nil,'strip') .. " -s %{cfg.targetdir}/"..name..".elf -o  %{cfg.targetdir}/"..name..".strip.elf",
  			gcc.gettoolname(nil,'objcopy') .. " -O binary %{cfg.targetdir}/"..name..".strip.elf %{cfg.targetdir}/"..name..".bin",
  			--gcc.gettoolname(nil,'objdump') .. " -h %{cfg.targetdir}/bootloader.strip.elf",
  			gcc.gettoolname(nil,'objdump') .. " -d -S -s %{cfg.targetdir}/"..name..".elf > %{cfg.targetdir}/"..name..".asm"
		}


		--configuration(nil)

		os.mkdir(path.join(_WORKING_DIR,'build','ld'))
		local ld_config = stm32_chip.ld_config
		ld_config.STM32_LIBS = 'libc_nano.a (*)'
		if prj_ld_config then
			ld_config = setmetatable(prj_ld_config,{
				__index = ld_config
				})
		end
		
		local linker_script_file = path.join(_WORKING_DIR,'build','ld',name .. '-flash.ld')
		_linker_script.generate(linker_script_file,ld_config)
		linker_script(linker_script_file)
		
end
