.PHONY: tflite debug

# Expected variables from toplevel makefile
# AR, ARFLAGS
# CXX, CXXFLAGS
# BUILD_DIR
# MCU


TFLITE_BUILD_DIR := $(BUILD_DIR)/tflite
TFLITE_SRC_DIR := ../../Middlewares/Third_Party/tflite_micro

TFLITE_LIB := $(TFLITE_BUILD_DIR)/libtflm.a # it is placed inside the build

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

TFLITE_DEFS = \
  	-DTF_LITE_STATIC_MEMORY \
	-DTF_LITE_MCU_DEBUG_LOG \
    -DPROJECT_GENERATION \
    -DCMSIS_NN

# sort also removes duplicates
# TFLITE_INCLUDES += $(sort $(dir $(shell find $(TFLITE_SRC_DIR) -name *.h)))
TFLITE_SOURCES = $(shell find $(TFLITE_SRC_DIR) -name *.cc)

TFLITE_OBJECTS = $(addprefix $(TFLITE_BUILD_DIR)/,$(notdir $(TFLITE_SOURCES:.cc=.o)))
vpath %.cc $(sort $(dir $(TFLITE_SOURCES)))


# Building the generated code of the tflite library
# todo: could use $(lastword $(MAKEFILE_LIST)) for the name of the current makefile (https://www.gnu.org/software/make/manual/html_node/Special-Variables.html)
$(TFLITE_LIB): $(TFLITE_OBJECTS) tflite.mk | $(TFLITE_BUILD_DIR)
	$(AR) $(ARFLAGS) $@ $(TFLITE_OBJECTS)

$(TFLITE_BUILD_DIR)/%.o: %.cc tflite.mk | $(TFLITE_BUILD_DIR)
	$(CXX) -c $(MCU) $(CXXFLAGS) $(TFLITE_INCLUDES) $(TFLITE_DEFS) -Wa,-a,-ad,-alms=$(TFLITE_BUILD_DIR)/$(notdir $(<:.cc=.lst)) $< -o $@

$(TFLITE_BUILD_DIR):
	mkdir $@

tflite: $(TFLITE_LIB)

debug:
	@echo $(AR) $(ARFLAGS)

# todo: try out, using the dependencies	
-include $(wildcard $(TFLITE_BUILD_DIR)/*.d)