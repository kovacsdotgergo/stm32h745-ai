# overwriting the default rv options for AR
ARFLAGS := -rcs

ASFLAGS +=

CFLAGS += \
	$(opt) \
	-Wall \
	-Wextra \
	-Wno-unused-parameter \
	-fdata-sections \
	-ffunction-sections

ifeq ($(debug), 1)
CFLAGS += -g3 -gdwarf-2
opt := -O0
else
opt := -Os
endif

CXXFLAGS += \
	$(opt) \
	-std=c++17 \
	-Wall \
	-Wextra \
	-Wno-unused-parameter \
	-fdata-sections \
	-ffunction-sections \
	-fno-rtti \
  	-fno-exceptions \
  	-fno-threadsafe-statics \
	-fno-unwind-tables \
	-fmessage-length=0

ifeq ($(debug), 1)
CXXFLAGS += -g3 -gdwarf-2
endif

# options to the linker
# some of them are recognised by gcc and g++, but some have to be passed with the -Wl flag directly to ld (https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html)
# the commands that are passed to ld have to be seperated by commas if they are supposed to be given to ld as seperate arguments
LDFLAGS += \
	$(mcu) \
	-specs=nano.specs \
	-u _printf_float \
	-Wl,-Map=$(build_dir)/$(target).map,--cref \
	-Wl,--gc-sections