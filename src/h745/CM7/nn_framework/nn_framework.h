#ifndef NN_FRAMEWORK_H
#define NN_FRAMEWORK_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "stdint.h"

void ai_model_init();
void ai_get_input_quant_details(float* input_scale, int32_t* input_zero_point);
void ai_model_run(const int8_t* mfcc, float* probabilities);

#ifdef __cplusplus
}  // extern "C"
#endif // __cplusplus

#endif  // NN_FRAMEWORK_H