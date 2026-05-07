#ifndef DRIVERS_EEPROM_EEPROM_H
#define DRIVERS_EEPROM_EEPROM_H

#include "core/compiler.h"
#include "core/types.h"

void eeprom_init(void);

uint8_t eeprom_read_byte(uint16_t address);
void eeprom_write_byte(uint16_t address, uint8_t value);

void eeprom_update_byte(uint16_t address, uint8_t value);

void eeprom_read_block(uint16_t address, uint8_t* buffer, uint16_t length);
void eeprom_write_block(uint16_t address, const uint8_t* data, uint16_t length);

#endif /* DRIVERS_EEPROM_EEPROM_H */
