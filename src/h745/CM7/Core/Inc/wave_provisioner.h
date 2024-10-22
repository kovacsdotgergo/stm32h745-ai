#ifndef WAVE_PROVISIONER_H
#define WAVE_PROVISIONER_H

#include <stdint.h>
#include <stddef.h>

extern volatile uint32_t errors;
extern volatile uint32_t print_errors;

typedef void (*wave_ready_callback)(int16_t *wave, size_t len);

void wave_set_wave_ready_callback(wave_ready_callback cb);
void wave_start_provisioning(void);

#endif // WAVE_PROVISIONER_H