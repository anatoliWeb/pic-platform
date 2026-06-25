/*
 * File: examples-projects/xc8/ds18b20_read.X/main.c
 */

#include "project_config.h"

#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/communication/onewire/onewire.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/sensors/ds18b20/ds18b20.h"
#include "libraries/system/uart_debug/uart_debug.h"

/*
 * DS18B20 single sensor read example.
 *
 * MCU: PIC18F452 DIP-40
 * DQ:  RB1 / pin 34
 *
 * Flow:
 *   SKIP_ROM -> CONVERT_T -> SKIP_ROM -> READ_SCRATCHPAD
 *
 * Notes:
 *   - Use this example when only one DS18B20 is connected to the 1-Wire bus.
 *   - For multiple sensors, use SEARCH_ROM + MATCH_ROM example.
 */

#define DS18B20_DQ_PIN  1u

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

void main(void)
{
    int16_t temp_x10;

    uart_init(9600u);

    /*
     * DS18B20 DQ:
     * RB1 / PIC18F452 pin 34.
     */
    onewire_init(&PORTB, &TRISB, DS18B20_DQ_PIN);

    /*
     * Required for PIC18F452 + XC8 + Proteus + 10 MHz.
     */
    onewire_use_proteus_pic18f452_timing();

    DBG_PRINTLN("");
    DBG_PRINTLN("PIC18F452 DS18B20 read example");
    DBG_PRINTLN("DQ = RB1 / pin 34");
    DBG_PRINTLN("Flow: SKIP_ROM -> CONVERT_T -> SKIP_ROM -> READ_SCRATCHPAD");

    while (1)
    {
        if (ds18b20_read_temperature_celsius_skip_rom(&temp_x10) != 0u)
        {
            DBG_PRINT("TEMP=");
            print_temp_x10(temp_x10);
            DBG_PRINTLN(" C");
        }
        else
        {
            DBG_PRINTLN("DS18B20 read error");
        }

        delay_ms(1000u);
    }
}