#ifndef WAVE_PROVISIONER_H
#define WAVE_PROVISIONER_H

#include <stdint.h>
#include <stddef.h>

// todo circ_buffer_beg, circ_buffer_len, wave_beg_idx, wave_len
typedef void (*wave_ready_callback)(volatile int16_t *wave, size_t len);

// IMPORTANT: this callback is called from an ISR
void wave_set_wave_ready_callback(wave_ready_callback cb);
void wave_start_provisioning(void);

#endif // WAVE_PROVISIONER_H