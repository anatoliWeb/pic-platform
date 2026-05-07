#include "core/compiler.h"
#include "drivers/communication/spi/spi.h"
#include "libraries/system/uart_debug/uart_debug.h"

#define SPI_DUMMY 0xFFu

static uint8_t spi_sensor_read_example(uint8_t reg)
{
    uint8_t value;

    spi_cs_select();
    spi_transfer_byte((uint8_t)(reg | 0x80u));
    value = spi_transfer_byte(SPI_DUMMY);
    spi_cs_deselect();

    return value;
}

static void spi_flash_read_example(void)
{
    uint8_t cmd[4];
    uint8_t rx;

    cmd[0] = 0x03u;
    cmd[1] = 0x00u;
    cmd[2] = 0x00u;
    cmd[3] = 0x10u;

    spi_cs_select();
    spi_transfer_buffer(cmd, (uint8_t*)0, 4u);
    rx = spi_transfer_byte(SPI_DUMMY);
    spi_cs_deselect();

    DBG_PRINT("SPI FLASH RX=");
    DBG_PRINT_HEX(rx);
    DBG_PRINTLN("");
}

void main(void)
{
    uint8_t sensor_val;

    spi_init_master(SPI_MODE_0, SPI_CLOCK_FOSC_16);
    spi_cs_init(&TRISB, &PORTB, 2u);

    sensor_val = spi_sensor_read_example(0x0Fu);
    DBG_PRINT("SENSOR=");
    DBG_PRINT_HEX(sensor_val);
    DBG_PRINTLN("");

    spi_flash_read_example();

    while (1)
    {
    }
}
