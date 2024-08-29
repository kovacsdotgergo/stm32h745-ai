# overwriting the default rv options for AR
ARFLAGS := -rcs

ASFLAGS +=

CFLAGS += \
	$(opt) \
	-Wall \
	-Wextra \
	-Wdouble-promotion \
	-Wno-unused-parameter \
	-fdata-sections \
	-ffunction-sections
# todo:
#  -Wsign-compare \
#  -Wdouble-promotion \
#  -Ofast -ffast-math \
#  -DNDEBUG \
#  -Wall -Wextra  -Werror \
#  -fshort-enums -fshort-wchar \
# -fshort-enums: uses shorter data type for enums instead of int if possible
# -ffast-math: non ieee745 compliant, e.g. doesn't use nans, but is the fastest. It enables a set of flags, some of which can be used most of the time, others affect the result of computations.
# -fshort-wchar: forces wchar to be represented by short unsigned int
# -Wsign-compare: enabled by Wextra, warns when when a comparison between signed and unsigned values could produce an incorrect result when the signed value is converted to unsigned
# -Wdouble-promotion: warns when float is implicitly promoted to double

ifeq ($(debug), 1)
CFLAGS += -g3 -gdwarf-2
opt := -O0
else
opt := -Os
# TODO: CFLAGS += -DNDEBUG for assert
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