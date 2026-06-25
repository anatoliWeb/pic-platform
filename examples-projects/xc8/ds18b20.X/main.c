/*
 * File: examples-projects/xc8/ds18b20.X/main.c
 */

#include "project_config.h"

#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/communication/uart/uart.h"
#include "drivers/communication/onewire/onewire.h"
#include "libraries/sensors/ds18b20/ds18b20.h"
#include "libraries/system/uart_debug/uart_debug.h"

#ifndef DS18B20_EXAMPLE_DEBUG
#define DS18B20_EXAMPLE_DEBUG 0u
#endif

#define DS18B20_PIN         1u
#define DS18B20_ROM_LENGTH  8u

static uint8_t g_ds18b20_rom[DS18B20_ROM_LENGTH];
static uint8_t g_ds18b20_has_rom = 0u;

static void app_print_temperature_x10(int16_t temp_x10)
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
    DBG_PRINTLN(" C");
}

#if defined(DS18B20_EXAMPLE_DEBUG) && (DS18B20_EXAMPLE_DEBUG != 0u)
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

    for (i = 0u; i < DS18B20_ROM_LENGTH; i++)
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
#endif

static void app_probe_sensor(void)
{
    if (ds18b20_find_first(g_ds18b20_rom) != 0u)
    {
        g_ds18b20_has_rom = 1u;
        DBG_PRINTLN("DS18B20 OK");

#if defined(DS18B20_EXAMPLE_DEBUG) && (DS18B20_EXAMPLE_DEBUG != 0u)
        DBG_PRINT("DS18B20 ROM: ");
        app_print_rom(g_ds18b20_rom);
        DBG_PRINTLN("");
        app_print_clock_info();
#endif
    }
    else
    {
        g_ds18b20_has_rom = 0u;
        DBG_PRINTLN("DS18B20 SEARCH FAILED");
    }
}

static void app_measure_once(void)
{
    int16_t temp_x10;

    if (g_ds18b20_has_rom != 0u)
    {
        if (ds18b20_read_temperature_celsius(g_ds18b20_rom, &temp_x10) == 0u)
        {
            DBG_PRINTLN("READ_FAILED");
            return;
        }
    }
    else
    {
        if (ds18b20_read_temperature_celsius_skip_rom(&temp_x10) == 0u)
        {
            DBG_PRINTLN("READ_FAILED");
            return;
        }
    }

    DBG_PRINT("TEMP=");
    app_print_temperature_x10(temp_x10);
}

void main(void)
{
    uart_init(9600u);

    /*
     * DS18B20 DQ:
     * RB1 / PIC18F452 pin 34.
     */
    onewire_init(&PORTB, &TRISB, DS18B20_PIN);

    /*
     * Validated Proteus/PIC18F452 timing preset:
     * reset: 580/50/480 us
     * write: 2/50 and 50/2 tuning ticks
     * read:  2/6/20 us
     */
    onewire_use_proteus_pic18f452_timing();

    DBG_PRINTLN("PIC18F452 DS18B20 library test");

#if defined(DS18B20_EXAMPLE_DEBUG) && (DS18B20_EXAMPLE_DEBUG != 0u)
    app_print_clock_info();
#endif

    app_probe_sensor();

    while (1)
    {
        app_measure_once();
        delay_ms(2000u);
    }
}
