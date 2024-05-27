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
#include "task.h"

#include "main.h"
#include "gpio.h"
#include "usart.h"
#include "tim.h"

#include <stdio.h>

#include "ai_datatypes_defines.h"
#include "ai_platform.h"
#include "sine_model.h"
#include "sine_model_data.h"

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
void vApplicationMallocFailedHook( void ) {
  configASSERT( 0 );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    char *pcTaskName ) {
  (void)xTask; (void)pcTaskName;
  configASSERT( 0 );
}
/*-----------------------------------------------------------*/

/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t * pulIdleTaskStackSize )
{
/* If the buffers to be provided to the Idle task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

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

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *puxTimerTaskStackSize )
{
/* If the buffers to be provided to the Timer task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

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
void StartCubeAITask(void *pvParameters);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

static TaskHandle_t defaultTask = NULL;
static TaskHandle_t cubeAITask = NULL;

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  #if 1
  xTaskCreate( StartDefaultTask, 			/* Function that implements the task. */
				 "DefaultTaskM7Core", 				/* Task name, for debugging only. */
				 2*configMINIMAL_STACK_SIZE,  /* Size of stack (in words) to allocate for this task. */
				 NULL, 						/* Task parameter, not used in this case. */
				 tskIDLE_PRIORITY + 1, 			/* Task priority. */
				 &defaultTask );				/* Task handle, used to unblock task from interrupt. */
  #else
  xTaskCreate( StartCubeAITask, "CubeAITask", 2*configMINIMAL_STACK_SIZE,
               NULL, tskIDLE_PRIORITY + 2, &cubeAITask);
  #endif
  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  pvParameters: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *pvParameters)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    HAL_GPIO_TogglePin(LD_GREEN_GPIO, LD_GREEN_GPIO_PIN);
    uint32_t st = __HAL_TIM_GET_COUNTER(&htim2);
    vTaskDelay( 500 / portTICK_PERIOD_MS );
    uint32_t end = __HAL_TIM_GET_COUNTER(&htim2);
    printf("cm7: %f\r\n", (float)(end - st) / getTIM2Freq());
  }
  /* USER CODE END StartDefaultTask */
}

void StartCubeAITask(void *pvParameters)
{
  (void)pvParameters;
  /* USER CODE BEGIN StartDefaultTask */
  ai_error ai_err;
  ai_i32 nbatch;
  float y_val;

  // Chunk of memory used to hold intermediate values for neural network
  AI_ALIGNED(4) static ai_u8 activations[AI_SINE_MODEL_DATA_ACTIVATIONS_SIZE];

  // Buffers used to store input and output tensors
  AI_ALIGNED(4) static ai_i8 in_data[AI_SINE_MODEL_IN_1_SIZE_BYTES];
  AI_ALIGNED(4) static ai_i8 out_data[AI_SINE_MODEL_OUT_1_SIZE_BYTES];

  // Pointer to our model
  ai_handle sine_model = AI_HANDLE_NULL;

  // Initialize wrapper structs that hold pointers to data and info about the
  // data (tensor height, width, channels)
  ai_buffer* ai_input = AI_SINE_MODEL_IN;
  ai_buffer* ai_output = AI_SINE_MODEL_OUT;

  // Set pointers wrapper structs to our data buffers
  //ai_input[0].n_batches = 1;
  ai_input[0].data = AI_HANDLE_PTR(in_data);
  //ai_output[0].n_batches = 1;
  ai_output[0].data = AI_HANDLE_PTR(out_data);

  // Greetings!
  printf("\r\n\r\nSTM32 X-Cube-AI test\r\n");

  // Create instance of neural network
  ai_err = ai_sine_model_create(&sine_model, AI_SINE_MODEL_DATA_CONFIG);
  if (ai_err.type != AI_ERROR_NONE)
  {
    printf("Error: could not create NN instance\r\n");
    while(1);
  }

  // Set working memory and get weights/biases from model
  ai_network_params ai_params = {
    AI_SINE_MODEL_DATA_WEIGHTS(ai_sine_model_data_weights_get()),
    AI_SINE_MODEL_DATA_ACTIVATIONS(activations)
  };

  // Initialize neural network
  if (!ai_sine_model_init(sine_model, &ai_params))
  {
    printf("Error: could not initialize NN\r\n");
    while(1);
  }


  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // Fill input buffer (use test value)
    for (uint32_t i = 0; i < AI_SINE_MODEL_IN_1_SIZE; i++)
    {
      ((ai_float *)in_data)[i] = (ai_float)2.0f;
    }

    // Get current timestamp
    uint32_t timestamp = __HAL_TIM_GET_COUNTER(&htim2);

    // Perform inference
    nbatch = ai_sine_model_run(sine_model, &ai_input[0], &ai_output[0]);
    if (nbatch != 1) {
      printf("Error: could not run inference\r\n");
    }

    // Read output (predicted y) of neural network
    y_val = ((float *)out_data)[0];

    // Print output of neural network along with inference time (microseconds)
    uint32_t runtime = __HAL_TIM_GET_COUNTER(&htim2) - timestamp;
    printf("Output: %f | Duration: %f [us]\r\n",
                      y_val,
                      1000000.0F * (float)runtime / getTIM2Freq());

    // Wait before doing it again
    vTaskDelay( 500 / portTICK_PERIOD_MS );
  }

}
/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

