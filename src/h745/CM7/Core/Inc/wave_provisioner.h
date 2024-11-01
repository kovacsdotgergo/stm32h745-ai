#ifndef WAVE_PROVISIONER_H
#define WAVE_PROVISIONER_H

#include <stdint.h>
#include <stddef.h>

#include <mfcc_config/shapes_config.h>

// todo circ_buffer_beg, circ_buffer_len, wave_beg_idx, wave_len
/// @param[in] wave input waveform of shape WAVEFORM_LEN
typedef void (*wave_ready_callback)(volatile int16_t *wave);

// IMPORTANT: this callback is called from an ISR
void wave_set_wave_ready_callback(wave_ready_callback cb);
void wave_start_provisioning(void);

#endif // WAVE_PROVISIONER_H