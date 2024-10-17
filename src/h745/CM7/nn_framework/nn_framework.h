#ifndef NN_FRAMEWORK_H
#define NN_FRAMEWORK_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "stdint.h"

void ai_model_init();
void ai_model_run(const int8_t* wave);

#ifdef __cplusplus
}  // extern "C"
#endif // __cplusplus

#endif  // NN_FRAMEWORK_H