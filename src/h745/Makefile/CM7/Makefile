# default rule only required because the tflite makefile has rules which appear before `all`, so those would be the default target
.PHONY: default all tflite
default: all
######################################
# target
######################################
target := stm32h745-ai_CM7

######################################
# build variables
######################################
# debug build?
debug ?= 1
#todo: rename tflite to tflm
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
$(root_dir)/CM7/Core/Src \
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

cxx_src_dirs := $(root_dir)/CM7/Core/Src
cxx_sources := $(shell (cd $(root_dir) && find $(cxx_src_dirs) -name *.cc))

asm_sources :=  \
$(root_dir)/Makefile/CM7/startup_stm32h745xx_CM7.s

asmm_sources := 

include ../binaries.mk

#######################################
# CFLAGS
#######################################
cpu := -mcpu=cortex-m7
fpu := -mfpu=fpv5-d16
float-abi := -mfloat-abi=hard
mcu := $(cpu) -mthumb $(fpu) $(float-abi)

include ../flags.mk

c_defs :=  \
-DCORE_CM7 \
-DUSE_HAL_DRIVER \
-DSTM32H745xx

cxx_defs := $(c_defs)

as_includes :=

c_includes :=  \
-I$(root_dir)/CM7/Core/Inc \
-I$(root_dir)/Drivers/STM32H7xx_HAL_Driver/Inc \
-I$(root_dir)/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy \
-I$(root_dir)/Drivers/CMSIS/Device/ST/STM32H7xx/Include \
-I$(root_dir)/Drivers/CMSIS/Include \
-I$(root_dir)/Middlewares/Third_Party/FreeRTOS/Source/include \
-I$(root_dir)/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F

cxx_includes := $(c_includes)

# linker options
# linker script
ldscript := stm32h745xx_flash_CM7.ld
# libraries
LDLIBS += # note: removed -lm
LDFLAGS += -T$(ldscript)

ifeq ($(nn_framework),X-CUBE-AI)

#######################################
# X-CUBE-AI
#######################################
c_includes += \
-I$(root_dir)/Middlewares/ST/AI/Inc \
-I$(root_dir)/CM7/nn_framework \
-I$(root_dir)/CM7/nn_framework/X-CUBE-AI
# -I../../CM7/X-CUBE-AI only the empty constants header
c_sources += \
$(shell (cd $(root_dir) && find $(root_dir)/CM7/nn_framework/X-CUBE-AI -name *.c))

LDLIBS += -l:NetworkRuntime810_CM7_GCC.a
LDFLAGS += -L$(root_dir)/Middlewares/ST/AI/Lib

else ifeq ($(nn_framework),tflm)
#######################################
# tflite
#######################################
include ../tflite.mk

# TODO: because `libs` is required for the elf to depend on, the other two lines could be generated from that
LDLIBS += -l:$(tflite_libname)
LDFLAGS += -L$(dir $(tflite_lib))
libs += $(tflite_lib)

cxx_defs += $(tflite_defs)
cxx_includes += $(tflite_includes)
c_includes += $(tflite_includes)

# tflm subfolder for app code
c_includes += \
-I$(root_dir)/CM7/nn_framework/tflm \
-I$(root_dir)/CM7/nn_framework
cxx_includes += \
-I$(root_dir)/CM7/nn_framework/tflm \
-I$(root_dir)/CM7/nn_framework
cxx_sources += $(shell (cd $(root_dir) && find $(root_dir)/CM7/nn_framework/tflm -name *.cc))

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
# CMSIS-DSP
#######################################
include ../cmsis_dsp.mk

LDLIBS += -l:$(dsp_libname)
LDFLAGS += -L$(dir $(dsp_lib))
libs += $(dsp_lib)

c_includes += $(dsp_includes) # TODO: this is mentioned in todo.md, maybe some of the includes (private ones) can be avoided

debug_target:
	$(log) dsp lib: $(LDLIBS), $(LDFLAGS)
	$(log) dsp rule: $(dsp_lib)
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
