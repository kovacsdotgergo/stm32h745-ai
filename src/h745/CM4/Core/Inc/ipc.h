#ifndef IPC_H
#define IPC_H

#include "ipc_config.h"
#include "FreeRTOS.h"
#include "message_buffer.h"

extern volatile MessageBufferHandle_t shared_data_mb;

void ipc_mb_init();

#endif // IPC_H