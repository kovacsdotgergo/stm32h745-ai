#include "wave_provisioner.h"

#include <assert.h>
#include <stdio.h>

#include "macros.h"
#include "usart.h"

static wave_ready_callback g_callback = NULL;
static int16_t wave[16000] __attribute__((aligned(32)));

volatile uint32_t errors = 0;
volatile uint32_t print_errors = 0;

void wave_set_wave_ready_callback(wave_ready_callback cb) { g_callback = cb; }

void wave_start_provisioning(void) {
  HAL_StatusTypeDef status =
      HAL_UART_Receive_DMA(&huart3, (uint8_t *)wave, sizeof(wave));
  assert(status == HAL_OK);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  assert(huart == &huart3);  // only this is implemented
  // todo start new reception immediately
  SCB_InvalidateDCache_by_Addr(wave, sizeof(wave));
  assert(0 < ARRAY_SIZE(wave));
  for (size_t i = 1; i < ARRAY_SIZE(wave); ++i) {
    if ((int16_t)(wave[i - 1] + 1) != wave[i]) ++errors;
  }
  print_errors = 1;

  HAL_StatusTypeDef status =
      HAL_UART_Receive_DMA(&huart3, (uint8_t *)wave, sizeof(wave));
  assert(status == HAL_OK);

  if (g_callback != NULL) {
    g_callback(wave, sizeof(wave));  // todo
  }
}

void HAL_UARTEx_RxFifoFullCallback(UART_HandleTypeDef *huart) {
  while(1)
    ;
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
  while (1)
    ;
}