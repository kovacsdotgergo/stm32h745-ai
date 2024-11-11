#include "wave_provisioner.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "benchmark.h"
#include "macros.h"
#include "usart.h"

#define WAVE_BUFFER_LEN WAVEFORM_LEN

#define BUFFER_BLOCK_NUM 4
static_assert(WAVE_BUFFER_LEN % BUFFER_BLOCK_NUM == 0);
#define BUFFER_BLOCK_LEN (WAVE_BUFFER_LEN / BUFFER_BLOCK_NUM)

static wave_ready_callback g_ready_callback = NULL;
static wave_pend_bottom_half_callback g_pend_callback = NULL;

// Implementaion option 0 requires a little bit less space and shorter
// processing after the handling of the waveform is done
#define IMPLEMENTATION_OPTION 0
#if IMPLEMENTATION_OPTION == 0
// Two buffers, both WAVE_BUFFER_LEN in total
// A | BUFFER_BLOCK_LEN | BUFFER_BLOCK_LEN | ... | BUFFER_BLOCK_LEN |
// B | BUFFER_BLOCK_LEN | BUFFER_BLOCK_LEN | ... | BUFFER_BLOCK_LEN |
static volatile int16_t wave_buffer_a[BUFFER_BLOCK_NUM][BUFFER_BLOCK_LEN]
    __attribute__((aligned(32)));  // invalidateDCache requirement
static volatile int16_t wave_buffer_b[BUFFER_BLOCK_NUM][BUFFER_BLOCK_LEN]
    __attribute__((aligned(32)));  // invalidateDCache requirement
static volatile int16_t (*volatile dma_buffer)[BUFFER_BLOCK_NUM]
                                              [BUFFER_BLOCK_LEN] =
                                                  &wave_buffer_a;
static volatile int16_t (*volatile working_buffer)[BUFFER_BLOCK_NUM]
                                                  [BUFFER_BLOCK_LEN] =
                                                      &wave_buffer_b;
#define BUFFER_BLOCK_SIZE (sizeof(wave_buffer_a[0]))

void wave_start_provisioning(void) {
  HAL_StatusTypeDef status = HAL_UART_Receive_DMA(
      &huart3, (uint8_t *)(*dma_buffer)[BUFFER_BLOCK_NUM - 1],
      BUFFER_BLOCK_SIZE);
  assert(status == HAL_OK);
}

// Always receive in the last block using DMA
// The newest block, just prepared by DMA has to be copied
// The blocks before this are already prepared during the previous cycle
//
// The numbers denote the timestamp of the blocks (example with 4 blocks)
// B |    4    |    5    |    x    |    x    | (x don't care)
// --------------------- begin a cycle ------------------------------------
// A |    3    |    4    |    5    |  6(DMA) | (newest block complete)
// copy newest block
// B |    4    |    5    |    6    |    x    | (x don't care)
// start DMA reception for last block in B
// process A (this changes the content of A)
// A |    x    |    x    |    x    |    x    | (x don't care)
// copy 5, 6 from B to A
// A |    5    |    6    |    x    |    x    | (x don't care)
// --------------------- next cycle ---------------------------------------
// B |    4    |    5    |    6    |  7(DMA) | (newest block complete)
// copy newest block
// A |    5    |    6    |    7    |    x    | (x don't care)
// start DMA reception for last block in A
// process B (this changes the content of B)
// B |    x    |    x    |    x    |    x    | (x don't care)
// copy 5, 6 from B to A
// B |    6    |    7    |    x    |    x    | (x don't care)
//
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  benchmark_set_point(IRQ_BEGIN);
  assert(huart == &huart3);  // only this is implemented
  static_assert(2 <= BUFFER_BLOCK_NUM);

  HAL_StatusTypeDef status = HAL_UART_Receive_DMA(
      &huart3, (uint8_t *)(*working_buffer)[BUFFER_BLOCK_NUM - 1],
      BUFFER_BLOCK_SIZE);
  assert(status == HAL_OK);

  benchmark_set_point(IRQ_PEND_BOTTOM_HALF);
  assert(g_pend_callback != NULL);
  g_pend_callback();
}

void wave_bottom_half(void) {
  benchmark_set_point(IRQ_BOTTOM_HALF_BEGIN);
  volatile int16_t(*volatile tmp)[BUFFER_BLOCK_NUM][BUFFER_BLOCK_LEN] =
      dma_buffer;
  dma_buffer = working_buffer;
  working_buffer = tmp;

  benchmark_set_point(IRQ_BEGIN_INVALIDATE);
  benchmark_set_point(IRQ_BEGIN_MEMCPY);
  memcpy((void *)(*dma_buffer)[BUFFER_BLOCK_NUM - 2],
         (void *)(*working_buffer)[BUFFER_BLOCK_NUM - 1], BUFFER_BLOCK_SIZE);

  benchmark_set_point(IRQ_BEGIN_CALLBACK);
  assert(g_ready_callback != NULL);
  g_ready_callback((volatile int16_t *)(*working_buffer)[0]);
}

