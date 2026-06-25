/*
 * File: drivers/memory/i2c_eeprom/example.c
 */

#include "core/compiler.h"
#include "drivers/communication/i2c/i2c.h"
#include "drivers/memory/i2c_eeprom/i2c_eeprom.h"

void main(void)
{
    uint8_t value = 0u;
    uint8_t tx[4] = { 0x11u, 0x22u, 0x33u, 0x44u };
    uint8_t rx[4] = { 0u, 0u, 0u, 0u };
    i2c_eeprom_config_t cfg;

    i2c_init(100000u);

    cfg.device_address = 0x50u;
    cfg.page_size = 16u;
    cfg.address_width = I2C_EEPROM_ADDR_WIDTH_8;
    cfg.ready_retries = 40u;
    cfg.write_cycle_ms = 5u;
    i2c_eeprom_init(&cfg);

    (void)i2c_eeprom_write_byte(0x0000u, 0x5Au);
    (void)i2c_eeprom_read_byte(0x0000u, &value);

    (void)i2c_eeprom_write_buffer(0x0010u, tx, 4u);
    (void)i2c_eeprom_read_buffer(0x0010u, rx, 4u);

    while (1)
    {
    }
}