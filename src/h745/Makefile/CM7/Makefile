# default rule only required because the tflite makefile has rules which appear before `all`, so those would be the default target
.PHONY: default all tflite
default: all
######################################
# target
######################################
TARGET := stm32h745-ai_CM7

######################################
# building variables
######################################
# debug build?
DEBUG := 1
# optimization
OPT := -O0

#######################################
#logging
#######################################
VERBOSE ?= 0
ifeq ($(VERBOSE), 1)
  q =
  log = @:
else
  q = @
  log = @echo
endif

#######################################
# paths
#######################################
# Build path
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
# src/h745 directory
root_dir := $(realpath $(dir $(mkfile_path))../..)
build_dir := $(dir $(mkfile_path))build

######################################
# source
######################################
c_src_dirs := \
$(root_dir)/Drivers/STM32H7xx_HAL_Driver \
$(root_dir)/CM7/Core/Src \
$(root_dir)/Common/Src
C_SOURCES := $(shell (cd $(root_dir) && find $(c_src_dirs) -name *.c))

C_SOURCES +=  \
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
CXX_SOURCES := $(shell (cd $(root_dir) && find $(cxx_src_dirs) -name *.cc))

ASM_SOURCES :=  \
$(root_dir)/Makefile/CM7/startup_stm32h745xx_CM7.s

ASMM_SOURCES := 

#######################################
# binaries
#######################################
prefix := arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC := $(GCC_PATH)/$(prefix)gcc
CXX := $(GCC_PATH)/$(prefix)g++
AS := $(GCC_PATH)/$(prefix)gcc -x assembler-with-cpp
CP := $(GCC_PATH)/$(prefix)objcopy
SZ := $(GCC_PATH)/$(prefix)size
DP := $(GCC_PATH)/$(prefix)objdump
AR := $(GCC_PATH)/$(prefix)ar
else
CC := $(prefix)gcc
CXX := $(prefix)g++
AS := $(prefix)gcc -x assembler-with-cpp
CP := $(prefix)objcopy
SZ := $(prefix)size
DP := $(prefix)objdump
AR := $(prefix)ar
endif
HEX := $(CP) -O ihex
BIN := $(CP) -O binary -S
 
ARFLAGS := -rcs

#######################################
# CFLAGS
#######################################
cpu := -mcpu=cortex-m7
fpu := -mfpu=fpv5-d16
float-abi := -mfloat-abi=hard
mcu := $(cpu) -mthumb $(fpu) $(float-abi)

# macros for gcc
AS_DEFS := 

C_DEFS :=  \
-DCORE_CM7 \
-DUSE_HAL_DRIVER \
-DSTM32H745xx

CXX_DEFS := $(C_DEFS) 

AS_INCLUDES := 

C_INCLUDES :=  \
-I$(root_dir)/CM7/Core/Inc \
-I$(root_dir)/Middlewares/ST/AI/Inc \
-I$(root_dir)/CM7/X-CUBE-AI/App \
-I$(root_dir)/Drivers/STM32H7xx_HAL_Driver/Inc \
-I$(root_dir)/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy \
-I$(root_dir)/Drivers/CMSIS/Device/ST/STM32H7xx/Include \
-I$(root_dir)/Drivers/CMSIS/Include \
-I$(root_dir)/Middlewares/Third_Party/FreeRTOS/Source/include \
-I$(root_dir)/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F

# todo: is the standard lib include required here
CXX_INCLUDES := $(C_INCLUDES) \
-I/usr/lib/arm-none-eabi/include/c++/10.3.1 \
-I$(root_dir)/CM7/Core/Src/hello_world

# compile gcc flags
ASFLAGS := $(OPT) -Wall -fdata-sections -ffunction-sections

# todo remove the parts that are not flags so that it can be customized in dep libs
CFLAGS += \
	$(OPT) \
	-Wall \
	-fdata-sections \
	-ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g3 -gdwarf-2
endif

# Generate dependency information (on every build)
# MMD is the flag to generate the dependency infromation
# MP generates a phony target for the header files, the rules don't fail then if the header is removed (like this: `header.h: `)
# MF is to specify the name of the dependency file
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

CXXFLAGS = \
	$(OPT) \
	-std=c++17 \
	-Wall \
	-fdata-sections \
	-ffunction-sections \
	-fno-rtti \
  	-fno-exceptions \
  	-fno-threadsafe-statics \
	-fno-unwind-tables \
	-fmessage-length=0

ifeq ($(DEBUG), 1)
CXXFLAGS += -g3 -gdwarf-2
endif

