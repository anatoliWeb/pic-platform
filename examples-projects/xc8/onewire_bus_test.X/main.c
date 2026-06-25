/*
 * File: examples-projects/xc8/onewire_bus_test.X/main.c
 */

#include "project_config.h"

#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/communication/uart/uart.h"
#include "drivers/communication/onewire/onewire.h"
#include "libraries/system/uart_debug/uart_debug.h"

#define BUS_TEST_PIN        1u
#define BUS_TEST_ROM_LENGTH 8u
#define BUS_TEST_MAX_ROM    4u

static void app_print_hex_nibble(uint8_t value)
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

static void app_print_hex_byte(uint8_t value)
{
    DBG_PRINT("0x");
    app_print_hex_nibble((uint8_t)(value >> 4u));
    app_print_hex_nibble(value);
}

static void app_print_rom(const uint8_t* rom)
{
    uint8_t i;

    if (rom == (const uint8_t*)0)
    {
        return;
    }

    for (i = 0u; i < BUS_TEST_ROM_LENGTH; i++)
    {
        if (i != 0u)
        {
            DBG_PRINT(" ");
        }

        app_print_hex_byte(rom[i]);
    }
}

static void app_print_clock_info(void)
{
#ifdef PIC_PLATFORM_CLOCK_HZ
    DBG_PRINT("PIC_PLATFORM_CLOCK_HZ=");
    DBG_PRINT_INT((int)(PIC_PLATFORM_CLOCK_HZ / 1000000UL));
    DBG_PRINTLN(" MHz");
#endif

#ifdef _XTAL_FREQ
    DBG_PRINT("_XTAL_FREQ=");
    DBG_PRINT_INT((int)(_XTAL_FREQ / 1000000UL));
    DBG_PRINTLN(" MHz");
#endif

#ifdef DRV_XTAL_FREQ
    DBG_PRINT("DRV_XTAL_FREQ=");
    DBG_PRINT_INT((int)(DRV_XTAL_FREQ / 1000000UL));
    DBG_PRINTLN(" MHz");
#endif
}

static void app_probe_bus(void)
{
    DBG_PRINTLN("OneWire reset/presence check...");

    if (onewire_reset() != 0u)
    {
        DBG_PRINTLN("Presence detected");
    }
    else
    {
        DBG_PRINTLN("No device present");
    }
}

static void app_search_roms(void)
{
    uint8_t roms[BUS_TEST_MAX_ROM][BUS_TEST_ROM_LENGTH];
    uint8_t found;
    uint8_t i;
    uint8_t crc;

    DBG_PRINTLN("SEARCH_ROM scan...");
    found = onewire_search_rom(roms, BUS_TEST_MAX_ROM);
    DBG_PRINT("Found ");
    DBG_PRINT_INT((int)found);
    DBG_PRINTLN(" device(s)");

    for (i = 0u; i < found; i++)
    {
        DBG_PRINT("ROM[");
        DBG_PRINT_INT((int)i);
        DBG_PRINT("]= ");
        app_print_rom(roms[i]);
        DBG_PRINTLN("");

        crc = onewire_crc8(roms[i], 7u);
        DBG_PRINT("Family=0x");
        app_print_hex_byte(roms[i][0]);
        DBG_PRINT(" ROM_CRC=");
        if (crc == roms[i][7])
        {
            DBG_PRINTLN("OK");
        }
        else
        {
            DBG_PRINT("BAD expected=");
            app_print_hex_byte(roms[i][7]);
            DBG_PRINTLN("");
        }
    }
}

void main(void)
{
    uart_init(9600u);

    /*
     * OneWire DQ:
     * RB1 / PIC18F452 pin 34.
     */
    onewire_init(&PORTB, &TRISB, BUS_TEST_PIN);

    /*
     * Validated Proteus/PIC18F452 timing preset:
     * reset: 580/50/480 us
     * write: 2/50 and 50/2 tuning ticks
     * read:  2/6/20 us
     */
    onewire_use_proteus_pic18f452_timing();

    DBG_PRINTLN("");
    DBG_PRINTLN("PIC18F452 OneWire bus test");
    app_print_clock_info();
    app_probe_bus();
    app_search_roms();

    while (1)
    {
        delay_ms(2000u);
    }
}
