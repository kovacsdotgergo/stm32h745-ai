#include "nn_framework.h"

#include <stdio.h>

#include "ai_datatypes_defines.h"
#include "ai_platform.h"
#include "models/sine_model.h"
#include "models/sine_model_data.h"
#include "tim.h"

// Chunk of memory used to hold intermediate values for neural network
AI_ALIGNED(4) static ai_u8 activations[AI_SINE_MODEL_DATA_ACTIVATIONS_SIZE];

// Buffers used to store input and output tensors
AI_ALIGNED(4) static ai_i8 in_data[AI_SINE_MODEL_IN_1_SIZE_BYTES];
AI_ALIGNED(4) static ai_i8 out_data[AI_SINE_MODEL_OUT_1_SIZE_BYTES];

// Pointer to our model
static ai_handle sine_model = AI_HANDLE_NULL;

// Wrapper structs that hold pointers to data and info about the data (tensor height, width, channels)
static ai_buffer* ai_input;
static ai_buffer* ai_output;

void ai_model_init() {
  // Initialize wrapper structs
  ai_input = AI_SINE_MODEL_IN;
  ai_output = AI_SINE_MODEL_OUT;
  // Set pointers wrapper structs to our data buffers
  // ai_input[0].n_batches = 1;
  ai_input[0].data = AI_HANDLE_PTR(in_data);
  // ai_output[0].n_batches = 1;
  ai_output[0].data = AI_HANDLE_PTR(out_data);

  // Greetings!
  printf("\r\n\r\nSTM32 X-Cube-AI test\r\n");

  // Create instance of neural network
  ai_error ai_err =
      ai_sine_model_create(&sine_model, AI_SINE_MODEL_DATA_CONFIG);
  if (ai_err.type != AI_ERROR_NONE) {
    printf("Error: could not create NN instance\r\n");
    while (1);
  }

  // Set working memory and get weights/biases from model
  ai_network_params ai_params = {
      AI_SINE_MODEL_DATA_WEIGHTS(ai_sine_model_data_weights_get()),
      AI_SINE_MODEL_DATA_ACTIVATIONS(activations)};

  // Initialize neural network
  if (!ai_sine_model_init(sine_model, &ai_params)) {
    printf("Error: could not initialize NN\r\n");
    while (1);
  }
}

void ai_model_run() {
  // Fill input buffer (use test value)
  for (uint32_t i = 0; i < AI_SINE_MODEL_IN_1_SIZE; i++) {
    ((ai_float*)in_data)[i] = (ai_float)2.0f;
  }

  // Get current timestamp
  uint32_t timestamp = __HAL_TIM_GET_COUNTER(&htim2);

  // Perform inference
  ai_i32 nbatch = ai_sine_model_run(sine_model, &ai_input[0], &ai_output[0]);

  if (nbatch != 1) {
    printf("Error: could not run inference\r\n");
  }

  // Read output (predicted y) of neural network
  float y_val = ((float*)out_data)[0];

  // Print output of neural network along with inference time (microseconds)
  uint32_t runtime = __HAL_TIM_GET_COUNTER(&htim2) - timestamp;
  printf("Output: %f | Duration: %f [us]\r\n", y_val,
         1000000.0F * (float)runtime / getTIM2Freq());
}
