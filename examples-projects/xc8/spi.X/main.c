#include "project_config.h"

#include "core/compiler.h"
#include "core/delay.h"

#include "drivers/communication/spi/spi.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"

/*
 * SPI basic example for PIC18F452.
 *
 * External SPI device:
 *   74HC595 shift register
 *
 * What this example does:
 *   1. Initializes UART debug output.
 *   2. Initializes PIC18F452 hardware SPI in master mode.
 *   3. Sends different byte patterns to 74HC595.
 *   4. 74HC595 shows the received byte on Q0..Q7 outputs.
 *   5. LEDs connected to Q0..Q7 show the SPI data visually.
 *
 * PIC18F452 SPI pins:
 *   RC3/SCK / DIP-40 pin 18 -> 74HC595 SH_CP / SRCLK / pin 11
 *   RC5/SDO / DIP-40 pin 24 -> 74HC595 DS / SER / pin 14
 *
 * Software latch pin:
 *   RB2 / DIP-40 pin 35 -> 74HC595 ST_CP / RCLK / pin 12
 *
 * UART debug:
 *   RC6/TX / DIP-40 pin 25 -> Virtual Terminal RXD
 *
 * Note:
 *   RC4/SDI / DIP-40 pin 23 is not used in this example,
 *   because 74HC595 is an output-only shift register.
 */

#define SPI_PATTERN_COUNT  12u

static const uint8_t spi_patterns[SPI_PATTERN_COUNT] =
{
    0x01u,
    0x02u,
    0x04u,
    0x08u,
    0x10u,
    0x20u,
    0x40u,
    0x80u,
    0x55u,
    0xAAu,
    0xFFu,
    0x00u
};

static void print_header(void)
{
    /*
     * Print a detailed description to the Virtual Terminal.
     *
     * This helps when the Proteus project is opened later:
     * the terminal itself explains what the example does
     * and how the external chip must be wired.
     */
    DBG_PRINTLN("");
    DBG_PRINTLN("PIC18F452 SPI basic example");
    DBG_PRINTLN("External chip: 74HC595 shift register");
    DBG_PRINTLN("");
    DBG_PRINTLN("Purpose:");
    DBG_PRINTLN("  Send bytes over SPI and show them on 8 LEDs.");
    DBG_PRINTLN("");
    DBG_PRINTLN("Proteus wiring:");
    DBG_PRINTLN("  RC3/SCK pin 18 -> 74HC595 SH_CP/SRCLK pin 11");
    DBG_PRINTLN("  RC5/SDO pin 24 -> 74HC595 DS/SER pin 14");
    DBG_PRINTLN("  RB2     pin 35 -> 74HC595 ST_CP/RCLK pin 12");
    DBG_PRINTLN("  74HC595 OE  pin 13 -> GND");
    DBG_PRINTLN("  74HC595 MR  pin 10 -> +5V");
    DBG_PRINTLN("  74HC595 VCC pin 16 -> +5V");
    DBG_PRINTLN("  74HC595 GND pin 8  -> GND");
    DBG_PRINTLN("  Q0..Q7 -> LEDs through 330R resistors");
    DBG_PRINTLN("");
    DBG_PRINTLN("UART:");
    DBG_PRINTLN("  RC6/TX pin 25 -> Virtual Terminal RXD");
    DBG_PRINTLN("  Baud: 9600 8N1");
    DBG_PRINTLN("");
    DBG_PRINTLN("Expected:");
    DBG_PRINTLN("  LEDs should move and change according to sent byte.");
    DBG_PRINTLN("");
}

static void spi_74hc595_write(uint8_t value)
{
    /*
     * Send one byte to 74HC595 using SPI.
     *
     * 74HC595 needs two actions:
     *
     * 1. Shift data into internal shift register:
     *      SPI sends 8 bits through DS/SER using SCK.
     *
     * 2. Latch shifted data to output pins Q0..Q7:
     *      ST_CP/RCLK must receive a rising edge.
     *
     * In this example RB2 is used as latch pin.
     * The existing SPI CS helper is reused for latch control:
     *
     *   spi_cs_select()   -> RB2 low
     *   spi_transfer_byte -> shift 8 bits
     *   spi_cs_deselect() -> RB2 high, creates latch rising edge
     */
    spi_cs_select();

    /*
     * Transfer byte over SPI.
     *
     * 74HC595 does not return meaningful data.
     * The returned byte is ignored.
     */
    (void)spi_transfer_byte(value);

    /*
     * Rising edge on latch pin updates Q0..Q7 outputs.
     */
    spi_cs_deselect();
}

static void print_pattern(uint8_t value)
{
    /*
     * Print the byte that is being sent.
     *
     * Example:
     *   SPI TX to 74HC595 = 0x55
     */
    DBG_PRINT("SPI TX to 74HC595 = ");
    DBG_PRINT_HEX(value);
    DBG_PRINTLN("");
}

void main(void)
{
    uint8_t index;

    /*
     * Initialize UART for debug messages.
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
     *   Data is sampled according to SPI mode 0 timing.
     *
     * SPI_CLOCK_FOSC_16:
     *   SPI clock = Fosc / 16.
     *
     * If project clock is 10 MHz:
     *   SPI clock is about 625 kHz.
     */
    spi_init_master(SPI_MODE_0, SPI_CLOCK_FOSC_16);

    /*
     * Initialize latch pin for 74HC595.
     *
     * spi_cs_init(&TRISB, &PORTB, 2u) means:
     *   RB2 / DIP-40 pin 35 is used as software CS/latch pin.
     *
     * For 74HC595 this pin is connected to:
     *   ST_CP / RCLK / pin 12
     */
    spi_cs_init(&TRISB, &PORTB, 2u);

    /*
     * Print description once after reset.
     */
    print_header();

    while (1)
    {
        /*
         * Send all test patterns one by one.
         */
        for (index = 0u; index < SPI_PATTERN_COUNT; index++)
        {
            /*
             * Print current byte to terminal.
             */
            print_pattern(spi_patterns[index]);

            /*
             * Send current byte to 74HC595.
             */
            spi_74hc595_write(spi_patterns[index]);

            /*
             * Keep this pattern visible on LEDs.
             */
            DRV_DELAY_MS(500u);
        }

        DBG_PRINTLN("Pattern cycle finished.");
        DBG_PRINTLN("");
    }
}