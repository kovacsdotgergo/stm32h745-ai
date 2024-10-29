#ifndef AI_TASK_H
#define AI_TASK_H

#include <stdint.h>
#include <stddef.h>

void notify_ai_task_callback(int16_t* wave, size_t len);
void test_input_task(void *pvParameters);

#endif // AI_TASK_H