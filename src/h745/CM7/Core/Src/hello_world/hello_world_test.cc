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

#include <math.h>

#include <cstdio>

#include "models/sine_model_dyn_quant.h"
#include "models/sine_model_fb_quant.h"
#include "models/sine_model_full_quant.h"
#include "models/sine_model_no_quant.h"
#include "tensorflow/lite/core/c/common.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_profiler.h"
#include "tensorflow/lite/micro/recording_micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

namespace {
using QuantOpResolver = tflite::MicroMutableOpResolver<1>;
using FloatOpResolver = tflite::MicroMutableOpResolver<3>;

TfLiteStatus RegisterOps(QuantOpResolver& op_resolver) {
  TF_LITE_ENSURE_STATUS(op_resolver.AddFullyConnected());
  return kTfLiteOk;
}

TfLiteStatus RegisterOps(FloatOpResolver& op_resolver) {
  TF_LITE_ENSURE_STATUS(op_resolver.AddFullyConnected());
  TF_LITE_ENSURE_STATUS(op_resolver.AddQuantize());
  TF_LITE_ENSURE_STATUS(op_resolver.AddDequantize());
  return kTfLiteOk;
}
}  // namespace

TfLiteStatus ProfileMemoryAndLatency(const void* p_model) {
  tflite::MicroProfiler profiler;
  FloatOpResolver op_resolver;
  TF_LITE_ENSURE_STATUS(RegisterOps(op_resolver));

  // Arena size just a round number. The exact arena usage can be determined
  // using the RecordingMicroInterpreter.
  constexpr int kTensorArenaSize = 3000;
  uint8_t tensor_arena[kTensorArenaSize];
  constexpr int kNumResourceVariables = 24;

  tflite::RecordingMicroAllocator* allocator(
      tflite::RecordingMicroAllocator::Create(tensor_arena, kTensorArenaSize));
  tflite::RecordingMicroInterpreter interpreter(
      tflite::GetModel(p_model), op_resolver, allocator,
      tflite::MicroResourceVariables::Create(allocator, kNumResourceVariables),
      &profiler);

  TF_LITE_ENSURE_STATUS(interpreter.AllocateTensors());
  TFLITE_CHECK_EQ(interpreter.inputs_size(), 1);
  interpreter.input(0)->data.f[0] = 1.f;
  TF_LITE_ENSURE_STATUS(interpreter.Invoke());

  MicroPrintf("");  // Print an empty new line
  profiler.LogTicksPerTagCsv();

  MicroPrintf("");  // Print an empty new line
  interpreter.GetMicroAllocator().PrintAllocations();
  return kTfLiteOk;
}

TfLiteStatus LoadFloatModelAndPerformInference(const void* p_model) {
  const tflite::Model* model = ::tflite::GetModel(p_model);
  TFLITE_CHECK_EQ(model->version(), TFLITE_SCHEMA_VERSION);

  FloatOpResolver op_resolver;
  TF_LITE_ENSURE_STATUS(RegisterOps(op_resolver));

  // Arena size just a round number. The exact arena usage can be determined
  // using the RecordingMicroInterpreter.
  constexpr int kTensorArenaSize = 3000;
  uint8_t tensor_arena[kTensorArenaSize];

  tflite::MicroInterpreter interpreter(model, op_resolver, tensor_arena,
                                       kTensorArenaSize);
  TF_LITE_ENSURE_STATUS(interpreter.AllocateTensors());

  // Check if the predicted output is within a small range of the
  // expected output
  float epsilon = 0.05f;
  float golden_inputs[] = {0.0F, 0.5F, 1.0F, 3.0F, 5.0F};
  constexpr int kNumTestValues =
      sizeof(golden_inputs) / sizeof(golden_inputs[0]);

  for (int i = 0; i < kNumTestValues; ++i) {
    interpreter.input(0)->data.f[0] = golden_inputs[i];
    TF_LITE_ENSURE_STATUS(interpreter.Invoke());
    float y_pred = interpreter.output(0)->data.f[0];
    printf("float[%d] out: %f correct: %f\r\n", i, y_pred, sin(golden_inputs[i]));
  }

  return kTfLiteOk;
}

TfLiteStatus LoadQuantModelAndPerformInference(const void* p_model) {
  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  const tflite::Model* model = ::tflite::GetModel(p_model);
  TFLITE_CHECK_EQ(model->version(), TFLITE_SCHEMA_VERSION);

  QuantOpResolver op_resolver;
  TF_LITE_ENSURE_STATUS(RegisterOps(op_resolver));

  // Arena size just a round number. The exact arena usage can be determined
  // using the RecordingMicroInterpreter.
  constexpr int kTensorArenaSize = 3000;
  uint8_t tensor_arena[kTensorArenaSize];

  tflite::MicroInterpreter interpreter(model, op_resolver, tensor_arena,
                                       kTensorArenaSize);

  TF_LITE_ENSURE_STATUS(interpreter.AllocateTensors());

  TfLiteTensor* input = interpreter.input(0);
  TFLITE_CHECK_NE(input, nullptr);

  TfLiteTensor* output = interpreter.output(0);
  TFLITE_CHECK_NE(output, nullptr);

  float output_scale = output->params.scale;
  int output_zero_point = output->params.zero_point;

  // Check if the predicted output is within a small range of the
  // expected output
  float epsilon = 0.05;

  constexpr int kNumTestValues = 4;
  float golden_inputs_float[kNumTestValues] = {0.77, 1.57, 2.3, 3.14};

  // The int8 values are calculated using the following formula
  // (golden_inputs_float[i] / input->params.scale + input->params.scale)
  int8_t golden_inputs_int8[kNumTestValues];
  for (size_t i = 0; i < kNumTestValues; ++i) {
    golden_inputs_int8[i] =
        golden_inputs_float[i] / input->params.scale + input->params.zero_point;
  }

  for (int i = 0; i < kNumTestValues; ++i) {
    input->data.int8[0] = golden_inputs_int8[i];
    TF_LITE_ENSURE_STATUS(interpreter.Invoke());
    float y_pred = (output->data.int8[0] - output_zero_point) * output_scale;
    printf("quant[%d] out: %f correct: %f\r\n", i, y_pred,
           sin(golden_inputs_float[i]));
  }

  return kTfLiteOk;
}

int tflite_helloworld(void) {
  tflite::InitializeTarget();
  TF_LITE_ENSURE_STATUS(ProfileMemoryAndLatency(sine_model_no_quant_tflite));
  MicroPrintf("NO quantization");
  TF_LITE_ENSURE_STATUS(
      LoadFloatModelAndPerformInference(sine_model_no_quant_tflite));
  MicroPrintf("Fallback quantization");
  TF_LITE_ENSURE_STATUS(
      LoadFloatModelAndPerformInference(sine_model_fb_quant_tflite));
  MicroPrintf("Dynamic quantization");
  TF_LITE_ENSURE_STATUS(
      LoadFloatModelAndPerformInference(sine_model_dyn_quant_tflite));
  MicroPrintf("Full quantization");
  TF_LITE_ENSURE_STATUS(
      LoadQuantModelAndPerformInference(sine_model_full_quant_tflite));
  MicroPrintf("~~~END OF HELLO WORLD~~~\n");
  return kTfLiteOk;
}
