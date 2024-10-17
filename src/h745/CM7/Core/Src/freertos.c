/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"

#include <stdio.h>

#include "benchmark.h"
#include "gpio.h"
#include "main.h"
#include "nn_framework.h"
#include "preprocess_mfcc.h"
#include "task.h"
#include "tim.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void vApplicationMallocFailedHook(void) { configASSERT(0); }
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
  (void)xTask;
  (void)pcTaskName;
  configASSERT(0);
}
/*-----------------------------------------------------------*/

/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide
an implementation of vApplicationGetIdleTaskMemory() to provide the memory that
is used by the Idle task. */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize) {
  /* If the buffers to be provided to the Idle task are declared inside this
  function then they must be declared static - otherwise they will be allocated
  on the stack and so not exists after this function exits. */
  static StaticTask_t xIdleTaskTCB;
  static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

  /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
  state will be stored. */
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

  /* Pass out the array that will be used as the Idle task's stack. */
  *ppxIdleTaskStackBuffer = uxIdleTaskStack;

  /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
  Note that, as the array is necessarily of type StackType_t,
  configMINIMAL_STACK_SIZE is specified in words, not bytes. */
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so
the application must provide an implementation of
vApplicationGetTimerTaskMemory() to provide the memory that is used by the Timer
service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *puxTimerTaskStackSize) {
  /* If the buffers to be provided to the Timer task are declared inside this
  function then they must be declared static - otherwise they will be allocated
  on the stack and so not exists after this function exits. */
  static StaticTask_t xTimerTaskTCB;
  static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

  /* Pass out a pointer to the StaticTask_t structure in which the Timer
  task's state will be stored. */
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

  /* Pass out the array that will be used as the Timer task's stack. */
  *ppxTimerTaskStackBuffer = uxTimerTaskStack;

  /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
  Note that, as the array is necessarily of type StackType_t,
  configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
  *puxTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *pvParameters);
void StartAiTask(void *pvParameters);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

// static TaskHandle_t defaultTask = NULL;
static TaskHandle_t aiTask = NULL;

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void) {
  // xTaskCreate(
  //     StartDefaultTask,             /* Function that implements the task. */
  //     "DefaultTaskM7Core",          /* Task name, for debugging only. */
  //     2 * configMINIMAL_STACK_SIZE, /* Size of stack (in words) to allocate
  //     for
  //                                      this task. */
  //     NULL,                         /* Task parameter, not used in this case.
  //     */ tskIDLE_PRIORITY + 1,         /* Task priority. */ &defaultTask); /*
  //     Task handle, used to unblock task from interrupt. */
  size_t stack_size_words = 8096 + 4096;
  xTaskCreate(
      StartAiTask, "AiTask", stack_size_words,
      NULL,  // mallocks the required amount in words (stack_type_t? is 4 bytes)
      tskIDLE_PRIORITY + 2, &aiTask);
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  pvParameters: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *pvParameters) {
  (void)pvParameters;
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for (;;) {
    HAL_GPIO_TogglePin(LD_GREEN_GPIO, LD_GREEN_GPIO_PIN);
    uint32_t st = __HAL_TIM_GET_COUNTER(&htim2);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    uint32_t end = __HAL_TIM_GET_COUNTER(&htim2);
    printf("cm7: %f\r\n", (float)(end - st) / getTIM2Freq());
  }
  /* USER CODE END StartDefaultTask */
}

void StartAiTask(void *pvParameters) {
  ai_model_init();
  preprocess_init_f32();
  preprocess_init_q31();
  preprocess_init_q15();
  // todo: these are static to save stack space
  static int16_t waveform[] = {
#include "example_wave.h"
  };
  static float32_t mfcc_f32[MFCC_TOTAL_LENGTH];
  static q31_t mfcc_q31[MFCC_TOTAL_LENGTH];
  static q15_t mfcc_q15[MFCC_TOTAL_LENGTH];
  static int8_t mfcc[MFCC_TOTAL_LENGTH];
  while (1) {
    // Wait before doing it again
    printf("START OF TASK ==============================================");
    printf("\r\nTask watermark: %lu (words left)\r\n",
           uxTaskGetStackHighWaterMark(NULL));

    benchmark_set_point(BEGIN_PREPOC);
    preprocess_calculate_f32(waveform, mfcc_f32);
    benchmark_set_point(PREPROC_F32);
    preprocess_calculate_q31(waveform, mfcc_q31);
    benchmark_set_point(PREPROC_Q31);
    preprocess_calculate_q15(waveform, mfcc_q15);
    benchmark_set_point(PREPROC_Q15);

    static float32_t copy[MFCC_TOTAL_LENGTH];
    memcpy(copy, mfcc_f32, sizeof(copy));
    benchmark_set_point(BEGIN_QUANTIZE);
    preprocess_quantize_mfcc_f32(mfcc_f32, mfcc, 83, 0.5847029089);
    benchmark_set_point(QUNATIZE);
    preprocess_quantize_mfcc_f32_naive(copy, mfcc, 83, 0.5847029089);
    benchmark_set_point(QUANTIZE_NAIVE);

    benchmark_set_point(BEGIN_RUN);
    ai_model_run(mfcc);
    benchmark_set_point(END_RUN);
    printf("\r\nTask watermark: %lu (words left)\r\n",
           uxTaskGetStackHighWaterMark(NULL));

    printf("\r\n");
    printf("Calculate:\r\n");
    printf("f32: %f\r\n",
           (double)benchmark_get_result_between_ms(BEGIN_PREPOC, PREPROC_F32));
    printf("q31: %f\r\n",
           (double)benchmark_get_result_between_ms(PREPROC_F32, PREPROC_Q31));
    printf("q15: %f\r\n",
           (double)benchmark_get_result_between_ms(PREPROC_Q31, PREPROC_Q15));
    printf("Quantize:\r\n");
    printf("using dsp: %f\r\n",
           (double)benchmark_get_result_between_ms(BEGIN_QUANTIZE, QUNATIZE));
    printf("naive: %f\r\n",
           (double)benchmark_get_result_between_ms(QUNATIZE, QUANTIZE_NAIVE));
    printf("Run:\r\n");
    printf("load model: %f\r\n",
           (double)benchmark_get_result_between_ms(BEGIN_RUN, INSIDE_LOAD_MODEL));
    printf("setup: %f\r\n", (double)benchmark_get_result_between_ms(
                                INSIDE_LOAD_MODEL, INSIDE_SETUP));
    printf("junk prints and variables: %f\r\n",
           (double)benchmark_get_result_between_ms(INSIDE_SETUP,
                                                INSIDE_BEFORE_INVOKE));
    printf("invoke: %f\r\n", (double)benchmark_get_result_between_ms(
                                 INSIDE_BEFORE_INVOKE, INSIDE_AFTER_INVOKE));
    printf("junk post print: %f\r\n",
           (double)benchmark_get_result_between_ms(INSIDE_AFTER_INVOKE, END_RUN));
    printf("Full runmodel call: %f\r\n",
           (double)benchmark_get_result_between_ms(BEGIN_RUN, END_RUN));
    printf("MAX possible measruement: %f",
           (double)benchmark_get_possible_max_ms());

    printf("END OF TASK ================================================");
    vTaskDelay(10000 / portTICK_PERIOD_MS);
    // while (1);
  }
}
/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
