/*
******************************************************************************
**

**  File        : LinkerScript.ld
**
**
**  Abstract    : Linker script for STM32H7 series
**                1024Kbytes FLASH and 192Kbytes RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : STMicroelectronics STM32
**
**  Distribution: The file is distributed �as is,� without any warranty
**                of any kind.
**
*****************************************************************************
** @attention
**
** Copyright (c) 2019 STMicroelectronics.
** All rights reserved.
**
** This software is licensed under terms that can be found in the LICENSE file
** in the root directory of this software component.
** If no LICENSE file comes with this software, it is provided AS-IS.
**
******************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = 0x24080000;    /* end of RAM */
/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x4000;      /* required amount of heap  */
_Min_Stack_Size = 0x800; /* required amount of stack */
_mpu_shared_size = 4K;
_mpu_non_cacheable_size = 16K;

/* Specify the memory areas */
MEMORY
{
FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 1024K
RAM (xrw)   : ORIGIN = 0x24000000, LENGTH =  512K /* Actually RAM_D1, renamed to make default */
DTCMRAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 128K
RAM_D2 (xrw)   : ORIGIN = 0x30000000, LENGTH = 288K
RAM_D3 (xrw)   : ORIGIN = 0x38000000, LENGTH = 64K
ITCMRAM (xrw)  : ORIGIN = 0x00000000, LENGTH = 64K
}

/* Define output sections */
SECTIONS
{
  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >FLASH

  /* The program code and other data goes into FLASH */
  .text :
  {
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;        /* define a global symbols at end of code */
  } >FLASH

  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } >FLASH

  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
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

  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data :
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >RAM AT> FLASH


  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM

  /* CUSTOM SECTIONS =================================================== */
  .speedy_mem.bss (NOLOAD) :
  {
    . = ALIGN(4);
    _start_speedy_mem_bss = .;
    *(.speedy_mem.bss)
    . = ALIGN(4);
    _end_speedy_mem_bss = .;
  } >RAM

  .speedy_mem.data :
  {
    . = ALIGN(4);
    _start_speedy_mem_data = .;
    *(.speedy_mem.data)
    . = ALIGN(4);
    _end_speedy_mem_data = .;
  } >RAM AT> FLASH

  _load_start_speedy_mem_data = LOADADDR(.speedy_mem.data);

  /* This section is shared between the cores, the content should match and
   * the address is as well. The values are initialized by the M7 core. H
   */
  .shared.bss (NOLOAD) :
  {
    . = ALIGN(_mpu_shared_size);
    _start_shared = .;
    _start_shared_bss = .;
    *(.shared.bss)
    . = ALIGN(4);
    _end_shared_bss = .;
  } >RAM_D3

  .shared.data :
  {
    . = ALIGN(4);
    _start_shared_data = .;
    *(.shared.data)
    . = ALIGN(4);
    _end_shared_data = .;
    _end_shared = .;
  } >RAM_D3 AT> FLASH

  _load_start_shared_data = LOADADDR(.shared.data);
  ASSERT(_end_shared - _start_shared < _mpu_shared_size, "error: Shared section longer than configured MPU size")
  . = _start_shared + _mpu_shared_size;

  .non_cacheable.bss (NOLOAD) :
  {
    . = ALIGN(_mpu_non_cacheable_size);
    _start_non_cacheable = .;
    _start_non_cacheable_bss = .;
    *(.non_cacheable.bss)
    . = ALIGN(4);
    _end_non_cacheable_bss = .;
  } >RAM

  .non_cacheable.data :
  {
    . = ALIGN(4);
    _start_non_cacheable_data = .;
    *(.non_cacheable.data)
    . = ALIGN(4);
    _end_non_cacheable_data = .;
    _end_non_cacheable = .;
  } >RAM AT> FLASH

  _load_start_non_cacheable_data = LOADADDR(.non_cacheable.data);
  ASSERT(_end_non_cacheable - _start_non_cacheable < _mpu_non_cacheable_size, "error: Non-cacheable section longer than configured MPU size")
  . = _start_non_cacheable + _mpu_non_cacheable_size;

  /* END OF CUSTOM SECTIONS ============================================ */
  /* IMPORTANT: if the size of this section is not configured right, then it will grow into the next section, so it is advisable to locate this to the end of the RAM (last of the sections) */
  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
