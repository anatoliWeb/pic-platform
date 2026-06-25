/*
 * File: C18/drivers/memory/eeprom/eeprom.c
 */

#include "drivers/memory/eeprom/eeprom.h"

static void eeprom_write_raw(uint16_t address, uint8_t value)
{
    uint8_t gie_state = INTCONbits.GIE;

    EEADR = (uint8_t)(address & 0xFFu);
#ifdef EEADRH
    EEADRH = (uint8_t)((address >> 8u) & 0x03u);
#endif
    EEDATA = value;

    EECON1bits.EEPGD = 0u;
    EECON1bits.CFGS = 0u;
    EECON1bits.WREN = 1u;

    INTCONbits.GIE = 0u;
    EECON2 = 0x55u;
    EECON2 = 0xAAu;
    EECON1bits.WR = 1u;

    while (EECON1bits.WR != 0u)
    {
    }

    EECON1bits.WREN = 0u;
    INTCONbits.GIE = gie_state;
}

void eeprom_init(void)
{
    EECON1bits.EEPGD = 0u;
    EECON1bits.CFGS = 0u;
}

uint8_t eeprom_read_byte(uint16_t address)
{
    EEADR = (uint8_t)(address & 0xFFu);
#ifdef EEADRH
    EEADRH = (uint8_t)((address >> 8u) & 0x03u);
#endif

    EECON1bits.EEPGD = 0u;
    EECON1bits.CFGS = 0u;
    EECON1bits.RD = 1u;

    return EEDATA;
}

void eeprom_update_byte(uint16_t address, uint8_t value)
{
    if (eeprom_read_byte(address) != value)
    {
        eeprom_write_raw(address, value);
    }
}

void eeprom_write_byte(uint16_t address, uint8_t value)
{
    eeprom_update_byte(address, value);
}

void eeprom_read_block(uint16_t address, uint8_t* buffer, uint16_t length)
{
    uint16_t i;

    if (buffer == (uint8_t*)0)
    {
        return;
    }

    for (i = 0u; i < length; i++)
    {
        buffer[i] = eeprom_read_byte((uint16_t)(address + i));
    }
}

void eeprom_write_block(uint16_t address, const uint8_t* data, uint16_t length)
{
    uint16_t i;

    if (data == (const uint8_t*)0)
    {
        return;
    }

    for (i = 0u; i < length; i++)
    {
        eeprom_update_byte((uint16_t)(address + i), data[i]);
    }
}
