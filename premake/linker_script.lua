local m = {}

function m.generate( file , config )

	local content = [[
  ENTRY(Reset_Handler)
  ]]..(
  config.ccram and 
  [[
  _estack = ${STM32_CCRAM_ORIGIN} + ${STM32_CCRAM_SIZE} - 8;
  ]] or 
  [[
  _estack = ${STM32_RAM_ORIGIN} + ${STM32_RAM_SIZE} - 8;
  ]] ).. 
  [[
  _Min_Heap_Size = ${STM32_MIN_HEAP_SIZE};
  _Min_Stack_Size = ${STM32_MIN_STACK_SIZE};
  MEMORY
  {
    FLASH (rx)     : ORIGIN = ${STM32_FLASH_ORIGIN}, LENGTH = ${STM32_FLASH_SIZE}
    RAM (xrw)      : ORIGIN = ${STM32_RAM_ORIGIN}, LENGTH = ${STM32_RAM_SIZE}
  ]]
  ..(
  config.STM32_CCRAM_ORIGIN and
  [[
    CCRAM (rw)  : ORIGIN = ${STM32_CCRAM_ORIGIN}, LENGTH = ${STM32_CCRAM_SIZE}
  ]] or ''
    )..
  [[
  }
  SECTIONS
  {
    .isr_vector :
    {
      . = ALIGN(4);
      KEEP(*(.isr_vector))
      . = ALIGN(4);
    } >FLASH
    .text :
    {
      . = ALIGN(4);
      *(.text)
      *(.text*)
      *(.glue_7)
      *(.glue_7t)
      *(.eh_frame)
      KEEP (*(.init))
      KEEP (*(.fini))
      . = ALIGN(4);
      _etext = .;
    } >FLASH
    .rodata :
    {
      . = ALIGN(4);
      *(.rodata)
      *(.rodata*)
      . = ALIGN(4);
    } >FLASH
    .ARM.extab   : 
    { 
    	*(.ARM.extab* .gnu.linkonce.armextab.*) 
    } >FLASH
    .ARM : {
      __exidx_start = .;
      *(.ARM.exidx*)
      __exidx_end = .;
    } >FLASH
    .preinit_array     :
    {
      PROVIDE_HIDDEN (__preinit_array_start = .);
      KEEP (*(.preinit_array*))
      PROVIDE_HIDDEN (__preinit_array_end = .);
    } >FLASH
    .init_array :
    {
      PROVIDE_HIDDEN (__init_array_start = .);
      KEEP (*(SORT(.init_array.*)))
      KEEP (*(.init_array*))
      PROVIDE_HIDDEN (__init_array_end = .);
    } >FLASH
    .fini_array :
    {
      PROVIDE_HIDDEN (__fini_array_start = .);
      KEEP (*(SORT(.fini_array.*)))
      KEEP (*(.fini_array*))
      PROVIDE_HIDDEN (__fini_array_end = .);
    } >FLASH
    _sidata = LOADADDR(.data);
    .data : 
    {
      . = ALIGN(4);
      _sdata = .;
      *(.data)
      *(.data*)
      . = ALIGN(4);
      _edata = .;
    } >RAM AT> FLASH
    ${STM32_CCRAM_SECTION}
    . = ALIGN(4);
    .bss :
    {
      _sbss = .;
      __bss_start__ = _sbss;
      *(.bss)
      *(.bss*)
      *(COMMON)
      . = ALIGN(4);
      _ebss = .;
      __bss_end__ = _ebss;
    } >RAM
    ._user_heap_stack :
    {
      . = ALIGN(4);
      PROVIDE ( end = . );
      PROVIDE ( _end = . );
      . = . + _Min_Heap_Size;
      . = . + _Min_Stack_Size;
      . = ALIGN(4);
    } > ]] .. (config.ccram and 'CCRAM' or 'RAM') .. [[
    /* Remove information from the standard libraries */
    /DISCARD/ :
  	{
      ${STM32_LIBS}
     	libgcc.a ( * )
  	}
    .ARM.attributes 0 : { *(.ARM.attributes) }
  }
]]

local pconent = string.gsub(content,"%${(.-)}",function(w) return assert(config[w],'undefined ' .. w) end)

local file = io.open(file,'w')
file:write(pconent)
file:close()

end

return m