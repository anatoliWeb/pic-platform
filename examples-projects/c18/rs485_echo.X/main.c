#include "project_config.h"
#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/communication/rs485/rs485.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"

#if defined(DRV_COMPILER_C18)
#include <p18f452.h>
#endif

/*
 * RS485 echo example
 *
 * Notes:
 *   - Uses RB2 as the DE/RE direction pin.
 *   - The frame protocol is handled by the RS485 driver.
 */

void main(void)
{
    uint8_t data;

    uart_init(9600u);
    rs485_init(&PORTB, &TRISB, 2u);
    rs485_set_rx();

    DBG_PRINTLN("RS485 echo example ready");

    while (1)
    {
        if (uart_is_data_ready() != 0u)
        {
            data = rs485_read_byte();

            DBG_PRINT("RS485 rx = 0x");
            DBG_PRINT_HEX(data);
            DBG_PRINTLN("");

            rs485_set_tx();
            rs485_send_byte(data);
            rs485_set_rx();
        }

        delay_ms(10u);
    }
}