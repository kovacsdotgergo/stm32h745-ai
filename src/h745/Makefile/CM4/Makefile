# default target so that rules can appear before all
.PHONY: default all tflite
default: all
######################################
# target
######################################
target := stm32h745-ai_CM4

######################################
# build variables
######################################
# debug build?
debug ?= 1
# todo: rename tflite to tflm everywhere
# alternatively X-CUBE-AI
nn_framework ?= tflm

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
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
# src/h745 directory
root_dir := $(realpath $(dir $(mkfile_path))../..)
base_build_dir := $(dir $(mkfile_path))build
ifeq ($(debug), 1)
  build_dir := $(base_build_dir)/debug
else
  build_dir := $(base_build_dir)/release
endif
build_dir := $(build_dir)/$(nn_framework)

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

cxx_includes := $(c_includes)

# linker options
# linker script
ldscript := stm32h745xx_flash_CM4.ld
# libraries
LDLIBS += # note: removed -lm
LDFLAGS += -T$(ldscript)

ifeq ($(nn_framework),X-CUBE-AI)

#######################################
# X-CUBE-AI
#######################################
c_includes += \
-I$(root_dir)/Middlewares/ST/AI/Inc \
-I$(root_dir)/CM4/nn_framework \
-I$(root_dir)/CM4/nn_framework/X-CUBE-AI
# -I../../CM4/X-CUBE-AI only the empty constants header
c_sources += \
$(shell (cd $(root_dir) && find $(root_dir)/CM4/nn_framework/X-CUBE-AI -name *.c))

LDLIBS += -l:NetworkRuntime810_CM4_GCC.a
LDFLAGS += -L$(root_dir)/Middlewares/ST/AI/Lib

else ifeq ($(nn_framework),tflm)
#######################################
# tflite
#######################################
include ../tflite.mk

LDLIBS += -l:$(tflite_libname)
LDFLAGS += -L$(dir $(tflite_lib))

cxx_defs += $(tflite_defs)
cxx_includes += $(tflite_includes)
c_includes += $(tflite_includes)

# tflm subfolder for app code
c_includes += \
-I$(root_dir)/CM4/nn_framework/tflm \
-I$(root_dir)/CM4/nn_framework
cxx_includes += \
-I$(root_dir)/CM4/nn_framework/tflm \
-I$(root_dir)/CM4/nn_framework
cxx_sources += $(shell (cd $(root_dir) && find $(root_dir)/CM4/nn_framework/tflm -name *.cc))

# todo: temporarily don't depend on Makefile to avoid long rebuilds
$(build_dir)/$(tflite_rel_src_dir)/%.o: $(root_dir)/$(tflite_rel_src_dir)/%.c
	@mkdir -p $(dir $@)
	$(log) compiling $(patsubst $(root_dir)/%,%,$@)
	$(q)$(CC) -c $(mcu) $(c_defs) $(c_includes) $(CFLAGS) -Wa,-a,-ad,-alms=$(@:.o=.lst) $< -o $@

$(build_dir)/$(tflite_rel_src_dir)/%.o: $(root_dir)/$(tflite_rel_src_dir)/%.cc
	@mkdir -p $(dir $@)
	$(log) compiling $(patsubst $(root_dir)/%,%,$@)
	$(q)$(CXX) -c $(mcu) $(cxx_defs) $(cxx_includes) $(CXXFLAGS) -Wa,-a,-ad,-alms=$(@:.o=.lst) $< -o $@

else
$(error Wrong value of nn_framework: $(nn_framework))
endif
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
	-rm -fR $(base_build_dir)
  
#######################################
# dependencies
#######################################
-include $(objects:.o=.d)

# *** EOF ***
