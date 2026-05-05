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

#endif /* DRIVERS_ONEWIRE_ONEWIRE_H */
