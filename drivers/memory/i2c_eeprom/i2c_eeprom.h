#ifndef DRIVERS_MEMORY_I2C_EEPROM_H
#define DRIVERS_MEMORY_I2C_EEPROM_H

#include "core/compiler.h"
#include "core/types.h"

/* Address width options for 24Cxx devices. */
#define I2C_EEPROM_ADDR_WIDTH_8   1u
#define I2C_EEPROM_ADDR_WIDTH_16  2u

typedef struct
{
    uint8_t device_address;
    uint16_t page_size;
    uint8_t address_width;
    uint8_t ready_retries;
    uint8_t write_cycle_ms;
} i2c_eeprom_config_t;

void i2c_eeprom_init(const i2c_eeprom_config_t* config);

uint8_t i2c_eeprom_is_ready(void);

uint8_t i2c_eeprom_write_byte(uint16_t address, uint8_t data);
uint8_t i2c_eeprom_read_byte(uint16_t address, uint8_t* data);

uint8_t i2c_eeprom_write_buffer(uint16_t address, const uint8_t* data, uint16_t length);
uint8_t i2c_eeprom_read_buffer(uint16_t address, uint8_t* data, uint16_t length);

#endif /* DRIVERS_MEMORY_I2C_EEPROM_H */