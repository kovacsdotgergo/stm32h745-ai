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

#include "hello_world_test.h"
#include "main.h"
#include "task.h"
#include "tim.h"
#include "usart.h"

// #include <stdio.h>

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
/* Definitions for defaultTask */

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

void StartDefaultTask(void *pvParamters);
void StartTfliteTask(void *pvParamters);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

static TaskHandle_t defaultTask = NULL;
static TaskHandle_t tfliteTask = NULL;
/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void) {
  // xTaskCreate(
  //     StartDefaultTask,             /* Function that implements the task. */
  //     "DefaultTaskM7Core",          /* Task name, for debugging only. */
  //     2 * configMINIMAL_STACK_SIZE, /* Size of stack (in words) to allocate for
  //                                      this task. */
  //     NULL,                         /* Task parameter, not used in this case. */
  //     tskIDLE_PRIORITY + 1,         /* Task priority. */
  //     &defaultTask); /* Task handle, used to unblock task from interrupt. */

  xTaskCreate(StartTfliteTask, "TfliteTask", 4 * 4096, NULL,
              tskIDLE_PRIORITY + 2, &tfliteTask);
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
    HAL_GPIO_TogglePin(LD_RED_GPIO, LD_RED_GPIO_PIN);
    uint32_t start = __HAL_TIM_GET_COUNTER(&htim2);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    uint32_t end = __HAL_TIM_GET_COUNTER(&htim2);
    printf("cm4: %f\r\n", (float)(end - start) / getTIM2Freq());
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

void StartTfliteTask(void *pvParameters) {
  while (1) {
    tflite_helloworld();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}