/*
 * File: examples-projects/c18/ds18b20_read.X/main.c
 */

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
 * DS18B20 read example
 *
 * Notes:
 *   - Uses RB1 for the 1-Wire bus.
 *   - Passes NULL ROM so the library uses Skip ROM.
 */

void main(void)
{
    int16_t temp_x10;

    uart_init(9600u);
    onewire_init(&PORTB, &TRISB, 1u);

    DBG_PRINTLN("DS18B20 read example ready");

    while (1)
    {
        temp_x10 = ds18b20_get_temperature_celsius_x10((uint8_t*)0);

        DBG_PRINT("Temp x10 = ");
        DBG_PRINT_INT((int)temp_x10);
        DBG_PRINTLN("");

        delay_ms(1000u);
    }
}