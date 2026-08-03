/*
 * File: examples-projects/xc8/rs485_basic.X/main.c
 */

#include "project_config.h"

#include "core/compiler.h"
#include "core/delay.h"

#include "drivers/communication/uart/uart.h"
#include "drivers/communication/rs485/rs485.h"

void main(void)
{
    /*
     * RS485 basic ASCII test frame.
     *
     * This frame is readable in Proteus Virtual Terminal.
     * It is better for visual testing than binary bytes like 0x01 0x55.
     */
    static const uint8_t frame[] = "RS485 BASIC OK\r\n";

    /*
     * Initialize UART.
     *
     * PIC18F452 UART:
     *   RC6/TX / pin 25 -> MAX487 DI
     *   RC7/RX / pin 26 <- MAX487 RO, optional for this TX-only test
     *
     * UART settings:
     *   9600 baud
     *   8N1
     */
    uart_init(9600u);

    /*
     * Initialize RS485 direction control.
     *
     * rs485_init(&PORTB, &TRISB, 2u) means:
     *   RB2 / pin 35 controls MAX487 direction.
     *
     * MAX487 #1:
     *   RB2 -> DE
     *   RB2 -> /RE
     *
     * Typical behavior:
     *   RB2 = 1 -> transmit mode
     *   RB2 = 0 -> receive mode
     */
    rs485_init(&PORTB, &TRISB, 2u);

    while (1)
    {
        /*
         * Send readable ASCII text over RS485.
         *
         * Expected on receiver Virtual Terminal:
         *   RS485 BASIC OK
         */
        rs485_send_frame((uint8_t*)frame, (uint8_t)(sizeof(frame) - 1u));

        /*
         * Repeat every 500 ms.
         */
        DRV_DELAY_MS(500);
    }
}