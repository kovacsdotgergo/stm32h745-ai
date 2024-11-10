#ifndef WAVE_PROVISIONER_H
#define WAVE_PROVISIONER_H

#include <stdint.h>
#include <stddef.h>

#include <mfcc_config/shapes_config.h>

/// @param[in] wave input waveform of shape WAVEFORM_LEN
typedef void (*wave_ready_callback)(volatile int16_t *wave);
typedef void (*wave_pend_bottom_half_callback)(void);

// This callback is called from the pended callback
void wave_set_wave_ready_callback(wave_ready_callback cb);
// IMPORTANT: this callback is called from an ISR
void wave_set_pend_bottom_half_callback(wave_pend_bottom_half_callback cb);

void wave_start_provisioning(void);
// this should be called after processing of the current buffer is done
void wave_bottom_half(void);
void wave_processing_done(void);

#endif // WAVE_PROVISIONER_H