// This perform the copy of the N-2 blocks after processing of the working
// buffer, prepares the next buffer
void wave_processing_done(void) {
  memcpy((void *)(*working_buffer)[0], (void *)(*dma_buffer)[1],
         (BUFFER_BLOCK_NUM - 2) * BUFFER_BLOCK_SIZE);
}

#elif IMPLEMENTATION_OPTION == 1
// Two buffers, WORKING is WAVE_BUFFER_LEN in total, DMA is one block longer
// DMA     | BUFFER_BLOCK_LEN | ... | BUFFER_BLOCK_LEN | BUFFER_BLOCK_LEN |
// WORKING | BUFFER_BLOCK_LEN | ... | BUFFER_BLOCK_LEN |
static volatile int16_t dma_buffer[BUFFER_BLOCK_NUM + 1][BUFFER_BLOCK_LEN]
    __attribute__((aligned(32)));  // invalidateDCache requirement
static volatile size_t dma_buffer_idx = 0;
static volatile int16_t working_buffer[BUFFER_BLOCK_NUM][BUFFER_BLOCK_LEN]
    __attribute__((aligned(32)));  // invalidateDCache requirement
#define BUFFER_BLOCK_SIZE (sizeof(dma_buffer[0]))

void wave_start_provisioning(void) {
  HAL_StatusTypeDef status = HAL_UART_Receive_DMA(
      &huart3, (uint8_t *)dma_buffer[dma_buffer_idx], BUFFER_BLOCK_SIZE);
  assert(status == HAL_OK);
}

// Receive in the dma_buffer as a circular buffer for blocks using DMA
// When data is ready, copy the wavefrom into the working_buffer
// The newest block, just prepared by DMA can't be copied earlier
// The blocks before can be prepared at the end of the previous cycle
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  benchmark_set_point(IRQ_BEGIN);
  assert(huart == &huart3);  // only this is implemented
  static_assert(2 <= BUFFER_BLOCK_NUM);

  size_t next_dma_buffer_idx = (dma_buffer_idx + 1) % ARRAY_LEN(dma_buffer);
  HAL_StatusTypeDef status = HAL_UART_Receive_DMA(
      &huart3, (uint8_t *)dma_buffer[next_dma_buffer_idx], BUFFER_BLOCK_SIZE);
  assert(status == HAL_OK);

  benchmark_set_point(IRQ_PEND_BOTTOM_HALF);
  assert(g_pend_callback != NULL);
  g_pend_callback();
}

void wave_bottom_half(void) {
  benchmark_set_point(IRQ_BOTTOM_HALF_BEGIN);

  benchmark_set_point(IRQ_BEGIN_INVALIDATE);
  SCB_InvalidateDCache_by_Addr((void *)dma_buffer[dma_buffer_idx],
                               BUFFER_BLOCK_SIZE);

  benchmark_set_point(IRQ_BEGIN_MEMCPY);
  memcpy((void *)working_buffer[BUFFER_BLOCK_NUM - 1],
         (void *)dma_buffer[dma_buffer_idx], BUFFER_BLOCK_SIZE);

  dma_buffer_idx = (dma_buffer_idx + 1) % ARRAY_LEN(dma_buffer);

  benchmark_set_point(IRQ_BEGIN_CALLBACK);
  assert(g_ready_callback != NULL);
  g_ready_callback((volatile int16_t *)working_buffer);
}

// This perform the copy of the N-1 blocks before the buffer received by DMA
// after processing of the working buffer, prepares the next buffer
void wave_processing_done(void) {
  size_t copy_blocks_num = BUFFER_BLOCK_NUM - 1;
  size_t dma_buffer_blocks_len = ARRAY_LEN(dma_buffer);
  size_t dma_beg_idx =
      ((dma_buffer_idx - 1) - (copy_blocks_num - 1) + dma_buffer_blocks_len) %
      dma_buffer_blocks_len;

  for (size_t i = 0; i < copy_blocks_num; ++i) {
    size_t dma_idx = (dma_beg_idx + i) % dma_buffer_blocks_len;
    memcpy((void *)working_buffer[i], (void *)dma_buffer[dma_idx],
           BUFFER_BLOCK_SIZE);
  }
}
#else
#error "Invalid IMPLEMENTATION_OPTION value"
#endif  // IMPLEMENTATION_OPTION

void wave_set_wave_ready_callback(wave_ready_callback cb) {
  g_ready_callback = cb;
}

void wave_set_pend_bottom_half_callback(wave_pend_bottom_half_callback cb) {
  g_pend_callback = cb;
}

void HAL_UARTEx_RxFifoFullCallback(UART_HandleTypeDef *huart) { while (1); }

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) { while (1); }