
local p = premake
local api = p.api

premake.option.get('os').allowed = {{'stm32',"ST STM32 bare metal"}}
premake.fields['kind'].allowed = {
	firmware='firmware',
	staticlib='StaticLib'
}


_OPTIONS.os = 'stm32'


newoption {
	trigger = "toolchain_path",
	value = "PATH",
	description = "arm toolchain path"
}

newoption {
	trigger = "chip",
	value = "string",
	description = "stm32 chip name"
}


api.register {
	name = "linker_script",
	scope = 'project',
	kind = 'string'
}


local gcc = {}
local tool_map = {
	cxx = 'g++',
	cc = 'gcc',
}
function gcc.gettoolname( cfg, tool )
	if not toolchain_abs then
		return 'arm-none-eabi-' .. (tool_map[tool] or tool)
	end
	return path.join(toolchain_abs,'bin','arm-none-eabi-' .. (tool_map[tool] or tool))
end
function gcc.getcppflags( cfg )
	return table.join(
		{"-MMD", "-MP"},
		stm32_chip.defs
		)
end
local function opt_flags( cfg , cxx )
	local cmn = {'-fno-exceptions','-nodefaultlibs'}
	if cxx then
		table.insert(cmn,'-fno-rtti')
	end
	if cfg.name == 'debug' and not cfg.optimize then
		return table.join(cmn,{'-O0', '-g', '-ggdb'})
	else
		return table.join(cmn,{cfg.optimize and '-O3' or '-Os'})
	end
end
function gcc.getcflags( cfg )
	return table.join(stm32_chip.flags.c,opt_flags(cfg))
end
function gcc.getcxxflags( cfg )
	return table.join(stm32_chip.flags.cxx,opt_flags(cfg,true))
end
function gcc.getldflags(cfg)
	return table.join(
		stm32_chip.flags.ld,
		{
			'-nostdlib',
			'-Wl,--gc-sections',
			'-u _scanf_float',
  			'-u _printf_float',
		},
		cfg.linker_script and {'-T',assert(cfg.linker_script)} or {},
		opt_flags(cfg,true))
end
premake.tools.arm_gcc = setmetatable(gcc,{__index=premake.tools.gcc})



newaction {
		trigger         = "arm_make",
		shortname       = "GNU Make for STM32",
		description     = "Generate GNU makefiles for STM32",

		valid_kinds     = { "firmware" , "StaticLib"},

		valid_languages = { "C", "C++" },

		valid_tools     = {
			cc     = { "arm_gcc" }
		},

		onWorkspace = function(wks)
			local make = premake.make
			premake.escaper(make.esc)
			premake.generate(wks, make.getmakefilename(wks, false), make.generate_workspace)
		end,

		onProject = function(prj)
			local make = premake.make
			premake.escaper(make.esc)
			local makefile = make.getmakefilename(prj, true)
			premake.generate(prj, makefile, make.cpp.generate)
		end,

		onCleanWorkspace = function(wks)
			local make = premake.make
			premake.clean.file(wks, make.getmakefilename(wks, false))
		end,

		onCleanProject = function(prj)
			local make = premake.make
			premake.clean.file(prj, make.getmakefilename(prj, true))
		end

	}