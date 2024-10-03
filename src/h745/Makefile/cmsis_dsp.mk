dsp_makefile := $(lastword $(MAKEFILE_LIST))

dsp_libname := libCMSISDSP.a
dsp_lib := $(build_dir)/$(dsp_libname)
cmsis_rel_dir := Drivers/CMSIS
cmsis_src_dir := $(root_dir)/cmsis_rel_dir
cmsis_dsp_rel_dir := Drivers/CMSIS-DSP
cmsis_dsp_src_dir := $(root_dir)/$(cmsis_dsp_rel_dir)

dsp_includes := \
	-I$(cmsis_5_src_dir)/Include \
	-I$(cmsis_dsp_src_dir)/Include \
	-I$(cmsis_dsp_src_dir)/PrivateInclude # might not even be present if not used

# special rules for building this library, these files listed here include all other needed source files, so only these have to be built
dsp_sources := \
	$(cmsis_dsp_src_dir)/Source/TransformFunctions/TransformFunctions.c \
	$(cmsis_dsp_src_dir)/Source/CommonTables/CommonTables.c \
	$(cmsis_dsp_src_dir)/Source/FastMathFunctions/FastMathFunctions.c \
	$(cmsis_dsp_src_dir)/Source/ComplexMathFunctions/ComplexMathFunctions.c \
	$(cmsis_dsp_src_dir)/Source/BasicMathFunctions/BasicMathFunctions.c \
	$(cmsis_dsp_src_dir)/Source/MatrixFunctions/MatrixFunctions.c \
	$(cmsis_dsp_src_dir)/Source/StatisticsFunctions/StatisticsFunctions.c

dsp_objects  := $(patsubst $(root_dir)%,$(build_dir)%,$(dsp_sources:.c=.o))

# objects are built like all others if the lib is used
$(dsp_lib): $(dsp_objects) $(dsp_makefile)
	@mkdir -p $(dir $@)
	$(log) archiving $(patsubst $(root_dir)/%,%,$@)
	$(q)$(AR) $(ARFLAGS) $@ $(dsp_objects)
