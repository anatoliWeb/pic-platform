#include "project_config.h"
#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/communication/spi/spi.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"

/*
 * SPI loopback example
 *
 * Notes:
 *   - Sends a test byte and prints the received byte.
 *   - SPI loopback requires external wiring on the test bench.
 */

void main(void)
{
    uint8_t rx;

    uart_init(9600u);
    spi_init_master(SPI_MODE_0, SPI_CLOCK_FOSC_16);

    DBG_PRINTLN("SPI loopback example ready");

    while (1)
    {
        rx = spi_transfer_byte(0xA5u);

        DBG_PRINT("SPI rx = 0x");
        DBG_PRINT_HEX(rx);
        DBG_PRINTLN("");

        delay_ms(1000u);
    }
}