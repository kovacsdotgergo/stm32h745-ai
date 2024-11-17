#include "ipc.h"

#include <stdio.h>  // todo: only for debug

#include "custom_sections.h"
#include "stm32h7xx_hal.h"

// todo: all should be in shared sectoin
volatile MessageBufferHandle_t SHARED_BSS_VAR(control_mb);
volatile MessageBufferHandle_t SHARED_BSS_VAR(shared_data_mb);
volatile StaticStreamBuffer_t SHARED_BSS_VAR(control_sb);
volatile StaticStreamBuffer_t SHARED_BSS_VAR(data_sb);
volatile uint8_t SHARED_BSS_ARR(control_storage, CONTROL_MB_SIZE);
volatile uint8_t SHARED_BSS_ARR(data_storage, DATA_MB_SIZE);

void ipc_mb_init(void) {
  /* SW interrupt for message buffer */
  HAL_NVIC_SetPriority(MB2TO1_INT_EXTI_IRQ, 14U, 0U);
  HAL_NVIC_EnableIRQ(MB2TO1_INT_EXTI_IRQ);

  printf("[DEBUG] before mb init\r\n");
  while (control_mb == NULL);
  while (shared_data_mb == NULL);
  printf("[DEBUG] after mb init\r\n");
}

static void ipc_mb_irq_handler(void) {
  MessageBufferHandle_t updated_mb;
  BaseType_t higher_prio_task_woken = pdFALSE;

  /* control_mb contains the handle of the message buffer that
  contains data. */
  if (xMessageBufferReceiveFromISR(control_mb, &updated_mb, sizeof(updated_mb),
                                   &higher_prio_task_woken) ==
      sizeof(updated_mb)) {
    /* API function notifying any task waiting for the messagebuffer*/
    xMessageBufferSendCompletedFromISR(updated_mb, &higher_prio_task_woken);
  }
  /* Scheduling with normal FreeRTOS semantics */
  HAL_EXTI_D1_ClearFlag(MB2TO1_GPIO_PIN);
  portYIELD_FROM_ISR(higher_prio_task_woken);
}

// external linkage to overwrite weak handler
void EXTI0_IRQHandler(void) {
  if (__HAL_GPIO_EXTI_GET_IT(MB2TO1_GPIO_PIN) != 0x00U) {
    __HAL_GPIO_EXTI_CLEAR_IT(MB2TO1_GPIO_PIN);
    ipc_mb_irq_handler();
  }
}
