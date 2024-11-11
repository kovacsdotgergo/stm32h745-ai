#include "ai_task.h"

#include <assert.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "benchmark.h"
#include "macros.h"
#include "nn_framework.h"
#include "timers.h"
// #include "postprocess.h"
// #include "preprocess_mfcc.h"
#include <stdint.h>

#include "semphr.h"
#include "wave_provisioner.h"
// #include "custom_sections.h"

#define IMPLEMENTATION_OPTION 3
SemaphoreHandle_t wave_ready_semaphore = NULL;
// static float32_t mfcc_f32[MFCC_TOTAL_LENGTH];
// static q31_t mfcc_q31[MFCC_TOTAL_LENGTH];
// static q15_t mfcc_q15[MFCC_TOTAL_LENGTH];
// static int8_t mfcc[MFCC_TOTAL_LENGTH];
static volatile int16_t *volatile waveform = NULL;

void notify_ai_task_callback(volatile int16_t *wave) {
  waveform = wave;
  // notfy task
  xSemaphoreGive(wave_ready_semaphore);
}

void pendable_wrapper_bottom_half(void *param1, uint32_t param2) {
  (void)param1, (void)param2;
  wave_bottom_half();
}

void pend_provisioning_bottom_half(void) {
  BaseType_t higher_prio_task_woken = pdFALSE;

  BaseType_t ret = xTimerPendFunctionCallFromISR(
      pendable_wrapper_bottom_half, NULL, 0, &higher_prio_task_woken);
  assert(ret == pdPASS);
  portYIELD_FROM_ISR(higher_prio_task_woken);
}

// void print_benchmark_results(void) {
//   float min, mean, max;
//   printf("\r\n");
//   printf("BENCHMARK (min, mean, max)\r\n");
//   benchmark_get_result_between_ms(IRQ_BEGIN, IRQ_PEND_BOTTOM_HALF, &min, &mean,
//                                   &max);
//   printf("[irq] actual interrupt level: %f, %f, %f\r\n", (double)min,
//          (double)mean, (double)max);
//   benchmark_get_result_between_ms(IRQ_PEND_BOTTOM_HALF, IRQ_BOTTOM_HALF_BEGIN,
//                                   &min, &mean, &max);
//   printf("[irq] pend to bottom half begin: %f, %f, %f\r\n", (double)min,
//          (double)mean, (double)max);
//   benchmark_get_result_between_ms(IRQ_BEGIN_INVALIDATE, IRQ_BEGIN_MEMCPY, &min,
//                                   &mean, &max);
//   printf("[irq] invalidate: %f, %f, %f\r\n", (double)min, (double)mean,
//          (double)max);
//   benchmark_get_result_between_ms(IRQ_BEGIN_MEMCPY, IRQ_BEGIN_CALLBACK, &min,
//                                   &mean, &max);
//   printf("[irq] memcpy: %f, %f, %f\r\n", (double)min, (double)mean,
//          (double)max);
//   benchmark_get_result_between_ms(IRQ_BEGIN_CALLBACK, TASK_BEGIN, &min, &mean,
//                                   &max);
//   printf("[irq] callback to task: %f, %f, %f\r\n", (double)min, (double)mean,
//          (double)max);
//   benchmark_get_result_between_ms(TASK_BEGIN, TASK_PREPROC_BEGIN, &min, &mean,
//                                   &max);
//   printf("[task] before preproc: %f, %f, %f\r\n", (double)min, (double)mean,
//          (double)max);
//   benchmark_get_result_between_ms(TASK_PREPROC_BEGIN, TASK_QUANTIZE_BEGIN, &min,
//                                   &mean, &max);
//   printf("[task] preproc: %f, %f, %f\r\n", (double)min, (double)mean,
//          (double)max);
//   benchmark_get_result_between_ms(TASK_QUANTIZE_BEGIN, TASK_RUN_BEGIN, &min,
//                                   &mean, &max);
//   printf("[task] quantize: %f, %f, %f\r\n", (double)min, (double)mean,
//          (double)max);
//   benchmark_get_result_between_ms(TASK_RUN_BEGIN, TASK_RUN_BEFORE_INVOKE, &min,
//                                   &mean, &max);
//   printf("[task][run] run before invoke: %f, %f, %f\r\n", (double)min,
//          (double)mean, (double)max);
//   benchmark_get_result_between_ms(TASK_RUN_BEFORE_INVOKE, TASK_RUN_AFTER_INVOKE,
//                                   &min, &mean, &max);
//   printf("[task][run] invoke: %f, %f, %f\r\n", (double)min, (double)mean,
//          (double)max);
//   benchmark_get_result_between_ms(TASK_RUN_AFTER_INVOKE, TASK_RUN_END, &min,
//                                   &mean, &max);
//   printf("[task][run] after invoke: %f, %f, %f\r\n", (double)min, (double)mean,
//          (double)max);
//   benchmark_get_result_between_ms(TASK_RUN_END, TASK_POSTPROC_BEGIN, &min,
//                                   &mean, &max);
//   printf("[task] print net output: %f, %f, %f\r\n", (double)min, (double)mean,
//          (double)max);
//   benchmark_get_result_between_ms(TASK_POSTPROC_BEGIN, TASK_LABLE_PRINT_BEGIN,
//                                   &min, &mean, &max);
//   printf("[task] postproc: %f, %f, %f\r\n", (double)min, (double)mean,
//          (double)max);
//   benchmark_get_result_between_ms(TASK_LABLE_PRINT_BEGIN,
//                                   TASK_WAVE_PROC_DONE_BEGIN, &min, &mean, &max);
//   printf("[task] label print: %f, %f, %f\r\n", (double)min, (double)mean,
//          (double)max);
//   benchmark_get_result_between_ms(TASK_WAVE_PROC_DONE_BEGIN, TASK_END, &min,
//                                   &mean, &max);
//   printf("[task] wave proc done: %f, %f, %f\r\n", (double)min, (double)mean,
//          (double)max);
//   benchmark_get_result_between_ms(TASK_BEGIN, TASK_END, &min, &mean, &max);
//   printf("[task] full runtime: %f, %f, %f\r\n", (double)min, (double)mean,
//          (double)max);
//   printf("MAX possible measruement: %f\r\n",
//          (double)benchmark_get_possible_max_ms());
// }

