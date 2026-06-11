#include "project_config.h"
#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/communication/onewire/onewire.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/sensors/ds18b20/ds18b20.h"
#include "libraries/system/uart_debug/uart_debug.h"

#if defined(DRV_COMPILER_C18)
#include <p18f452.h>
#endif

/*
 * Example: DS18B20 multi-sensor read on one 1-Wire bus
 * MCU: PIC18F452 DIP-40
 * Compiler: XC8 or C18
 * Oscillator: external HS crystal
 * Purpose:
 *   Demonstrate how multiple DS18B20 sensors can share one 1-Wire data line.
 * Hardware notes:
 *   - All DS18B20 DQ pins share the same 1-Wire data line.
 *   - Use one pull-up resistor on the data line, typically 4.7k to VDD.
 *   - Each sensor has a unique 64-bit ROM address.
 * Proteus notes:
 *   - Add multiple DS18B20 devices on the same DQ net.
 *   - Verify ROM addressing or bus scan support before relying on readings.
 * Status:
 *   - Project skeleton prepared.
 *   - Build/hardware/Proteus validation may still be required.
 */

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

    for (i = 0u; i < 8u; i++)
    {
        DBG_PRINT_HEX(rom[i]);
        if (i < 7u)
        {
            DBG_PRINT(":");
        }
    }
}

void main(void)
{
    uint8_t roms[4][8];
    uint8_t scratchpad[9];
    uint8_t count;
    uint8_t i;
    int16_t temp_raw;
    int16_t temp_x10;

    uart_init(9600u);
    onewire_init(&PORTB, &TRISB, 1u);

    DBG_PRINTLN("DS18B20 multi-sensor example ready");

    /*
     * TODO: onewire_search_rom() is currently a placeholder in the driver.
     * When real ROM search is available, this example will enumerate devices
     * and read each sensor by its unique 64-bit ROM address.
     */
    if (onewire_reset() != 0u)
    {
        count = onewire_search_rom(roms, 4u);
    }
    else
    {
        count = 0u;
    }

    DBG_PRINT("Found ROMs: ");
    DBG_PRINT_INT((int)count);
    DBG_PRINTLN("");

    while (1)
    {
        if (count != 0u)
        {
            if (onewire_reset() != 0u)
            {
                /*
                 * Start a conversion for every sensor on the shared bus.
                 * This uses Skip ROM because the bus search is still a stub.
                 */
                onewire_skip_rom();
                onewire_write_byte(0x44u);
                delay_ms(750u);

                for (i = 0u; i < count; i++)
                {
                    DBG_PRINT("ROM[");
                    DBG_PRINT_INT((int)i);
                    DBG_PRINT("] = ");
                    print_rom(roms[i]);
                    DBG_PRINTLN("");

                    if (ds18b20_read_scratchpad(roms[i], scratchpad) != 0u)
                    {
                        temp_raw = (int16_t)(((uint16_t)scratchpad[1] << 8u) | scratchpad[0]);
                        temp_x10 = (int16_t)((temp_raw * 10) / 16);

                        DBG_PRINT("ROM[");
                        DBG_PRINT_INT((int)i);
                        DBG_PRINT("] T(C): ");
                        print_temp_x10(temp_x10);
                        DBG_PRINTLN("");
                    }
                    else
                    {
                        DBG_PRINT("ROM[");
                        DBG_PRINT_INT((int)i);
                        DBG_PRINTLN("] read error");
                    }
                }
            }
        }
        else
        {
            DBG_PRINTLN("TODO: Multi-sensor support needs ROM search / Match ROM support.");
            delay_ms(1000u);
        }
    }
}
