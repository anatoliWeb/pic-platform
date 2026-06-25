/*
 * File: drivers/communication/spi/spi.h
 */

#ifndef DRIVERS_SPI_SPI_H
#define DRIVERS_SPI_SPI_H

#include "core/compiler.h"
#include "core/types.h"

typedef enum
{
    SPI_MODE_0 = 0,
    SPI_MODE_1,
    SPI_MODE_2,
    SPI_MODE_3
} spi_mode_t;

typedef enum
{
    SPI_CLOCK_FOSC_4 = 0,
    SPI_CLOCK_FOSC_16,
    SPI_CLOCK_FOSC_64,
    SPI_CLOCK_TMR2
} spi_clock_t;

void spi_init_master(spi_mode_t mode, spi_clock_t clock);
void spi_init_slave(spi_mode_t mode);

uint8_t spi_transfer_byte(uint8_t data);

void spi_transfer_buffer(
    const uint8_t* tx_buffer,
    uint8_t* rx_buffer,
    uint16_t length
);

void spi_cs_init(volatile uint8_t* tris, volatile uint8_t* port, uint8_t pin);
void spi_cs_select(void);
void spi_cs_deselect(void);

#endif /* DRIVERS_SPI_SPI_H */
