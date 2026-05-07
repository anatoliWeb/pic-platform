#ifndef DRIVERS_ONEWIRE_ONEWIRE_H
#define DRIVERS_ONEWIRE_ONEWIRE_H

#include "core/compiler.h"
#include "core/types.h"

void onewire_init(
    volatile uint8_t* port,
    volatile uint8_t* tris,
    uint8_t pin
);

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

#endif /* DRIVERS_ONEWIRE_ONEWIRE_H */
