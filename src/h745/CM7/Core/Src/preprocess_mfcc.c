#include "preprocess_mfcc.h"

#include <assert.h>
#include <string.h>

#include "dsp/transform_functions.h"

#define STFT_STRIDE 320
#define STFT_FFT_SIZE 512
#define NUM_MEL_FILTERS 40

static const float32_t dct_coefs[] = {
#include "mfcc_config/dct_coefs_float32.h"
};
static const uint32_t filter_pos[] = {
#include "mfcc_config/filt_pos_float32.h"
};
static const uint32_t filter_lengths[] = {
#include "mfcc_config/filt_len_float32.h"
};
static const float32_t filter_coefs[] = {
#include "mfcc_config/filter_coefs_float32.h"
};
static const float32_t window_coefs[] = {
#include "mfcc_config/window_coefs_float32.h"
};
static arm_mfcc_instance_f32 instance;

// globals to save stack space
static float32_t scratchpad[STFT_FFT_SIZE + 2];
static float32_t waveform_float[WAVEFORM_LEN];

void preprocess_init(void) {
  arm_status status = arm_mfcc_init_512_f32(
      &instance, NUM_MEL_FILTERS, MFCC_NUM_DCT_OUTPUTS, dct_coefs, filter_pos,
      filter_lengths, filter_coefs, window_coefs);
  assert(ARM_MATH_SUCCESS == status);
}

void preprocess_calculate(int16_t waveform[], float32_t mfcc[]) {
  // implementation for float32
  // todo convert the int16 values to f32
  for (size_t i = 0; i < WAVEFORM_LEN; ++i) {
    waveform_float[i] = (float32_t)waveform[i];
  }

  // normalize the input
  // TODO use the cmsis max function
  float32_t max = waveform_float[0];
  for (size_t i = 1; i < WAVEFORM_LEN; ++i) {
    if (max < waveform_float[i]) max = waveform_float[i];
  }
  // todo use cmsis scale
  for (size_t i = 0; i < WAVEFORM_LEN; ++i) {
    waveform_float[i] /= max;
  }

  // transform
  // todo: possible optimization: only copy the overlapping part
  static float32_t input_scratch[STFT_FFT_SIZE];
  for (size_t step = 0; step < MFCC_TIMESTEPS; ++step) {
    memcpy(input_scratch, &waveform_float[step * STFT_STRIDE],
           sizeof(input_scratch));
    arm_mfcc_f32(&instance, input_scratch, &mfcc[step * MFCC_NUM_DCT_OUTPUTS],
                 scratchpad);
  }
  // deconvert, nop for float
}

void preprocess_calculate_uint32(int16_t waveform[], uint32_t mfcc[]) {
  // convert input values
  // normalize
  // transfrom
}

void preprocess_calculate_uint16(int16_t waveform[], uint16_t mfcc[]) {
  // convert input values
  // normalize
  // transfrom
}