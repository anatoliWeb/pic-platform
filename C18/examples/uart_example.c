#include "core/compiler.h"
#include "core/delay.h"
#include "core/debug.h"
#include "drivers/uart/uart.h"

void main(void)
{
    uart_init(9600u);
    uart_write_string("C18 UART ready\r\n");

    while (1)
    {
        if (uart_is_data_ready() != 0u)
        {
            uint8_t data = uart_read_byte();
            uart_write_byte(data);
            DBG_PRINT("C18 DBG echo\r\n");
        }

        DRV_DELAY_MS(10);
    }
}

