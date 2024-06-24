# default target so that rules can appear before all
.PHONY: default all tflite
default: all
######################################
# target
######################################
target := stm32h745-ai_CM4


######################################
# building variables
######################################
# debug build?
debug ?= 1

#######################################
# logging
#######################################
verbose ?= 0
ifeq ($(verbose), 1)
  q :=
  log := @:
else
  q := @
  log := @echo
endif

#######################################
# paths
#######################################
# Build path
# todo rename to lower case
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
# src/h745 directory
root_dir := $(realpath $(dir $(mkfile_path))../..)
build_dir := $(dir $(mkfile_path))build

######################################
# source
######################################
# todo: currently everything is used from the driver directory, but it is shared between the two cores, so only the neccessary files should be selected
c_src_dirs := \
$(root_dir)/Drivers/STM32H7xx_HAL_Driver \
$(root_dir)/CM4/Core/Src \
$(root_dir)/Common/Src
c_sources := $(shell (cd $(root_dir) && find $(c_src_dirs) -name *.c))

c_sources += \
$(root_dir)/Middlewares/Third_Party/FreeRTOS/Source/croutine.c \
$(root_dir)/Middlewares/Third_Party/FreeRTOS/Source/event_groups.c \
$(root_dir)/Middlewares/Third_Party/FreeRTOS/Source/list.c \
$(root_dir)/Middlewares/Third_Party/FreeRTOS/Source/queue.c \
$(root_dir)/Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c \
$(root_dir)/Middlewares/Third_Party/FreeRTOS/Source/tasks.c \
$(root_dir)/Middlewares/Third_Party/FreeRTOS/Source/timers.c \
$(root_dir)/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c \
# Excluded according to https://nadler.com/embedded/newlibAndFreeRTOS.html
# ../../Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c

cxx_src_dirs := $(root_dir)/CM4/Core/Src
cxx_sources := $(shell (cd $(root_dir) && find $(cxx_src_dirs) -name *.cc))

asm_sources :=  \
$(root_dir)/Makefile/CM4/startup_stm32h745xx_CM4.s

asmm_sources := 

include ../binaries.mk 

#######################################
# CFLAGS
#######################################
#todo lower case
cpu := -mcpu=cortex-m4
fpu := -mfpu=fpv4-sp-d16
float-abi := -mfloat-abi=hard
mcu := $(cpu) -mthumb $(fpu) $(float-abi)

include ../flags.mk

c_defs :=  \
-DCORE_CM4 \
-DUSE_HAL_DRIVER \
-DSTM32H745xx

cxx_defs := $(c_defs)

as_includes :=

c_includes := \
-I$(root_dir)/CM4/Core/Inc \
-I$(root_dir)/Drivers/STM32H7xx_HAL_Driver/Inc \
-I$(root_dir)/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy \
-I$(root_dir)/Drivers/CMSIS/Device/ST/STM32H7xx/Include \
-I$(root_dir)/Drivers/CMSIS/Include \
-I$(root_dir)/Middlewares/Third_Party/FreeRTOS/Source/include \
-I$(root_dir)/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F

cxx_includes := $(c_includes) \
-I$(root_dir)/CM4/Core/Src/hello_world

# linker options
# link script
ldscript := stm32h745xx_flash_CM4.ld
# libraries
LDLIBS += # note: removed -lm
LDFLAGS += -T$(ldscript)

#######################################
# building the binary
#######################################
# default action: build all
all: $(build_dir)/$(target).elf $(build_dir)/$(target).hex $(build_dir)/$(target).bin $(build_dir)/$(target).list

# all the flags required in build_rules
CFLAGS := $(CFLAGS) $(mcu) $(c_defs) $(c_includes)
CXXFLAGS := $(CXXFLAGS) $(mcu) $(cxx_defs) $(cxx_includes)
ASFLAGS := $(ASFLAGS) $(CFLAGS)
LDFLAGS := $(LDFLAGS)
LDLIBS := $(LDLIBS)
include ../build_rules.mk
#######################################
# clean up
#######################################
clean:
	-rm -fR $(build_dir)
  
#######################################
# dependencies
#######################################
-include $(objects:.o=.d)

# *** EOF ***