void ai_task(void *pvParameters) {
  wave_ready_semaphore = xSemaphoreCreateBinary();
  assert(wave_ready_semaphore != NULL);
  wave_set_wave_ready_callback(notify_ai_task_callback);
  wave_set_pend_bottom_half_callback(pend_provisioning_bottom_half);

  //   ai_model_init();
  //   int32_t input_zero_point;
  //   float input_scale;
  //   ai_get_input_quant_details(&input_scale, &input_zero_point);
  //   preprocess_init_f32();
  //   preprocess_init_q31();
  //   preprocess_init_q15();

  wave_start_provisioning();
  size_t cycle_count = 0;
  while (1) {
    // todo: add error handler if uart takes longer than the period
    if (xSemaphoreTake(wave_ready_semaphore, portMAX_DELAY) == pdTRUE) {
      printf("Received: %d, %d, %d, %d\r\n", waveform[0], waveform[1],
             waveform[2], waveform[3]);
      //       benchmark_set_point(TASK_BEGIN);
      //       // printf("First few: %d, %d, %d\r\n", waveform[0], waveform[1],
      //       // waveform[2]);

      //       benchmark_set_point(TASK_PREPROC_BEGIN);
      // #if IMPLEMENTATION_OPTION == 0
      //       preprocess_calculate_f32(waveform, mfcc_f32);
      //       benchmark_set_point(TASK_QUANTIZE_BEGIN);
      //       preprocess_quantize_mfcc_f32(mfcc_f32, mfcc, input_zero_point,
      //                                    input_scale);
      // #elif IMPLEMENTATION_OPTION == 1
      //       preprocess_calculate_f32(waveform, mfcc_f32);
      //       benchmark_set_point(TASK_QUANTIZE_BEGIN);
      //       preprocess_quantize_mfcc_f32_naive(mfcc_f32, mfcc,
      //       input_zero_point,
      //                                          input_scale);
      // #elif IMPLEMENTATION_OPTION == 2
      //       preprocess_calculate_q31(waveform, mfcc_q31);
      //       benchmark_set_point(TASK_QUANTIZE_BEGIN);
      //       preprocess_quantize_mfcc_q31_naive(mfcc_q31, mfcc,
      //       input_zero_point,
      //                                          input_scale);
      // #elif IMPLEMENTATION_OPTION == 3
      //       preprocess_calculate_q15(waveform, mfcc_q15);
      //       benchmark_set_point(TASK_QUANTIZE_BEGIN);
      //       preprocess_quantize_mfcc_q15_naive(mfcc_q15, mfcc,
      //       input_zero_point,
      //                                          input_scale);
      // #endif
      //       benchmark_set_point(TASK_RUN_BEGIN);
      //       float probabilities[POSTPROCESS_LABEL_NUM];
      //       ai_model_run(mfcc, probabilities);

      //       benchmark_set_point(TASK_RUN_END);
      //       // postprocess and logging
      //       printf("Net outputs:\r\n");
      //       for (size_t i = 0; i < ARRAY_LEN(probabilities); ++i) {
      //         printf("%.2f ", (double)probabilities[i]);
      //       }

      //       benchmark_set_point(TASK_POSTPROC_BEGIN);
      //       float argmax_max = probabilities[0];
      //       size_t argmax_idx = 0;
      //       for (size_t i = 1; i < ARRAY_LEN(probabilities); ++i) {
      //         if (argmax_max < probabilities[i]) {
      //           argmax_max = probabilities[i];
      //           argmax_idx = i;
      //         }
      //       }

      //       benchmark_set_point(TASK_LABLE_PRINT_BEGIN);
      //       assert(argmax_idx < POSTPROCESS_LABEL_NUM);
      //       printf("%s\r\n", postprocess_label_to_str[argmax_idx]);
      //       benchmark_set_point(TASK_WAVE_PROC_DONE_BEGIN);
      wave_processing_done();

      //       benchmark_set_point(TASK_END);
      //       ++cycle_count;
      //       if (cycle_count == BENCHMARK_MEAS_VECTOR_MAX_LEN) {
      //         print_benchmark_results();
      //         benchmark_reset_measurements();
      //         cycle_count = 0;
      //         // printf("\r\nTask watermark: %lu (words left)\r\n",
      //         //        uxTaskGetStackHighWaterMark(NULL));
      //       }
    }
  }
};
