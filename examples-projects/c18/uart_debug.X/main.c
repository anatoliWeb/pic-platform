#include "project_config.h"
#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"

/*
 * UART debug example
 *
 * Notes:
 *   - Keep the example simple and source-only.
 *   - The debug helper prints through the UART driver.
 */

void main(void)
{
    uart_init(9600u);

    DBG_PRINTLN("UART debug example ready");

    while (1)
    {
        DBG_PRINTLN("UART debug alive");
        delay_ms(1000u);
    }
}