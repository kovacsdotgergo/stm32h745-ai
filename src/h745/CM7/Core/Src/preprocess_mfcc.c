#include "preprocess_mfcc.h"

#include <assert.h>
#include <string.h>

#include "dsp/statistics_functions.h"
#include "dsp/support_functions.h"
#include "dsp/transform_functions.h"
#include "macros.h"

#define STFT_STRIDE 320
#define STFT_FFT_SIZE 512
#define NUM_MEL_FILTERS 40

// filter positions are the same for all data types
static const uint32_t filter_pos[] = {
#include "mfcc_config/filt_pos_float32.h"
};
static const uint32_t filter_lengths[] = {
#include "mfcc_config/filt_len_float32.h"
};

static const float32_t dct_coefs_f32[] = {
#include "mfcc_config/dct_coefs_float32.h"
};
static const float32_t filter_coefs_f32[] = {
#include "mfcc_config/filter_coefs_float32.h"
};
static const float32_t window_coefs_f32[] = {
#include "mfcc_config/window_coefs_float32.h"
};

static const q15_t dct_coefs_q15[] = {
#include "mfcc_config/dct_coefs_q15.h"
};
static const q15_t filter_coefs_q15[] = {
#include "mfcc_config/filter_coefs_q15.h"
};
static const q15_t window_coefs_q15[] = {
#include "mfcc_config/window_coefs_q15.h"
};

static const q31_t dct_coefs_q31[] = {
#include "mfcc_config/dct_coefs_q31.h"
};
static const q31_t filter_coefs_q31[] = {
#include "mfcc_config/filter_coefs_q31.h"
};
static const q31_t window_coefs_q31[] = {
#include "mfcc_config/window_coefs_q31.h"
};

static arm_mfcc_instance_f32 instance_f32;
static arm_mfcc_instance_q31 instance_q31;
static arm_mfcc_instance_q15 instance_q15;

void preprocess_init_f32(void) {
  arm_status status = arm_mfcc_init_512_f32(
      &instance_f32, NUM_MEL_FILTERS, MFCC_NUM_DCT_OUTPUTS, dct_coefs_f32,
      filter_pos, filter_lengths, filter_coefs_f32, window_coefs_f32);
  assert(ARM_MATH_SUCCESS == status);
}

void preprocess_init_q31(void) {
  arm_status status = arm_mfcc_init_512_q31(
      &instance_q31, NUM_MEL_FILTERS, MFCC_NUM_DCT_OUTPUTS, dct_coefs_q31,
      filter_pos, filter_lengths, filter_coefs_q31, window_coefs_q31);
  assert(ARM_MATH_SUCCESS == status);
}

void preprocess_init_q15(void) {
  arm_status status = arm_mfcc_init_512_q15(
      &instance_q15, NUM_MEL_FILTERS, MFCC_NUM_DCT_OUTPUTS, dct_coefs_q15,
      filter_pos, filter_lengths, filter_coefs_q15, window_coefs_q15);
  assert(ARM_MATH_SUCCESS == status);
}

void preprocess_calculate_f32(int16_t waveform[], float32_t mfcc[]) {
  // todo: static to save stack space
  static float32_t scratchpad[STFT_FFT_SIZE + 2];
  static float32_t waveform_inner_dtype[WAVEFORM_LEN];
  // implementation for float32
  for (size_t i = 0; i < WAVEFORM_LEN; ++i) {
    waveform_inner_dtype[i] = (float32_t)waveform[i];
  }

  // intentionally normalize with max instead of absmax (same as tensorflow
  // implmementation)
  // TODO use the cmsis max function
  float32_t max = waveform_inner_dtype[0];
  for (size_t i = 1; i < WAVEFORM_LEN; ++i) {
    if (max < waveform_inner_dtype[i]) max = waveform_inner_dtype[i];
  }
  // todo use cmsis scale
  for (size_t i = 0; i < WAVEFORM_LEN; ++i) {
    waveform_inner_dtype[i] /= max;
  }

  // transform
  // todo: possible optimization: only copy the overlapping part
  static float32_t input_scratch[STFT_FFT_SIZE];
  for (size_t step = 0; step < MFCC_TIMESTEPS; ++step) {
    memcpy(input_scratch, &waveform_inner_dtype[step * STFT_STRIDE],
           sizeof(input_scratch));
    arm_mfcc_f32(&instance_f32, input_scratch,
                 &mfcc[step * MFCC_NUM_DCT_OUTPUTS], scratchpad);
  }
}

