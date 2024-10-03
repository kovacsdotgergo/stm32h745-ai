#ifndef PREPROCESS_MFCC_H
#define PREPROCESS_MFCC_H

#include <stdint.h>

#include "arm_math_types.h"

#define WAVEFORM_LEN 15872
#define MFCC_NUM_DCT_OUTPUTS 10
#define MFCC_TIMESTEPS 49

void preprocess_init(void);
/// @brief  Calculates the MFCC of the input with preset parameters
/// @param[in] waveform Input waveform of WAVEFORM_LEN length
/// @param[out] mfcc 2D output of shape (MFCC_TIMESTEPS, MFCC_NUM_DCT_OUTPUTS)
/// @note waveform is modified during the call
void preprocess_calculate_float(int16_t waveform[], float32_t mfcc[]);
void preprocess_calculate_uint16(int16_t waveform[], uint16_t mfcc[]);
void preprocess_calculate_uint32(int16_t waveform[], uint32_t mfcc[]);

#endif  // PREPROCESS_MFCC_H