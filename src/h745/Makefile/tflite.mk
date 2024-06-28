# Expected variables from toplevel makefile
# AR, ARFLAGS
# build_dir, root_dir

tflite_makefile := $(lastword $(MAKEFILE_LIST))
tflite_libname := libtflm.a
tflite_lib := $(build_dir)/$(tflite_libname) # it is placed inside build
tflite_rel_src_dir := Middlewares/Third_Party/tflite_micro
tflite_src_dir := $(root_dir)/$(tflite_rel_src_dir)

# todo: might need to add the printf library, for that base third_party
tflite_includes := \
	-I$(tflite_src_dir) \
	-I$(tflite_src_dir)/third_party/gemmlowp \
	-I$(tflite_src_dir)/third_party/flatbuffers/include \
	-I$(tflite_src_dir)/third_party/kissfft \
	-I$(tflite_src_dir)/third_party/kissfft/tools \
	-I$(tflite_src_dir)/third_party/ruy
# cmsis_nn includes
tflite_includes += \
	-I$(tflite_src_dir)/third_party/cmsis \
    -I$(tflite_src_dir)/third_party/cmsis_nn \
    -I$(tflite_src_dir)/third_party/cmsis_nn/Include \
    -I$(tflite_src_dir)/third_party/cmsis/CMSIS/Core/Include

tflite_defs := \
  	-DTF_LITE_STATIC_MEMORY \
	-DTF_LITE_MCU_DEBUG_LOG \
    -DCMSIS_NN
    # -DPROJECT_GENERATION

# sort also removes duplicates
tflite_sources := $(shell (cd $(root_dir) && find $(tflite_rel_src_dir) -name *.cc -o -name *.c))

tflite_objects := $(addprefix $(build_dir)/,$(patsubst %.c,%.o,$(patsubst %.cc,%.o,$(tflite_sources))))

# Building the generated code of the tflite library
# todo: could use $(lastword $(MAKEFILE_LIST)) for the name of the current makefile (https://www.gnu.org/software/make/manual/html_node/Special-Variables.html)
$(tflite_lib): $(tflite_objects) $(tflite_makefile)
	@mkdir -p $(dir $@)
	$(log) archiving $(patsubst $(root_dir)/%,%,$@)
	$(q)$(AR) $(ARFLAGS) $@ $(tflite_objects)
