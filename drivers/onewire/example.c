#include "core/compiler.h"
#include "drivers/uart/uart.h"
#include "drivers/uart_debug/uart_debug.h"
#include "drivers/onewire/onewire.h"

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
    DBG_PRINT("\r\n");
}

void main(void)
{
    uint8_t present;
    uint8_t rom[8];
    uint8_t found_roms[2][8];
    uint8_t found_count;

    uart_init(9600u);
    onewire_init(&PORTB, &TRISB, 1u);

    present = onewire_reset();

    if (present != 0u)
    {
        DBG_PRINT("Device found\r\n");

        if (onewire_reset() != 0u)
        {
            onewire_read_rom(rom);
            DBG_PRINT("ROM: ");
            print_rom(rom);
        }

        /* TODO: Search ROM not implemented yet (placeholder returns 0). */
        if (onewire_reset() != 0u)
        {
            found_count = onewire_search_rom(found_roms, 2u);
            DBG_PRINT("Search count: 0x");
            DBG_PRINT_HEX(found_count);
            DBG_PRINT("\r\n");
        }
    }
    else
    {
        DBG_PRINT("No device\r\n");
    }

    while (1)
    {
    }
}
