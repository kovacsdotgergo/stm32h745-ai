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
 