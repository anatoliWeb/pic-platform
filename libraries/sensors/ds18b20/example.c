/*
 * File: libraries/sensors/ds18b20/example.c
 */

#include "core/compiler.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"
#include "drivers/communication/onewire/onewire.h"
#include "libraries/sensors/ds18b20/ds18b20.h"

static void print_hex_nibble(uint8_t value)
{
    value &= 0x0Fu;

    if (value < 10u)
    {
        DBG_PRINT_INT((int)value);
    }
    else
    {
        switch (value)
        {
            case 10u: DBG_PRINT("A"); break;
            case 11u: DBG_PRINT("B"); break;
            case 12u: DBG_PRINT("C"); break;
            case 13u: DBG_PRINT("D"); break;
            case 14u: DBG_PRINT("E"); break;
            default:  DBG_PRINT("F"); break;
        }
    }
}

static void print_hex_byte(uint8_t value)
{
    DBG_PRINT("0x");
    print_hex_nibble((uint8_t)(value >> 4u));
    print_hex_nibble(value);
}

static void print_temp_x10(int16_t temp_x10)
{
    int16_t whole;
    int16_t frac;

    if (temp_x10 < 0)
    {
        DBG_PRINT("-");
        temp_x10 = (int16_t)(-temp_x10);
    }

    whole = (int16_t)(temp_x10 / 10);
    frac = (int16_t)(temp_x10 % 10);

    DBG_PRINT_INT((int)whole);
    DBG_PRINT(".");
    DBG_PRINT_INT((int)frac);
}

static void print_rom(const uint8_t* rom)
{
    uint8_t i;

    if (rom == (const uint8_t*)0)
    {
        return;
    }

    for (i = 0u; i < 8u; i++)
    {
        if (i != 0u)
        {
            DBG_PRINT(" ");
        }

        print_hex_byte(rom[i]);
    }
}

void main(void)
{
    int16_t temp_x10;
    uint8_t rom[8];

    uart_init(9600u);
    onewire_init(&PORTB, &TRISB, 1u);
    onewire_use_proteus_pic18f452_timing();

    if (ds18b20_find_first(rom) != 0u)
    {
        DBG_PRINT("DS18B20 ROM: ");
        print_rom(rom);
        DBG_PRINT("\r\n");

        if (ds18b20_read_temperature_celsius(rom, &temp_x10) != 0u)
        {
            DBG_PRINT("Temperature: ");
            print_temp_x10(temp_x10);
            DBG_PRINT(" C\r\n");
        }
        else
        {
            DBG_PRINT("Temperature read error\r\n");
        }
    }
    else
    {
        DBG_PRINT("DS18B20 ROM not found, trying SKIP_ROM mode\r\n");

        if (ds18b20_read_temperature_celsius_skip_rom(&temp_x10) != 0u)
        {
            DBG_PRINT("Temperature: ");
            print_temp_x10(temp_x10);
            DBG_PRINT(" C\r\n");
        }
        else
        {
            DBG_PRINT("Temperature read error\r\n");
        }
    }

    while (1)
    {
    }
}
