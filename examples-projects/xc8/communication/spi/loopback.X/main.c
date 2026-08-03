/*
 * File: examples-projects/xc8/spi_loopback.X/main.c
 */

#include "project_config.h"

#include "core/compiler.h"
#include "core/delay.h"

#include "drivers/communication/spi/spi.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"

/*
 * SPI loopback example for PIC18F452.
 *
 * This example tests the PIC18F452 hardware SPI module without
 * any external SPI slave chip.
 *
 * Test idea:
 *   The SPI output pin is connected directly to the SPI input pin.
 *
 * Required Proteus wiring:
 *   RC5/SDO / DIP-40 pin 24 -> RC4/SDI / DIP-40 pin 23
 *
 * When the PIC sends a byte through SDO, the same byte comes back
 * through SDI.
 *
 * Expected result:
 *   TX byte must be equal to RX byte.
 *
 * PIC18F452 SPI pins:
 *   RC3/SCK / DIP-40 pin 18 - SPI clock output
 *   RC4/SDI / DIP-40 pin 23 - SPI data input
 *   RC5/SDO / DIP-40 pin 24 - SPI data output
 *
 * UART debug:
 *   RC6/TX / DIP-40 pin 25 -> Virtual Terminal RXD
 *
 * This example is useful before testing a real SPI chip.
 * If loopback works, the SPI driver and PIC SPI pins are basically OK.
 */

#define SPI_TEST_PATTERN_COUNT  8u

static const uint8_t spi_test_patterns[SPI_TEST_PATTERN_COUNT] =
{
    0x00u,
    0xFFu,
    0xA5u,
    0x5Au,
    0x55u,
    0xAAu,
    0x0Fu,
    0xF0u
};

static void print_header(void)
{
    /*
     * Print example description to Virtual Terminal.
     */
    DBG_PRINTLN("");
    DBG_PRINTLN("PIC18F452 SPI loopback example");
    DBG_PRINTLN("");
    DBG_PRINTLN("Purpose:");
    DBG_PRINTLN("  Test hardware SPI without external SPI chip.");
    DBG_PRINTLN("");
    DBG_PRINTLN("Required Proteus wiring:");
    DBG_PRINTLN("  RC5/SDO pin 24 -> RC4/SDI pin 23");
    DBG_PRINTLN("");
    DBG_PRINTLN("SPI pins:");
    DBG_PRINTLN("  RC3/SCK pin 18 -> SPI clock");
    DBG_PRINTLN("  RC4/SDI pin 23 -> SPI input");
    DBG_PRINTLN("  RC5/SDO pin 24 -> SPI output");
    DBG_PRINTLN("");
    DBG_PRINTLN("UART:");
    DBG_PRINTLN("  RC6/TX pin 25 -> Virtual Terminal RXD");
    DBG_PRINTLN("  Baud: 9600 8N1");
    DBG_PRINTLN("");
    DBG_PRINTLN("Expected:");
    DBG_PRINTLN("  TX and RX values must be equal.");
    DBG_PRINTLN("");
}

static void print_test_result(uint8_t tx, uint8_t rx)
{
    /*
     * Print one SPI loopback test result.
     *
     * PASS means:
     *   received byte is exactly the same as transmitted byte.
     *
     * FAIL usually means:
     *   - RC5/SDO is not connected to RC4/SDI
     *   - wrong SPI pin wiring
     *   - SPI driver initialization problem
     */
    DBG_PRINT("SPI TX=");
    DBG_PRINT_HEX(tx);

    DBG_PRINT(" RX=");
    DBG_PRINT_HEX(rx);

    DBG_PRINT(" ");

    if (tx == rx)
    {
        DBG_PRINTLN("PASS");
    }
    else
    {
        DBG_PRINTLN("FAIL");
    }
}

void main(void)
{
    uint8_t index;
    uint8_t tx;
    uint8_t rx;

    /*
     * Initialize UART for debug output.
     *
     * PIC18F452:
     *   RC6/TX / pin 25 -> Virtual Terminal RXD
     *   RC7/RX / pin 26 <- Virtual Terminal TXD, optional
     */
    uart_init(9600u);

    /*
     * Initialize SPI in master mode.
     *
     * SPI_MODE_0:
     *   Clock idle state is low.
     *
     * SPI_CLOCK_FOSC_16:
     *   SPI clock = Fosc / 16.
     *
     * If Fosc is 10 MHz:
     *   SPI clock is about 625 kHz.
     */
    spi_init_master(SPI_MODE_0, SPI_CLOCK_FOSC_16);

    /*
     * Print test information once after reset.
     */
    print_header();

    while (1)
    {
        /*
         * Send all test patterns one by one.
         */
        for (index = 0u; index < SPI_TEST_PATTERN_COUNT; index++)
        {
            tx = spi_test_patterns[index];

            /*
             * SPI is full-duplex:
             *   every transmitted byte also reads one received byte.
             *
             * Because SDO is connected to SDI,
             * RX should be equal to TX.
             */
            rx = spi_transfer_byte(tx);

            /*
             * Print TX/RX comparison.
             */
            print_test_result(tx, rx);

            /*
             * Small delay to make terminal output readable.
             */
            DRV_DELAY_MS(500u);
        }

        DBG_PRINTLN("SPI loopback cycle finished.");
        DBG_PRINTLN("");

        DRV_DELAY_MS(1000u);
    }
}