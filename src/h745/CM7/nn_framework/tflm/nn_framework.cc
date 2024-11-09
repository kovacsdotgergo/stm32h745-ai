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

#include "benchmark.h"
#include "kws_test_file3.h"
#include "mfcc_config/shapes_config.h"
#include "models/kws_model_quant.h"
#include "postprocess.h"
#include "tensorflow/lite/core/c/common.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_profiler.h"
#include "tensorflow/lite/micro/recording_micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

namespace {
// the same resolver works for the float and quantized models
constexpr int kNumberOperators = 6;
using KwsOpResolver = tflite::MicroMutableOpResolver<kNumberOperators>;

tflite::MicroInterpreter* interpreter = nullptr;

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

// returns pointer to the initialized static local interpreter in the function
static tflite::MicroInterpreter* init_interpreter() {
  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  static const tflite::Model* model =
      ::tflite::GetModel(g_kws_model_quant_data);
  TFLITE_CHECK_EQ(model->version(), TFLITE_SCHEMA_VERSION);

  static KwsOpResolver op_resolver;
  TfLiteStatus status = RegisterOps(op_resolver);
  TFLITE_CHECK_EQ(status, kTfLiteOk);

  // Arena size just a round number. The exact arena usage can be determined
  // using the RecordingMicroInterpreter.
  constexpr int kTensorArenaSize = 24000;
  static uint8_t tensor_arena[kTensorArenaSize];

  static tflite::MicroInterpreter interpreter{model, op_resolver, tensor_arena,
                                              kTensorArenaSize};

  status = interpreter.AllocateTensors();
  TFLITE_DCHECK_EQ(status, kTfLiteOk);

  return &interpreter;
}

void ai_model_init() {
  tflite::InitializeTarget();
  interpreter = init_interpreter();
}

void ai_get_input_quant_details(float* scale, int32_t* zero_point) {
  TFLITE_DCHECK_NE(interpreter, nullptr);
  TfLiteTensor* input = interpreter->input(0);
  TFLITE_CHECK_NE(input, nullptr);

  *scale = input->params.scale;
  *zero_point = input->params.zero_point;
}

void ai_model_run(const int8_t* mfcc, float* probabilities) {
  TFLITE_DCHECK_NE(interpreter, nullptr);

  TfLiteTensor* input = interpreter->input(0);
  TFLITE_CHECK_NE(input, nullptr);

  TfLiteTensor* output = interpreter->output(0);
  TFLITE_CHECK_NE(output, nullptr);

  float output_scale = output->params.scale;
  int32_t output_zero_point = output->params.zero_point;

  // filling the inputs with the test data
  auto* inputs = tflite::GetTensorData<int8_t>(input);
  std::memcpy(inputs, mfcc, MFCC_TOTAL_LENGTH * sizeof(*mfcc));

  benchmark_set_point(TASK_RUN_BEFORE_INVOKE);
  TfLiteStatus status = interpreter->Invoke();
  TFLITE_CHECK_EQ(status, kTfLiteOk);
  benchmark_set_point(TASK_RUN_AFTER_INVOKE);

  TFLITE_DCHECK_EQ(output->bytes, POSTPROCESS_LABEL_NUM);
  for (size_t i =0; i < output->bytes; ++i) {
    probabilities[i] =
        (output->data.int8[i] - output_zero_point) * output_scale;
  }
}

// todo: is this already measured?
// assert(kTfLiteOk == ProfileMemoryAndLatency(g_kws_model_quant_data));

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
