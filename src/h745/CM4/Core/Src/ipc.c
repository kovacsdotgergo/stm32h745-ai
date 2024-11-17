#include "ipc.h"

#include <assert.h>

#include "custom_sections.h"
#include "stm32h7xx_hal.h"

// todo: all should be in shared sectoin
volatile MessageBufferHandle_t SHARED_BSS_VAR(control_mb);
volatile MessageBufferHandle_t SHARED_BSS_VAR(shared_data_mb);
volatile StaticStreamBuffer_t SHARED_BSS_VAR(control_sb);
volatile StaticStreamBuffer_t SHARED_BSS_VAR(data_sb);
volatile uint8_t SHARED_BSS_ARR(control_storage, CONTROL_MB_SIZE);
volatile uint8_t SHARED_BSS_ARR(data_storage, DATA_MB_SIZE);

void generate_it(void) {
  HAL_EXTI_D2_EventInputConfig(MB2TO1_INT_EXTI_LINE, EXTI_MODE_IT, DISABLE);
  HAL_EXTI_D1_EventInputConfig(MB2TO1_INT_EXTI_LINE, EXTI_MODE_IT, ENABLE);
  HAL_EXTI_GenerateSWInterrupt(MB2TO1_INT_EXTI_LINE);
}

void ipc_mb_generate_interrupt(MessageBufferHandle_t updated_mb,
                               BaseType_t is_inside_isr,
                               BaseType_t* const higher_prio_task_woken) {
  (void)is_inside_isr;
  (void)higher_prio_task_woken;
  if (updated_mb != control_mb) {
    /* Use control_mb to pass the handle of the message buffer
    written to by core 1 to the interrupt handler about to be generated in
    core 2. */
    const TickType_t dont_block = 0;
    xMessageBufferSend(control_mb, &updated_mb, sizeof(updated_mb), dont_block);

    /* This is where the interrupt would be generated. */
    generate_it();
  }
}

void ipc_mb_init(void) {
  /* AIEC Common configuration: make CPU1 and CPU2 SWI line0
  sensitive to rising edge : Configured only once */
  HAL_EXTI_EdgeConfig(MB2TO1_INT_EXTI_LINE, EXTI_RISING_EDGE);

  // Creating message buffer in a shared location
  shared_data_mb = xMessageBufferCreateStaticWithCallback(
      DATA_MB_SIZE, (uint8_t *)data_storage, (StaticStreamBuffer_t *)&data_sb,
      ipc_mb_generate_interrupt, NULL);
  assert(shared_data_mb != NULL);
  control_mb =
      xMessageBufferCreateStatic(CONTROL_MB_SIZE, (uint8_t *)control_storage,
                                 (StaticStreamBuffer_t *)&control_sb);
  assert(control_mb != NULL);
}
