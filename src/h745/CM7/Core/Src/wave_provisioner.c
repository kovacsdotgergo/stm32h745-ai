#include "wave_provisioner.h"

#include <assert.h>
#include <stdio.h>

#include "macros.h"
#include "usart.h"

#define WAVE_BUFFER_LEN WAVEFORM_LEN
#define WAVE_BUFFER_NUM 2

static wave_ready_callback g_callback = NULL;
static volatile int16_t wave_buffers[WAVE_BUFFER_NUM][WAVE_BUFFER_LEN]
    __attribute__((aligned(32)));
static size_t wave_buffer_idx = 0;

void wave_set_wave_ready_callback(wave_ready_callback cb) { g_callback = cb; }

void wave_start_provisioning(void) {
  HAL_StatusTypeDef status =
      HAL_UART_Receive_DMA(&huart3, (uint8_t *)wave_buffers[wave_buffer_idx],
                           sizeof(wave_buffers[wave_buffer_idx]));
  assert(status == HAL_OK);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  assert(huart == &huart3);  // only this is implemented

  size_t prev_buffer_idx = wave_buffer_idx;
  wave_buffer_idx = (wave_buffer_idx + 1) % WAVE_BUFFER_NUM;
  HAL_StatusTypeDef status =
      HAL_UART_Receive_DMA(&huart3, (uint8_t *)wave_buffers[wave_buffer_idx],
                           sizeof(wave_buffers[wave_buffer_idx]));
  assert(status == HAL_OK);
  // todo set up mpu
  SCB_InvalidateDCache_by_Addr((void *)wave_buffers[prev_buffer_idx],
                               sizeof(wave_buffers[prev_buffer_idx]));

  if (g_callback != NULL) {
    g_callback(wave_buffers[prev_buffer_idx]);
  }
}

void HAL_UARTEx_RxFifoFullCallback(UART_HandleTypeDef *huart) { while (1); }

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) { while (1); }