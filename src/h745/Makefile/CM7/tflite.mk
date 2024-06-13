.PHONY: tflite debug

# Expected variables from toplevel makefile
# AR, ARFLAGS
# BUILD_DIR
# CXX, CXXFLAGS, CC, CFLAGS
# MCU
# TFLITE_LIB

ROOT_DIR := ../..
TFLITE_REL_SRC_DIR := Middlewares/Third_Party/tflite_micro
TFLITE_SRC_DIR := $(ROOT_DIR)/$(TFLITE_REL_SRC_DIR)

# todo: might need to add the printf library, for that base third_party
TFLITE_INCLUDES := \
	-I$(TFLITE_SRC_DIR) \
	-I$(TFLITE_SRC_DIR)/third_party/gemmlowp \
	-I$(TFLITE_SRC_DIR)/third_party/flatbuffers/include \
	-I$(TFLITE_SRC_DIR)/third_party/kissfft \
	-I$(TFLITE_SRC_DIR)/third_party/kissfft/tools \
	-I$(TFLITE_SRC_DIR)/third_party/ruy
# cmsis_nn includes
TFLITE_INCLUDES += \
	-I$(TFLITE_SRC_DIR)/third_party/cmsis \
    -I$(TFLITE_SRC_DIR)/third_party/cmsis_nn \
    -I$(TFLITE_SRC_DIR)/third_party/cmsis_nn/Include \
    -I$(TFLITE_SRC_DIR)/third_party/cmsis/CMSIS/Core/Include

TFLITE_DEFS := \
  	-DTF_LITE_STATIC_MEMORY \
	-DTF_LITE_MCU_DEBUG_LOG \
    -DPROJECT_GENERATION \
    -DCMSIS_NN

# sort also removes duplicates
# TFLITE_INCLUDES += $(sort $(dir $(shell find $(TFLITE_SRC_DIR) -name *.h)))
TFLITE_SOURCES := $(shell (cd $(ROOT_DIR) && find $(TFLITE_REL_SRC_DIR) -name *.cc -o -name *.c))

TFLITE_OBJECTS := $(addprefix $(BUILD_DIR)/,$(patsubst %.c,%.o,$(patsubst %.cc,%.o,$(TFLITE_SOURCES))))

# Building the generated code of the tflite library
# todo: could use $(lastword $(MAKEFILE_LIST)) for the name of the current makefile (https://www.gnu.org/software/make/manual/html_node/Special-Variables.html)
$(TFLITE_LIB): $(TFLITE_OBJECTS) tflite.mk
	@mkdir -p $(dir $@)
	$(AR) $(ARFLAGS) $@ $(TFLITE_OBJECTS)

# currently only the tflite sources are built in subdirectories, so these rules only apply then
$(BUILD_DIR)/%.o: $(ROOT_DIR)/%.cc tflite.mk
	@mkdir -p $(dir $@)
	$(CXX) -c $(MCU) $(CXXFLAGS) $(TFLITE_INCLUDES) $(TFLITE_DEFS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.cc=.lst)) $< -o $@

$(BUILD_DIR)/%.o: $(ROOT_DIR)/%.c tflite.mk
	@mkdir -p $(dir $@)
	$(CC) -c $(MCU) $(CFLAGS) $(TFLITE_INCLUDES) $(TFLITE_DEFS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

#debug:
#	@echo $(TFLITE_OBJECTS:.o=.d)

# todo: try out, using the dependencies	
-include $(TFLITE_OBJECTS:.o=.d)