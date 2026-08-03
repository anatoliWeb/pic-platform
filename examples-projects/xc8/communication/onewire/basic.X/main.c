/*
 * File: examples-projects/xc8/onewire.X/main.c
 */

#include "project_config.h"

#include "core/compiler.h"
#include "core/delay.h"

#include "drivers/communication/onewire/onewire.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"

#define ONEWIRE_PORT        PORTB
#define ONEWIRE_TRIS        TRISB
#define ONEWIRE_PIN         1u

#define MAX_1WIRE_DEVICES   7u

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
            DBG_PRINT(" ");
        }
    }
}

static void run_reset_test(void)
{
    uint8_t presence;

    DBG_PRINTLN("");
    DBG_PRINTLN("OneWire reset/presence test");
    DBG_PRINTLN("DQ=RB1/pin34, pull-up=4.7k");

    /*
     * onewire_reset() should generate approximately:
     *   reset low  ~580 us
     *   release    ~50 us
     *   wait tail  ~480 us
     *
     * In Proteus, use onewire_use_proteus_pic18f452_timing()
     * before calling reset.
     */
    presence = onewire_reset();

    if (presence != 0u)
    {
        DBG_PRINTLN("Presence: OK");
    }
    else
    {
        DBG_PRINTLN("Presence: NO DEVICE");
        DBG_PRINTLN("Check DQ, pull-up, DS18B20 VDD/GND.");
    }
}

static void run_search_rom_test(void)
{
    uint8_t roms[MAX_1WIRE_DEVICES][8];
    uint8_t count;
    uint8_t i;

    DBG_PRINTLN("");
    DBG_PRINTLN("OneWire Search ROM test");

    count = onewire_search_rom(roms, MAX_1WIRE_DEVICES);

    DBG_PRINT("Devices found: ");
    DBG_PRINT_INT((int)count);
    DBG_PRINTLN("");

    if (count == 0u)
    {
        DBG_PRINTLN("No devices found by Search ROM.");
        return;
    }

    for (i = 0u; i < count; i++)
    {
        DBG_PRINT("ROM ");
        DBG_PRINT_INT((int)i);
        DBG_PRINT(": ");
        print_rom(roms[i]);

        if (onewire_crc8(roms[i], 8u) == 0u)
        {
            DBG_PRINTLN(" CRC_OK");
        }
        else
        {
            DBG_PRINTLN(" CRC_FAIL");
        }
    }
}

void main(void)
{
    /*
     * PIC18F452 OneWire low-level Proteus test.
     *
     * Proteus wiring:
     *   DS18B20 DQ  -> RB1 / pin 34
     *   DQ pull-up  -> 4.7k -> +5V
     *   DS18B20 VDD -> +5V
     *   DS18B20 GND -> GND
     *
     * UART debug:
     *   RC6/TX / pin 25 -> Virtual Terminal RXD
     *   RC7/RX / pin 26 <- Virtual Terminal TXD
     */

    uart_init(9600u);

    onewire_init(&ONEWIRE_PORT, &ONEWIRE_TRIS, ONEWIRE_PIN);

    /*
     * Critical for Proteus + PIC18F452 + 10 MHz.
     * Without this tuning, reset/write/read pulses can become too long
     * or too short in simulation.
     */
    onewire_use_proteus_pic18f452_timing();

    DBG_PRINTLN("");
    DBG_PRINTLN("PIC18F452 OneWire example");
    DBG_PRINTLN("Device: DS18B20");
    DBG_PRINTLN("DQ=RB1/pin34");
    DBG_PRINTLN("UART: 9600 8N1");
    DBG_PRINTLN("Timing: Proteus PIC18F452 tuned");

    while (1)
    {
        run_reset_test();
        run_search_rom_test();

        DBG_PRINTLN("");
        DBG_PRINTLN("Next test in 3 seconds...");
        DBG_PRINTLN("");

        DRV_DELAY_MS(3000);
    }
}