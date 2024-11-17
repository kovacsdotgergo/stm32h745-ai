#ifndef IPC_H
#define IPC_H

#include "FreeRTOS.h"
#include "ipc_config.h"
#include "message_buffer.h"

//shared message buffer
extern volatile MessageBufferHandle_t shared_data_mb;

void ipc_mb_init(void);

#endif // IPC_H