# Generate dependency information
CXXFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

#######################################
# tflite
#######################################
include tflite.mk

LIBS := -l:$(tflite_libname)
LIBDIR := -L$(dir $(tflite_lib))

CXX_DEFS += $(tflite_defs)
CXX_INCLUDES += $(tflite_includes)
C_INCLUDES += $(tflite_includes)

# todo: temporarily don't depend on Makefile to avoid long rebuilds
$(build_dir)/$(tflite_rel_src_dir)/%.o: $(root_dir)/$(tflite_rel_src_dir)/%.c
	@mkdir -p $(dir $@)
	$(log) building $(patsubst $(root_dir)/%,%,$@)
	$(q)$(CC) -c $(mcu) $(C_DEFS) $(C_INCLUDES) $(CFLAGS) -Wa,-a,-ad,-alms=$(@:.o=.lst) $< -o $@

$(build_dir)/$(tflite_rel_src_dir)/%.o: $(root_dir)/$(tflite_rel_src_dir)/%.cc
	@mkdir -p $(dir $@)
	$(log) building $(patsubst $(root_dir)/%,%,$@)
	$(q)$(CXX) -c $(mcu) $(CXX_DEFS) $(CXX_INCLUDES) $(CXXFLAGS) -Wa,-a,-ad,-alms=$(@:.o=.lst) $< -o $@

#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT := stm32h745xx_flash_CM7.ld

# libraries
LIBS += -lc -lm -lnosys
LDFLAGS := $(mcu) -specs=nano.specs -u _printf_float -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(build_dir)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(build_dir)/$(TARGET).elf $(build_dir)/$(TARGET).hex $(build_dir)/$(TARGET).bin $(build_dir)/$(TARGET).list

tflite: $(tflite_lib)

#######################################
# build the application
#######################################
# list of objects
OBJECTS := $(patsubst $(root_dir)%,$(build_dir)%,$(C_SOURCES:.c=.o))
# list of c++ objects
OBJECTS += $(patsubst $(root_dir)%,$(build_dir)%,$(CXX_SOURCES:.cc=.o))
# list of ASM program objects
OBJECTS += $(patsubst $(root_dir)%,$(build_dir)%,$(ASM_SOURCES:.s=.o))
OBJECTS += $(patsubst $(root_dir)%,$(build_dir)%,$(ASMM_SOURCES:.S=.o))

$(build_dir)/%.o: $(root_dir)/%.c Makefile
	@mkdir -p $(dir $@)
	$(log) compiling $(patsubst $(root_dir)/%,%,$@)
	$(q)$(CC) -c $(mcu) $(C_DEFS) $(C_INCLUDES) $(CFLAGS) -Wa,-a,-ad,-alms=$(@:.o=.lst) $< -o $@

$(build_dir)/%.o: $(root_dir)/%.cc Makefile
	@mkdir -p $(dir $@)
	$(log) compiling $(patsubst $(root_dir)/%,%,$@)
	$(q)$(CXX) -c $(mcu) $(CXX_DEFS) $(CXX_INCLUDES) $(CXXFLAGS) -Wa,-a,-ad,-alms=$(@:.o=.lst) $< -o $@

$(build_dir)/%.o: $(root_dir)/%.s Makefile
	@mkdir -p $(dir $@)
	$(log) compiling $(patsubst $(root_dir)/%,%,$@)
	$(q)$(AS) -c $(mcu) $(C_DEFS) $(C_INCLUDES) $(CFLAGS) $< -o $@
$(build_dir)/%.o: $(root_dir)/%.S Makefile
	@mkdir -p $(dir $@)
	$(log) compiling $(patsubst $(root_dir)/%,%,$@)
	$(q)$(AS) -c $(mcu) $(C_DEFS) $(C_INCLUDES) $(CFLAGS) $< -o $@

$(build_dir)/$(TARGET).elf: $(OBJECTS) $(tflite_lib) Makefile
	@mkdir -p $(dir $@)
	$(log) linking $@
	$(q)$(CXX) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(build_dir)/%.hex: $(build_dir)/%.elf
	$(HEX) $< $@
	
$(build_dir)/%.bin: $(build_dir)/%.elf
	$(BIN) $< $@	

$(build_dir)/%.list: $(build_dir)/%.elf
	$(DP) -h -S $< > $@

#######################################
# clean up
#######################################
clean:
	-rm -fR $(build_dir)
  
#######################################
# dependencies
#######################################
-include $(OBJECTS:.o=.d)

# *** EOF ***
