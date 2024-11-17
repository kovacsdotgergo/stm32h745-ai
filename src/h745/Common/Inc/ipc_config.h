#ifndef IPC_CONFIG_H
#define IPC_CONFIG_H

#define MB2TO1_INT_EXTI_IRQ EXTI0_IRQn
#define MB2TO1_INT_EXTI_LINE EXTI_LINE0
#define MB2TO1_GPIO_PIN GPIO_PIN_0

#define DATA_MB_SIZE 2048
#define CONTROL_MB_SIZE 24
// -4 isn't enough in my experience
#define IPC_MAX_DATA_SIZE (DATA_MB_SIZE - 8)

#endif // IPC_CONFIG_H