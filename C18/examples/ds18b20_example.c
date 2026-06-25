/*
 * File: C18/examples/ds18b20_example.c
 */

#include "core/compiler.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"
#include "drivers/communication/onewire/onewire.h"
#include "libraries/sensors/ds18b20/ds18b20.h"

void main(void)
{
    int16_t temp_x10;

    uart_init(9600u);
    onewire_init(&PORTB, &TRISB, 1u);

    temp_x10 = ds18b20_get_temperature_celsius_x10((uint8_t*)0);

    DBG_PRINT("C18 DS18B20 x10: ");
    DBG_PRINT_INT(temp_x10);
    DBG_PRINT("\r\n");

    while (1)
    {
    }
}
