#include "ai_task.h"

#include <assert.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "benchmark.h"
#include "custom_sections.h"
#include "ipc.h"
#include "macros.h"
#include "nn_framework.h"
#include "postprocess.h"
#include "mfcc_config/shapes_config.h"
#include "timers.h"

// using a multiple of 4 byte to have aligned messages in message buffer used for sending
static int8_t mfcc[BUFFER_SIZE_4_BYTE_CEIL(MFCC_TOTAL_LENGTH)];

void print_benchmark_results(void) {
  float min, mean, max;
  printf("\r\n");
  printf("BENCHMARK (min, mean, max)\r\n");
  benchmark_get_result_between_ms(TASK_BEGIN, TASK_RUN_BEGIN, &min, &mean,
                                  &max);
  printf("[task] waiting and ipc: %f, %f, %f\r\n", (double)min, (double)mean,
         (double)max);
  benchmark_get_result_between_ms(TASK_RUN_BEGIN, TASK_RUN_BEFORE_INVOKE, &min,
                                  &mean, &max);
  printf("[task][run] run before invoke: %f, %f, %f\r\n", (double)min,
         (double)mean, (double)max);
  benchmark_get_result_between_ms(TASK_RUN_BEFORE_INVOKE, TASK_RUN_AFTER_INVOKE,
                                  &min, &mean, &max);
  printf("[task][run] invoke: %f, %f, %f\r\n", (double)min, (double)mean,
         (double)max);
  benchmark_get_result_between_ms(TASK_RUN_AFTER_INVOKE, TASK_RUN_END, &min,
                                  &mean, &max);
  printf("[task][run] after invoke: %f, %f, %f\r\n", (double)min, (double)mean,
         (double)max);
  benchmark_get_result_between_ms(TASK_RUN_END, TASK_POSTPROC_BEGIN, &min,
                                  &mean, &max);
  printf("[task] print net output: %f, %f, %f\r\n", (double)min, (double)mean,
         (double)max);
  benchmark_get_result_between_ms(TASK_POSTPROC_BEGIN, TASK_LABLE_PRINT_BEGIN,
                                  &min, &mean, &max);
  printf("[task] postproc: %f, %f, %f\r\n", (double)min, (double)mean,
         (double)max);
  benchmark_get_result_between_ms(TASK_LABLE_PRINT_BEGIN,
                                  TASK_WAVE_PROC_DONE_BEGIN, &min, &mean, &max);
  printf("[task] label print: %f, %f, %f\r\n", (double)min, (double)mean,
         (double)max);
  benchmark_get_result_between_ms(TASK_WAVE_PROC_DONE_BEGIN, TASK_END, &min,
                                  &mean, &max);
  printf("[task] wave proc done: %f, %f, %f\r\n", (double)min, (double)mean,
         (double)max);
  benchmark_get_result_between_ms(TASK_BEGIN, TASK_END, &min, &mean, &max);
  printf("[task] full runtime: %f, %f, %f\r\n", (double)min, (double)mean,
         (double)max);
  printf("MAX possible measruement: %f\r\n",
         (double)benchmark_get_possible_max_ms());
}

void test_input_task(void *pvParameters) {
  ai_model_init();

  size_t cycle_count = 0;
  while (1) {
    benchmark_set_point(TASK_BEGIN);
    printf("[DEBUG] start recv\r\n");
    printf("[DEUBG] input params: %p, %p, %x\r\n", shared_data_mb, mfcc, sizeof(mfcc));
    xMessageBufferReceive(shared_data_mb, mfcc, sizeof(mfcc), portMAX_DELAY);

    float probabilities[POSTPROCESS_LABEL_NUM];
    benchmark_set_point(TASK_RUN_BEGIN);
    ai_model_run(mfcc, probabilities);

    benchmark_set_point(TASK_RUN_END);
    // postprocess and logging
    printf("Net outputs:\r\n");
    for (size_t i = 0; i < ARRAY_LEN(probabilities); ++i) {
      printf("%.2f ", (double)probabilities[i]);
    }

    benchmark_set_point(TASK_POSTPROC_BEGIN);
    float argmax_max = probabilities[0];
    size_t argmax_idx = 0;
    for (size_t i = 1; i < ARRAY_LEN(probabilities); ++i) {
      if (argmax_max < probabilities[i]) {
        argmax_max = probabilities[i];
        argmax_idx = i;
      }
    }

    benchmark_set_point(TASK_LABLE_PRINT_BEGIN);
    assert(argmax_idx < POSTPROCESS_LABEL_NUM);
    printf("%s\r\n", postprocess_label_to_str[argmax_idx]);
    benchmark_set_point(TASK_WAVE_PROC_DONE_BEGIN);
    benchmark_set_point(TASK_END);
    ++cycle_count;
    if (cycle_count == BENCHMARK_MEAS_VECTOR_MAX_LEN) {
      print_benchmark_results();
      benchmark_reset_measurements();
      cycle_count = 0;
      // printf("\r\nTask watermark: %lu (words left)\r\n",
      //        uxTaskGetStackHighWaterMark(NULL));
    }
  }
};