void preprocess_calculate_q31(int16_t waveform[], q31_t mfcc[]) {
  // normalize
  static q31_t waveform_inner_dtype[WAVEFORM_LEN];
  arm_q15_to_q31(waveform, waveform_inner_dtype, WAVEFORM_LEN);

  // this differs to tensorflow (absmax instead of max)
  q31_t max;
  arm_absmax_no_idx_q31(waveform_inner_dtype, WAVEFORM_LEN, &max);
  if ((max != 0) && (max != 0x7FFFFFFF)) {
    q31_t quotient;
    int16_t shift;

    arm_status status = arm_divide_q31(0x7FFFFFFF, max, &quotient, &shift);
    assert(status == ARM_MATH_SUCCESS);  // as max == 0 is not possible

    arm_scale_q31(waveform_inner_dtype, quotient, shift, waveform_inner_dtype,
                  WAVEFORM_LEN);
  }

  // transform
  // todo: possible optimization: only copy the overlapping part
  static q31_t scratchpad[STFT_FFT_SIZE + 2];
  static q31_t input_scratch[STFT_FFT_SIZE];
  for (size_t step = 0; step < MFCC_TIMESTEPS; ++step) {
    memcpy(input_scratch, &waveform_inner_dtype[step * STFT_STRIDE],
           sizeof(input_scratch));
    arm_mfcc_q31(&instance_q31, input_scratch,
                 &mfcc[step * MFCC_NUM_DCT_OUTPUTS], scratchpad);
  }
}

void preprocess_calculate_q15(int16_t waveform[], q15_t mfcc[]) {
  // this differs to tensorflow (absmax instead of max)
  q15_t max;
  arm_absmax_no_idx_q15(waveform, WAVEFORM_LEN, &max);
  if ((max != 0) && (max != 0x7FFF)) {
    q15_t quotient;
    int16_t shift;

    arm_status status = arm_divide_q15(0x7FFF, max, &quotient, &shift);
    assert(status == ARM_MATH_SUCCESS);  // as max == 0 is not possible

    arm_scale_q15(waveform, quotient, shift, waveform, WAVEFORM_LEN);
  }

  // transform
  // todo: possible optimization: only copy the overlapping part
  static q31_t scratchpad[STFT_FFT_SIZE * 2];
  static q15_t input_scratch[STFT_FFT_SIZE];
  for (size_t step = 0; step < MFCC_TIMESTEPS; ++step) {
    memcpy(input_scratch, &waveform[step * STFT_STRIDE], sizeof(input_scratch));
    arm_mfcc_q15(&instance_q15, input_scratch,
                 &mfcc[step * MFCC_NUM_DCT_OUTPUTS], scratchpad);
  }
}

void preprocess_quantize_mfcc_q31(q31_t* in, int8_t* out,
                                  int32_t mfcc_zero_point, float mfcc_scale) {
#if 0
  // todo only started this
  assert(mfcc_scale < 1);
  q31_t mfcc_scale_inner_dtype;
  arm_float_to_q31(&mfcc_scale, &mfcc_scale_inner_dtype, 1);

  q31_t quotient;
  int16_t shift;
  arm_status status = arm_divide_q31(0x7FFFFFFF, mfcc_scale_inner_dtype, &quotient, &shift);
  assert(status == ARM_MATH_SUCCESS);  // as 1 < mfcc_scale is not possible

  arm_scale_q31(mfcc_inner_dtype, quotient, shift, mfcc_inner_dtype, MFCC_TIMESTEPS * MFCC_NUM_DCT_OUTPUTS);

  // todo: add offset

  arm_q31_to_q7(mfcc_inner_dtype, mfcc, ARRAY_SIZE(mfcc_inner_dtype));
#endif
}

void preprocess_quantize_mfcc_f32(float32_t* in, int8_t* out,
                                  int32_t mfcc_zero_point, float mfcc_scale) {
  arm_scale_f32(in, 1.0F / mfcc_scale, in, MFCC_TOTAL_LENGTH);
  for (size_t i = 0; i < MFCC_TOTAL_LENGTH; ++i) {
    out[i] = (uint8_t)(in[i] + mfcc_zero_point);
  }
}

void preprocess_quantize_mfcc_f32_naive(float32_t* in, int8_t* out,
                                        int32_t mfcc_zero_point,
                                        float mfcc_scale) {
  for (size_t i = 0; i < MFCC_TOTAL_LENGTH; ++i) {
    out[i] = (int8_t)(in[i] / mfcc_scale + mfcc_zero_point);
  }
}

void preprocess_quantize_mfcc_q15(q15_t* in, int8_t* out,
                                  int32_t mfcc_zero_point, float mfcc_scale) {}
