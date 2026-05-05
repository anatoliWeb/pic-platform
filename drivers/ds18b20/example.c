#include "core/compiler.h"
#include "drivers/uart/uart.h"
#include "drivers/uart_debug/uart_debug.h"
#include "drivers/onewire/onewire.h"
#include "drivers/ds18b20/ds18b20.h"

static void print_temp_x10(int16_t temp_x10)
{
    int16_t whole = (int16_t)(temp_x10 / 10);
    int16_t frac = (int16_t)(temp_x10 % 10);

    if (frac < 0)
    {
        frac = (int16_t)(-frac);
    }

    DBG_PRINT_INT(whole);
    DBG_PRINT(".");
    DBG_PRINT_INT(frac);
}

void main(void)
{
    int16_t temp_x10;
    uint8_t roms[4][8];
    uint8_t count;
    uint8_t i;

    uart_init(9600u);
    onewire_init(&PORTB, &TRISB, 1u);

    /* Single device mode: use Skip ROM by passing NULL. */
    temp_x10 = ds18b20_get_temperature_celsius_x10((uint8_t*)0);
    DBG_PRINT("Single T(C): ");
    print_temp_x10(temp_x10);
    DBG_PRINT("\r\n");

    /* Multi-device mode: discover ROMs and read each sensor. */
    count = onewire_search_rom(roms, 4u);
    DBG_PRINT("Found devices: ");
    DBG_PRINT_INT(count);
    DBG_PRINT("\r\n");

    for (i = 0u; i < count; i++)
    {
        temp_x10 = ds18b20_get_temperature_celsius_x10(roms[i]);
        DBG_PRINT("ROM[");
        DBG_PRINT_INT(i);
        DBG_PRINT("] T(C): ");
        print_temp_x10(temp_x10);
        DBG_PRINT("\r\n");
    }

    while (1)
    {
    }
}
