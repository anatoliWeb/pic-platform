/*
 * File: examples-projects/xc8/ds18b20_multi_read.X/main.c
 */

#include "project_config.h"

#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/communication/onewire/onewire.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/sensors/ds18b20/ds18b20.h"
#include "libraries/system/uart_debug/uart_debug.h"

#define DS18B20_DQ_PIN        1u
#define DS18B20_MAX_SENSORS   4u

static void print_temp_x10(int16_t temp_x10)
{
    int16_t whole;
    int16_t frac;

    whole = (int16_t)(temp_x10 / 10);
    frac = (int16_t)(temp_x10 % 10);

    if (frac < 0)
    {
        frac = (int16_t)(-frac);
    }

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
        DBG_PRINT_HEX(rom[i]);

        if (i < 7u)
        {
            DBG_PRINT(":");
        }
    }
}

static void print_clock_info(void)
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

static void print_sensor_list(uint8_t roms[][8], uint8_t count)
{
    uint8_t i;

    DBG_PRINT("Found DS18B20 sensors: ");
    DBG_PRINT_INT((int)count);
    DBG_PRINTLN("");

    for (i = 0u; i < count; i++)
    {
        DBG_PRINT("ROM[");
        DBG_PRINT_INT((int)i);
        DBG_PRINT("] = ");
        print_rom(roms[i]);
        DBG_PRINTLN("");
    }
}

static void read_all_sensors(uint8_t roms[][8], uint8_t count)
{
    uint8_t i;
    int16_t temp_x10;

    if (count == 0u)
    {
        DBG_PRINTLN("No DS18B20 sensors found");
        return;
    }

    /*
     * Start temperature conversion on all sensors at once.
     * SKIP_ROM is valid here because every DS18B20 on the bus
     * must start conversion at the same time.
     */
    if (ds18b20_start_conversion_skip_rom() == 0u)
    {
        DBG_PRINTLN("CONVERT_T error");
        return;
    }

    /*
     * Use 1000 ms for Proteus-safe 12-bit conversion.
     */
    delay_ms(1000u);

    for (i = 0u; i < count; i++)
    {
        /*
         * Read each sensor by its unique ROM address.
         */
        if (ds18b20_read_temperature_celsius(roms[i], &temp_x10) != 0u)
        {
            DBG_PRINT("T[");
            DBG_PRINT_INT((int)i);
            DBG_PRINT("] ");
            print_rom(roms[i]);
            DBG_PRINT(" = ");
            print_temp_x10(temp_x10);
            DBG_PRINTLN(" C");
        }
        else
        {
            DBG_PRINT("T[");
            DBG_PRINT_INT((int)i);
            DBG_PRINT("] ");
            print_rom(roms[i]);
            DBG_PRINTLN(" read error");
        }
    }
}

void main(void)
{
    uint8_t roms[DS18B20_MAX_SENSORS][8];
    uint8_t count;

    uart_init(9600u);

    /*
     * DS18B20 DQ:
     * RB1 / PIC18F452 pin 34.
     */
    onewire_init(&PORTB, &TRISB, DS18B20_DQ_PIN);

    /*
     * Required for the validated PIC18F452 + XC8 + Proteus + 10 MHz setup.
     */
    onewire_use_proteus_pic18f452_timing();

    DBG_PRINTLN("");
    DBG_PRINTLN("PIC18F452 DS18B20 multi-sensor example");
    DBG_PRINTLN("DQ = RB1 / pin 34");
    DBG_PRINTLN("Flow: SEARCH_ROM -> SKIP_ROM CONVERT_T -> MATCH_ROM READ TEMPERATURE");
    print_clock_info();

    count = ds18b20_search(roms, DS18B20_MAX_SENSORS);
    print_sensor_list(roms, count);

    while (1)
    {
        DBG_PRINTLN("---");
        read_all_sensors(roms, count);
        delay_ms(1000u);
    }
}