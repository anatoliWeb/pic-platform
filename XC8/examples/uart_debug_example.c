#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"

void main(void)
{
    uart_init(9600u);
    DBG_PRINTLN("XC8 UART debug ready");

    while (1)
    {
        DBG_PRINT("Counter=");
        DBG_PRINT_INT(8);
        DBG_PRINT(" Hex=");
        DBG_PRINT_HEX(0x08u);
        DBG_PRINTLN("");
        DRV_DELAY_MS(250);
    }
}

