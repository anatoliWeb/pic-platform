#include "project_config.h"

#include "core/compiler.h"
#include "core/delay.h"

#include "drivers/communication/rs485/rs485.h"
#include "drivers/communication/uart/uart.h"

#if defined(DRV_COMPILER_C18)
#include <p18f452.h>
#endif

#if defined(DRV_COMPILER_XC8)
#include <xc.h>
#endif

/*
 * RS485 echo example for PIC18F452.
 *
 * This example receives a complete text line from the RS485 bus
 * and sends the same line back.
 *
 * Important:
 *   This is a half-duplex RS485 example.
 *   The device must not answer while the sender is still transmitting.
 *
 * Correct behavior:
 *   1. Receive bytes into RAM buffer.
 *   2. Wait until end of line: '\n'.
 *   3. Switch MAX487 to TX mode.
 *   4. Send the complete line back.
 *   5. Return MAX487 to RX mode.
 *
 * PIC18F452 UART pins:
 *   RC6/TX / DIP-40 pin 25 -> MAX487 DI
 *   RC7/RX / DIP-40 pin 26 <- MAX487 RO
 *
 * RS485 direction control:
 *   RB2 / DIP-40 pin 35 -> MAX487 DE
 *   RB2 / DIP-40 pin 35 -> MAX487 /RE
 *
 * Direction logic:
 *   RB2 = 1 -> transmit mode
 *   RB2 = 0 -> receive mode
 */

#define RS485_ECHO_BUFFER_SIZE  32u

static uint8_t rx_buffer[RS485_ECHO_BUFFER_SIZE];
static uint8_t rx_len = 0u;

static void rs485_send_raw_buffer(uint8_t* data, uint8_t len)
{
    uint8_t i;

    if ((data == (uint8_t*)0) || (len == 0u))
    {
        return;
    }

    /*
     * Enable RS485 transmit mode.
     *
     * RB2 = 1:
     *   MAX487 DE  = 1, driver enabled
     *   MAX487 /RE = 1, receiver disabled
     */
    rs485_set_tx();

    /*
     * Small guard delay.
     * Gives MAX487 time to enable the driver before first UART bit.
     */
    DRV_DELAY_US(50u);

    /*
     * Send all received bytes back.
     */
    for (i = 0u; i < len; i++)
    {
        rs485_send_byte(data[i]);
    }

    /*
     * Critical:
     * Wait until the last byte is fully shifted out.
     *
     * TXIF only means TXREG is empty.
     * TRMT means the UART shift register is empty too.
     */
    while (TXSTAbits.TRMT == 0u)
    {
    }

    /*
     * Small guard delay before releasing the RS485 bus.
     */
    DRV_DELAY_US(50u);

    /*
     * Return to receive mode.
     *
     * RB2 = 0:
     *   MAX487 DE  = 0, driver disabled
     *   MAX487 /RE = 0, receiver enabled
     */
    rs485_set_rx();
}

void main(void)
{
    uint8_t data;

    /*
     * Initialize UART.
     *
     * UART settings:
     *   9600 baud
     *   8N1
     *
     * The RS485 driver uses this UART internally.
     */
    uart_init(9600u);

    /*
     * Initialize RS485 direction control.
     *
     * rs485_init(&PORTB, &TRISB, 2u) means:
     *   RB2 / pin 35 controls MAX487 DE and /RE.
     */
    rs485_init(&PORTB, &TRISB, 2u);

    /*
     * Echo node starts in receive mode.
     */
    rs485_set_rx();

    /*
     * Do not use DBG_PRINT here.
     *
     * This example uses the only hardware UART for RS485.
     * Debug printing through the same UART can corrupt the RS485 test.
     */

    while (1)
    {
        /*
         * Poll UART for bytes received from MAX487 RO.
         */
        if (uart_is_data_ready() != 0u)
        {
            data = rs485_read_byte();

            /*
             * Store byte into local receive buffer.
             */
            if (rx_len < RS485_ECHO_BUFFER_SIZE)
            {
                rx_buffer[rx_len] = data;
                rx_len++;
            }

            /*
             * Echo only after full line is received.
             *
             * Sender should send text like:
             *   PING\r\n
             *
             * '\n' means the sender has finished the message.
             */
            if ((data == '\n') || (rx_len >= RS485_ECHO_BUFFER_SIZE))
            {
                /*
                 * Small delay before answering.
                 * This helps make sure the sender has released the bus.
                 */
                DRV_DELAY_MS(2u);

                /*
                 * Send the complete received line back.
                 */
                rs485_send_raw_buffer(rx_buffer, rx_len);

                /*
                 * Clear buffer for next message.
                 */
                rx_len = 0u;
            }
        }

        /*
         * Small polling delay.
         */
        DRV_DELAY_MS(1u);
    }
}