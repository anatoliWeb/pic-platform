#include "project_config.h"

#include "core/compiler.h"
#include "core/types.h"
#include "core/delay.h"
#include "core/debug.h"

#include "drivers/communication/uart/uart.h"

/*
 * PIC18F452 UART example.
 *
 * Purpose:
 *   Test the hardware UART module.
 *
 * What this example does:
 *   1. Initializes UART at 9600 baud.
 *   2. Prints startup message to Virtual Terminal.
 *   3. Waits for received bytes.
 *   4. Echoes every received byte back to terminal.
 *   5. Prints debug message after each received byte.
 *
 * Proteus wiring:
 *   RC6/TX / DIP-40 pin 25 -> Virtual Terminal RXD
 *   RC7/RX / DIP-40 pin 26 <- Virtual Terminal TXD
 *
 * Virtual Terminal settings:
 *   9600 baud
 *   8 data bits
 *   no parity
 *   1 stop bit
 *
 * Important:
 *   PIC Clock Frequency in Proteus must be 10MHz
 *   if the project is built with PIC_PLATFORM_CLOCK_HZ=10000000UL.
 */

void main(void)
{
    /*
     * Initialize UART.
     *
     * PIC18F452 hardware UART pins:
     *   RC6/TX / pin 25 - UART transmit
     *   RC7/RX / pin 26 - UART receive
     *
     * Baud rate:
     *   9600
     */
    uart_init(9600u);

    /*
     * Print startup message.
     *
     * If this text is unreadable in Proteus, check:
     *   - PIC Clock Frequency = 10MHz
     *   - Virtual Terminal baud = 9600
     *   - RC6/TX is connected to RXD
     */
    uart_write_string("XC8 UART ready\r\n");

    /*
     * Main loop.
     */
    while (1)
    {
        /*
         * Check if one byte was received from Virtual Terminal.
         *
         * User can type characters in the Virtual Terminal window.
         */
        if (uart_is_data_ready() != 0u)
        {
            /*
             * Read received byte.
             */
            uint8_t data = uart_read_byte();

            /*
             * Echo the same byte back.
             *
             * Example:
             *   user types A
             *   terminal receives A back
             */
            uart_write_byte(data);

            /*
             * Print debug message.
             *
             * Note:
             *   DBG_PRINT depends on core/debug.h configuration.
             *   If debug is disabled in project_config/core config,
             *   this line may produce no output.
             *
             * The byte echo above will still work.
             */
            DBG_PRINT("XC8 DBG echo\r\n");
        }

        /*
         * Small delay to avoid aggressive polling.
         */
        DRV_DELAY_MS(10u);
    }
}