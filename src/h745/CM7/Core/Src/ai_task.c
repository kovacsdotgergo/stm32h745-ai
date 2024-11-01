#include "ai_task.h"

#include <assert.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "benchmark.h"
#include "nn_framework.h"
#include "preprocess_mfcc.h"
#include "semphr.h"
#include "wave_provisioner.h"

SemaphoreHandle_t wave_ready_semaphore = NULL;
static float32_t mfcc_f32[MFCC_TOTAL_LENGTH];
static int8_t mfcc[MFCC_TOTAL_LENGTH];
static volatile int16_t *volatile waveform = NULL;

void notify_ai_task_callback(volatile int16_t *wave) {
  waveform = wave;
  // notfy task
  BaseType_t higher_prio_task_woken = pdFALSE;
  xSemaphoreGiveFromISR(wave_ready_semaphore, &higher_prio_task_woken);
  if (higher_prio_task_woken != pdFALSE) {
    portYIELD_FROM_ISR(higher_prio_task_woken);
  }
}

void test_input_task(void *pvParameters) {
  wave_ready_semaphore = xSemaphoreCreateBinary();
  assert(wave_ready_semaphore != NULL);
  wave_set_wave_ready_callback(notify_ai_task_callback);

  ai_model_init();
  preprocess_init_f32();

  wave_start_provisioning();
  while (1) {
    // todo: add error handler if uart takes longer than the period
    if (xSemaphoreTake(wave_ready_semaphore, portMAX_DELAY) == pdTRUE) {
      // printf("First few: %d, %d, %d\r\n", waveform[0], waveform[1], waveform[2]);

      preprocess_calculate_f32(waveform, mfcc_f32);
      preprocess_quantize_mfcc_f32(mfcc_f32, mfcc, 83, 0.5847029089);
      ai_model_run(mfcc);
    }
  }
};

// void StartAiTask(void *pvParameters) {
//   ai_model_init();
//   preprocess_init_f32();
//   preprocess_init_q31();
//   preprocess_init_q15();
//   // todo: these are static to save stack space
//   static int16_t waveform[] = {
// #include "example_wave.h"
//   };
//   static float32_t mfcc_f32[MFCC_TOTAL_LENGTH];
//   static q31_t mfcc_q31[MFCC_TOTAL_LENGTH];
//   static q15_t mfcc_q15[MFCC_TOTAL_LENGTH];
//   static int8_t mfcc[MFCC_TOTAL_LENGTH];
//   while (1) {
//     // Wait before doing it again
//     //     printf("START OF TASK
//     //     =============================================="); printf("\r\nTask
//     //     watermark: %lu (words left)\r\n",
//     uxTaskGetStackHighWaterMark(NULL));

//     benchmark_set_point(BEGIN_PREPOC);
//     preprocess_calculate_f32(waveform, mfcc_f32);
//     benchmark_set_point(PREPROC_F32);
//     preprocess_calculate_q31(waveform, mfcc_q31);
//     benchmark_set_point(PREPROC_Q31);
//     preprocess_calculate_q15(waveform, mfcc_q15);
//     benchmark_set_point(PREPROC_Q15);

//     static float32_t copy[MFCC_TOTAL_LENGTH];
//     memcpy(copy, mfcc_f32, sizeof(copy));
//     benchmark_set_point(BEGIN_QUANTIZE);
//     preprocess_quantize_mfcc_f32(mfcc_f32, mfcc, 83, 0.5847029089);
//     benchmark_set_point(QUNATIZE);
//     preprocess_quantize_mfcc_f32_naive(copy, mfcc, 83, 0.5847029089);
//     benchmark_set_point(QUANTIZE_NAIVE);

//     benchmark_set_point(BEGIN_RUN);
//     ai_model_run(mfcc);
//     benchmark_set_point(END_RUN);
//     //     printf("\r\nTask watermark: %lu (words left)\r\n",
//     //            uxTaskGetStackHighWaterMark(NULL));

//     //     printf("\r\n");
//     //     printf("Calculate:\r\n");
//     //     printf("f32: %f\r\n",
//     //            (double)benchmark_get_result_between_ms(BEGIN_PREPOC,
//     //            PREPROC_F32));
//     //     printf("q31: %f\r\n",
//     //            (double)benchmark_get_result_between_ms(PREPROC_F32,
//     //            PREPROC_Q31));
//     //     printf("q15: %f\r\n",
//     //            (double)benchmark_get_result_between_ms(PREPROC_Q31,
//     //            PREPROC_Q15));
//     //     printf("Quantize:\r\n");
//     //     printf("using dsp: %f\r\n",
//     //            (double)benchmark_get_result_between_ms(BEGIN_QUANTIZE,
//     //            QUNATIZE));
//     //     printf("naive: %f\r\n",
//     //            (double)benchmark_get_result_between_ms(QUNATIZE,
//     //            QUANTIZE_NAIVE));
//     //     printf("Run:\r\n");
//     //     printf("load model: %f\r\n",
//     //            (double)benchmark_get_result_between_ms(BEGIN_RUN,
//     //            INSIDE_LOAD_MODEL));
//     //     printf("setup: %f\r\n", (double)benchmark_get_result_between_ms(
//     //                                 INSIDE_LOAD_MODEL, INSIDE_SETUP));
//     //     printf("junk prints and variables: %f\r\n",
//     //            (double)benchmark_get_result_between_ms(INSIDE_SETUP,
//     // INSIDE_BEFORE_INVOKE));
//     //     printf("invoke: %f\r\n", (double)benchmark_get_result_between_ms(
//     //                                  INSIDE_BEFORE_INVOKE,
//     //                                  INSIDE_AFTER_INVOKE));
//     //     printf("junk post print: %f\r\n",
//     // (double)benchmark_get_result_between_ms(INSIDE_AFTER_INVOKE,
//     //            END_RUN));
//     //     printf("Full runmodel call: %f\r\n",
//     //            (double)benchmark_get_result_between_ms(BEGIN_RUN,
//     END_RUN));
//     //     printf("MAX possible measruement: %f",
//     //            (double)benchmark_get_possible_max_ms());

//     //     printf("END OF TASK
//     //     ================================================");
//     vTaskDelay(10000 / portTICK_PERIOD_MS);
//     // while (1);
//   }
// }