#include "ai_task.h"
#include <assert.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "wave_provisioner.h"
#include <stdio.h>

SemaphoreHandle_t xSemaphore = NULL;

void notify_ai_task_callback(int16_t* wave, size_t len) {
  // notfy task
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);
  if (xHigherPriorityTaskWoken != pdFALSE) {
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

void test_input_task(void *pvParameters) {
  xSemaphore = xSemaphoreCreateBinary();
  assert(xSemaphore != NULL);
  wave_set_wave_ready_callback(notify_ai_task_callback);
  wave_start_provisioning();
  while (1) {
    // todo: add error handler if uart takes longer than the period
    if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
      printf("Errors: %ld\r\n", errors);
      errors = 0;
    }
  }
};
