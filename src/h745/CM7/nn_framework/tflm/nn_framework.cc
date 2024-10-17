/* Copyright 2023 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "nn_framework.h"

#include <math.h>

#include <cstdio>
#include <cstring>

#include "kws_test_file3.h"
#include "models/kws_model_quant.h"
// #include "models/sine_model_dyn_quant.h"
// #include "models/sine_model_fb_quant.h"
// #include "models/sine_model_full_quant.h"
// #include "models/sine_model_no_quant.h"
#include "tensorflow/lite/core/c/common.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_profiler.h"
#include "tensorflow/lite/micro/recording_micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tim.h"
#include "benchmark.h"

const uint8_t* test_file_arr = src_nn_kws_bin_files_mfcc_tst_000103_On_5_bin;
const unsigned int test_file_len =
    src_nn_kws_bin_files_mfcc_tst_000103_On_5_bin_len;

namespace {
// the same resolver works for the float and quantized models
constexpr int kNumberOperators = 6;
using KwsOpResolver = tflite::MicroMutableOpResolver<kNumberOperators>;

TfLiteStatus RegisterOps(KwsOpResolver& op_resolver) {
  TF_LITE_ENSURE_STATUS(op_resolver.AddAveragePool2D());
  TF_LITE_ENSURE_STATUS(op_resolver.AddConv2D());
  TF_LITE_ENSURE_STATUS(op_resolver.AddDepthwiseConv2D());
  TF_LITE_ENSURE_STATUS(op_resolver.AddFullyConnected());
  TF_LITE_ENSURE_STATUS(op_resolver.AddReshape());
  TF_LITE_ENSURE_STATUS(op_resolver.AddSoftmax());

  return kTfLiteOk;
}
}  // namespace

// for quantized model
TfLiteStatus ProfileMemoryAndLatency(const void* p_model) {
  tflite::MicroProfiler profiler;
  KwsOpResolver op_resolver;
  MicroPrintf("Registering ops");
  TF_LITE_ENSURE_STATUS(RegisterOps(op_resolver));

  // Arena size just a round number. The exact arena usage can be determined
  // using the RecordingMicroInterpreter.
  constexpr int kTensorArenaSize = 24000;
  uint8_t tensor_arena[kTensorArenaSize];
  constexpr int kNumResourceVariables = 24;

  tflite::RecordingMicroAllocator* allocator(
      tflite::RecordingMicroAllocator::Create(tensor_arena, kTensorArenaSize));
  tflite::RecordingMicroInterpreter interpreter(
      tflite::GetModel(p_model), op_resolver, allocator,
      tflite::MicroResourceVariables::Create(allocator, kNumResourceVariables),
      &profiler);

  TF_LITE_ENSURE_STATUS(interpreter.AllocateTensors());
  // TFLITE_CHECK_EQ(interpreter.inputs_size(), 49 * 10);
  MicroPrintf("Inputs size: %d", interpreter.inputs_size());

  const size_t dimsize = interpreter.input(0)->dims->size;
  MicroPrintf("dim len: %d", dimsize);
  for (size_t i = 0; i < dimsize; ++i) {
    MicroPrintf("dim [%d]: %d", i, interpreter.input(0)->dims->data[i]);
  }

  interpreter.input(0)->data.int8[0] = 1;
  TF_LITE_ENSURE_STATUS(interpreter.Invoke());

  MicroPrintf("");  // Print an empty new line
  profiler.LogTicksPerTagCsv();

  MicroPrintf("");  // Print an empty new line
  interpreter.GetMicroAllocator().PrintAllocations();
  return kTfLiteOk;
}

TfLiteStatus LoadQuantModelAndPerformInference(const void* p_model, const int8_t* wave) {
  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  const tflite::Model* model = ::tflite::GetModel(p_model);
  benchmark_set_point(INSIDE_LOAD_MODEL); //-------------------------------
  TFLITE_CHECK_EQ(model->version(), TFLITE_SCHEMA_VERSION);

  KwsOpResolver op_resolver;
  TF_LITE_ENSURE_STATUS(RegisterOps(op_resolver));

  // Arena size just a round number. The exact arena usage can be determined
  // using the RecordingMicroInterpreter.
  constexpr int kTensorArenaSize = 24000;
  uint8_t tensor_arena[kTensorArenaSize];

  tflite::MicroInterpreter interpreter(model, op_resolver, tensor_arena,
                                       kTensorArenaSize);

  TF_LITE_ENSURE_STATUS(interpreter.AllocateTensors());
  benchmark_set_point(INSIDE_SETUP); // -----------------------------------

  TfLiteTensor* input = interpreter.input(0);
  TFLITE_CHECK_NE(input, nullptr);

  TfLiteTensor* output = interpreter.output(0);
  TFLITE_CHECK_NE(output, nullptr);

  float output_scale = output->params.scale;
  int output_zero_point = output->params.zero_point;

  // filling the inputs with the test data
  assert(input->bytes == test_file_len);
  auto* inputs = tflite::GetTensorData<int8_t>(input);
  std::memcpy(inputs, wave, test_file_len);
  // tflite::GetTensorData<int8_t>(input)[0] = golden_input;
  // input->data.int8[0] = golden_input;

  printf("INputs\r\n");
  for (size_t i = 0; i < input->bytes; ++i) {
    std::printf("0x%2x ", input->data.uint8[i]);
    constexpr size_t line_len = 16;
    if (line_len - 1 == i % line_len) MicroPrintf("");
  }
  MicroPrintf("");

  benchmark_set_point(INSIDE_BEFORE_INVOKE); //----------------------------
  TF_LITE_ENSURE_STATUS(interpreter.Invoke());
  benchmark_set_point(INSIDE_AFTER_INVOKE); //-----------------------------

  MicroPrintf("");
  for (size_t i = 0; i < output->bytes; ++i) {
    float y_pred = (output->data.int8[i] - output_zero_point) * output_scale;
    std::printf("%.2f ", y_pred);
  }

  return kTfLiteOk;
}

void ai_model_init() {

}

void ai_model_run(const int8_t* wave) {
  tflite::InitializeTarget();
  printf("STARTING TEST\r\n");
  // assert(kTfLiteOk == ProfileMemoryAndLatency(g_kws_model_quant_data));
  assert(kTfLiteOk == LoadQuantModelAndPerformInference(g_kws_model_quant_data, wave));
  printf("END OF TEST");
}
