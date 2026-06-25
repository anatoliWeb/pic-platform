/*
 * File: libraries/system/uart_debug/example.c
 */

#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"

void main(void)
{
    uart_init(9600u);

    DBG_PRINTLN("UART debug module ready");
    DBG_PRINT("Value: ");
    DBG_PRINT_INT(1234);
    DBG_PRINT(" Hex: ");
    DBG_PRINT_HEX(0xABu);
    DBG_PRINTLN("");

    while (1)
    {
        if (uart_is_data_ready() != 0u)
        {
            uint8_t data = uart_read_byte();

            DBG_PRINT("RX=");
            DBG_WRITE_BYTE(data);
            DBG_PRINT(" (hex ");
            DBG_PRINT_HEX(data);
            DBG_PRINTLN(")");
        }

        DRV_DELAY_MS(10);
    }
}
