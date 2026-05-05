#include <xc.h>
#include "core/compiler.h"
#include "drivers/spi/spi.h"
#include "drivers/uart/uart.h"
#include "drivers/uart_debug/uart_debug.h"

void main(void)
{
    uint8_t rx;

    uart_init(9600u);
    spi_init_master(SPI_MODE_0, SPI_CLOCK_FOSC_16);
    spi_cs_init(&TRISB, &PORTB, 2u);

    spi_cs_select();
    rx = spi_transfer_byte(0x9Fu);
    spi_cs_deselect();

    DBG_PRINT("XC8 SPI RX=");
    DBG_PRINT_HEX(rx);
    DBG_PRINTLN("");

    while (1)
    {
    }
}
