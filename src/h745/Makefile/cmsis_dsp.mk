
# Compilation tools
CC := arm-none-eabi-gcc
ARMAR :=  arm-none-eabi-ar

cpu := -mcpu=cortex-m7
fpu := -mfpu=fpv5-d16
float-abi := -mfloat-abi=hard
mcu := $(cpu) -mthumb $(fpu) $(float-abi)
CFLAGS += $(mcu) -g3 -gdwarf-2 -O0 \
	-Wall \
	-Wextra \
	-Wno-unused-parameter \
	-fdata-sections \
	-ffunction-sections
# todo for opt -DNDEBUG

# Path to CMSIS_5
CMSIS_5 := /home/gergo/workspace/stm32h745-ai/src/h745/Drivers/CMSIS

# Path to CMSIS_DSP
CMSIS_DSP := /home/gergo/workspace/stm32h745-ai/src/h745/Drivers/CMSIS-DSP

# Path to CMSIS Core includes for Cortex-M
# For low end Cortex-A, use Core_A
# For high end Cortex-A (aarch64), don't use CMSIS 
# Core Includes (Refer to the CMSIS-DSP README to 
# know how to build in that case)
CMSIS_CORE_INCLUDES := $(CMSIS_5)/Include 

# Sources
SRCS := $(CMSIS_DSP)/Source/TransformFunctions/TransformFunctions.c

# Includes
DSP_INCLUDES := $(CMSIS_DSP)/Include \
  $(CMSIS_DSP)/PrivateInclude 

# Compilation flags for include folders
INC_FLAGS := $(addprefix -I,$(DSP_INCLUDES))
INC_FLAGS += $(addprefix -I,$(CMSIS_CORE_INCLUDES))
CFLAGS += $(INC_FLAGS)

# Output folder for build products
BUILDDIR := ./tmp

OBJECTS := $(SRCS:%=$(BUILDDIR)/%.o)

# Build rules
$(BUILDDIR)/libCMSISDSP.a: $(OBJECTS)
	$(ARMAR) -rc $@ $(OBJECTS)
	

$(BUILDDIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@
