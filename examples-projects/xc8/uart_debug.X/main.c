#include "project_config.h"

#include "core/compiler.h"
#include "core/types.h"
#include "core/delay.h"

#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"

/*
 * PIC18F452 UART debug example.
 *
 * Purpose:
 *   Demonstrate the uart_debug helper functions.
 *
 * This example shows:
 *   - simple text output
 *   - text with newline
 *   - HEX byte output
 *   - integer output
 *   - formatted debug-style messages
 *   - repeated alive log
 *
 * Proteus wiring:
 *   RC6/TX / DIP-40 pin 25 -> Virtual Terminal RXD
 *   RC7/RX / DIP-40 pin 26 <- Virtual Terminal TXD, optional
 *
 * Virtual Terminal:
 *   9600 baud
 *   8 data bits
 *   no parity
 *   1 stop bit
 *
 * Important:
 *   PIC Clock Frequency in Proteus must be 10MHz
 *   if the project is built with PIC_PLATFORM_CLOCK_HZ=10000000UL.
 */

static void print_header(void)
{
    /*
     * Print example information.
     *
     * DBG_PRINTLN() prints text and then adds a new line.
     */
    DBG_PRINTLN("");
    DBG_PRINTLN("PIC18F452 UART debug example");
    DBG_PRINTLN("");
    DBG_PRINTLN("Purpose:");
    DBG_PRINTLN("  Demonstrate uart_debug output helpers.");
    DBG_PRINTLN("");
    DBG_PRINTLN("Proteus wiring:");
    DBG_PRINTLN("  RC6/TX pin 25 -> Virtual Terminal RXD");
    DBG_PRINTLN("  RC7/RX pin 26 <- Virtual Terminal TXD, optional");
    DBG_PRINTLN("");
    DBG_PRINTLN("Expected:");
    DBG_PRINTLN("  Terminal shows text, HEX values, integers and alive logs.");
    DBG_PRINTLN("");
}

static void demo_print_text(void)
{
    /*
     * DBG_PRINT() prints text without automatic new line.
     * DBG_PRINTLN() prints text with new line.
     */
    DBG_PRINTLN("=== Text output test ===");

    DBG_PRINT("DBG_PRINT: ");
    DBG_PRINT("hello ");
    DBG_PRINT("from ");
    DBG_PRINT("PIC18F452");
    DBG_PRINTLN("");

    DBG_PRINTLN("DBG_PRINTLN: this line ends automatically");
    DBG_PRINTLN("");
}

static void demo_print_hex(void)
{
    uint8_t value_a = 0x00u;
    uint8_t value_b = 0x55u;
    uint8_t value_c = 0xAAu;
    uint8_t value_d = 0xFFu;

    /*
     * DBG_PRINT_HEX() prints one byte in hexadecimal form.
     *
     * Useful for:
     *   - received UART bytes
     *   - SPI/I2C registers
     *   - sensor raw values
     *   - status flags
     */
    DBG_PRINTLN("=== HEX output test ===");

    DBG_PRINT("HEX 0x00 = ");
    DBG_PRINT_HEX(value_a);
    DBG_PRINTLN("");

    DBG_PRINT("HEX 0x55 = ");
    DBG_PRINT_HEX(value_b);
    DBG_PRINTLN("");

    DBG_PRINT("HEX 0xAA = ");
    DBG_PRINT_HEX(value_c);
    DBG_PRINTLN("");

    DBG_PRINT("HEX 0xFF = ");
    DBG_PRINT_HEX(value_d);
    DBG_PRINTLN("");

    DBG_PRINTLN("");
}

static void demo_print_int(void)
{
    /*
     * DBG_PRINT_INT() prints integer values.
     *
     * Useful for:
     *   - counters
     *   - ADC values
     *   - error codes
     *   - simple measurements
     */
    DBG_PRINTLN("=== Integer output test ===");

    DBG_PRINT("INT 0 = ");
    DBG_PRINT_INT(0);
    DBG_PRINTLN("");

    DBG_PRINT("INT 123 = ");
    DBG_PRINT_INT(123);
    DBG_PRINTLN("");

    DBG_PRINT("INT -45 = ");
    DBG_PRINT_INT(-45);
    DBG_PRINTLN("");

    DBG_PRINTLN("");
}

static void demo_status_messages(void)
{
    uint8_t status_ok = 1u;
    uint8_t status_fail = 0u;
    uint8_t register_value = 0x5Au;

    /*
     * This block demonstrates how debug output can be used
     * in real driver examples.
     */
    DBG_PRINTLN("=== Driver-style status log ===");

    DBG_PRINT("UART init: ");
    if (status_ok != 0u)
    {
        DBG_PRINTLN("OK");
    }
    else
    {
        DBG_PRINTLN("FAIL");
    }

    DBG_PRINT("Fake sensor register 0x01 = ");
    DBG_PRINT_HEX(register_value);
    DBG_PRINTLN("");

    DBG_PRINT("Fake check result: ");
    if (status_fail != 0u)
    {
        DBG_PRINTLN("OK");
    }
    else
    {
        DBG_PRINTLN("FAIL");
    }

    DBG_PRINTLN("");
}

void main(void)
{
    uint16_t loop_counter = 0u;

    /*
     * Initialize UART before using uart_debug.
     *
     * uart_debug prints through the UART driver.
     */
    uart_init(9600u);

    /*
     * Print all one-time demo blocks.
     */
    print_header();
    demo_print_text();
    demo_print_hex();
    demo_print_int();
    demo_status_messages();

    DBG_PRINTLN("UART debug example ready");
    DBG_PRINTLN("");

    while (1)
    {
        /*
         * Periodic alive log.
         *
         * This confirms that:
         *   - main loop is still running
         *   - UART debug output still works
         *   - delay works
         */
        DBG_PRINT("UART debug alive, loop = ");
        DBG_PRINT_INT((int)loop_counter);
        DBG_PRINTLN("");

        loop_counter++;

        /*
         * Wait one second between messages.
         */
        DRV_DELAY_MS(1000u);
    }
}