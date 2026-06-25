/*
 * File: drivers/communication/onewire/onewire.h
 */

#ifndef DRIVERS_COMMUNICATION_ONEWIRE_ONEWIRE_H
/* drivers/communication/onewire/onewire.h */
#define DRIVERS_COMMUNICATION_ONEWIRE_ONEWIRE_H

#include "core/compiler.h"
#include "core/types.h"

/*
 * Timing model:
 * - reset/presence/read fields are real microseconds.
 * - write fields keep the validated slot values for the current profile.
 *   For the Proteus PIC18F452 preset, write fields behave as raw tuning ticks,
 *   not as real microseconds.
 */
typedef struct
{
    uint16_t reset_low_us;
    uint16_t presence_sample_us;
    uint16_t reset_finish_us;

    uint16_t write_1_low_us;
    uint16_t write_1_release_us;

    uint16_t write_0_low_us;
    uint16_t write_0_release_us;

    uint16_t read_low_us;
    uint16_t read_sample_us;
    uint16_t read_finish_us;
} onewire_timing_t;

void onewire_init(volatile uint8_t* port, volatile uint8_t* tris, uint8_t pin);

void onewire_set_timing(const onewire_timing_t* timing);

void onewire_use_standard_timing(void);

void onewire_use_proteus_pic18f452_timing(void);

/* Legacy compatibility alias for the previous preset name. */
void onewire_use_proteus_pic18f452_8mhz_timing(void);

uint8_t onewire_reset(void);

void onewire_write_bit(uint8_t bit);
uint8_t onewire_read_bit(void);

void onewire_write_byte(uint8_t byte);
uint8_t onewire_read_byte(void);

void onewire_skip_rom(void);
void onewire_match_rom(uint8_t* rom);

void onewire_read_rom(uint8_t* rom);

uint8_t onewire_search_rom(uint8_t (*roms)[8], uint8_t max_devices);

uint8_t onewire_crc8(const uint8_t* data, uint8_t len);

#endif /* DRIVERS_COMMUNICATION_ONEWIRE_ONEWIRE_H */
