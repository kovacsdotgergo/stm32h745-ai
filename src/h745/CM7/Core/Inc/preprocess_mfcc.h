#ifndef PREPROCESS_MFCC_H
#define PREPROCESS_MFCC_H

#include <stdint.h>

#include "arm_math_types.h"

#define WAVEFORM_LEN 15872
#define MFCC_NUM_DCT_OUTPUTS 10
#define MFCC_TIMESTEPS 49
#define MFCC_TOTAL_LENGTH ((MFCC_NUM_DCT_OUTPUTS) * (MFCC_TIMESTEPS))

void preprocess_init_f32(void);
void preprocess_init_q15(void);
void preprocess_init_q31(void);
/// @brief  Calculates the MFCC of the input with preset parameters
/// @param[in] waveform Input waveform of WAVEFORM_LEN length
/// @param[out] mfcc 2D output of shape (MFCC_TIMESTEPS, MFCC_NUM_DCT_OUTPUTS)
/// @note waveform is modified during the call
void preprocess_calculate_f32(int16_t waveform[], float32_t mfcc[]);
void preprocess_calculate_q15(int16_t waveform[], q15_t mfcc[]);
void preprocess_calculate_q31(int16_t waveform[], q31_t mfcc[]);

void preprocess_quantize_mfcc_f32(float32_t* in, int8_t* out,
                                  int32_t mfcc_zero_point, float mfcc_scale);
void preprocess_quantize_mfcc_f32_naive(float32_t* in, int8_t* out,
                                        int32_t mfcc_zero_point,
                                        float mfcc_scale);
void preprocess_quantize_mfcc_q15(q15_t* in, int8_t* out,
                                  int32_t mfcc_zero_point, float mfcc_scale);
void preprocess_quantize_mfcc_q31(q31_t* in, int8_t* out,
                                  int32_t mfcc_zero_point, float mfcc_scale);

#endif  // PREPROCESS_MFCC_H