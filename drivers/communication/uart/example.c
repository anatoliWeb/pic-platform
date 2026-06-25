/*
 * File: drivers/communication/uart/example.c
 */

#include "core/compiler.h"
#include "core/delay.h"
#include "core/debug.h"
#include "drivers/communication/uart/uart.h"

static void uart_example_send_string(void)
{
    uart_write_string("Hello UART\r\n");
}

static void uart_example_debug_print(uint8_t value)
{
    DBG_PRINT("DBG: value=\r\n");
    uart_write_byte(value);
    uart_write_string("\r\n");
}

void main(void)
{
    uart_init(9600u);
    uart_example_send_string();

    while (1)
    {
        if (uart_is_data_ready() != 0u)
        {
            uint8_t data = uart_read_byte();

            /* Echo received byte. */
            uart_write_byte(data);

            /* Debug-style print hook. */
            uart_example_debug_print(data);
        }

        DRV_DELAY_MS(10);
    }
}
