#include "drivers/communication/spi/spi.h"

#if defined(DRV_COMPILER_C18)
    #include "../../../C18/drivers/communication/spi/spi.c"
#elif defined(DRV_COMPILER_XC8)
    #include "../../../XC8/drivers/communication/spi/spi.c"
#else

#include "drivers/gpio/gpio.h"

static volatile uint8_t* g_spi_cs_port = (volatile uint8_t*)0;
static volatile uint8_t* g_spi_cs_tris = (volatile uint8_t*)0;
static uint8_t g_spi_cs_pin = 0u;

static void spi_apply_mode(spi_mode_t mode)
{
    switch (mode)
    {
        case SPI_MODE_0: SSPCON1bits.CKP = 0u; SSPSTATbits.CKE = 1u; break;
        case SPI_MODE_1: SSPCON1bits.CKP = 0u; SSPSTATbits.CKE = 0u; break;
        case SPI_MODE_2: SSPCON1bits.CKP = 1u; SSPSTATbits.CKE = 0u; break;
        case SPI_MODE_3: SSPCON1bits.CKP = 1u; SSPSTATbits.CKE = 1u; break;
        default:         SSPCON1bits.CKP = 0u; SSPSTATbits.CKE = 1u; break;
    }

    SSPSTATbits.SMP = 0u;
}

static uint8_t spi_clock_bits(spi_clock_t clock)
{
    switch (clock)
    {
        case SPI_CLOCK_FOSC_4:  return 0x00u;
        case SPI_CLOCK_FOSC_16: return 0x01u;
        case SPI_CLOCK_FOSC_64: return 0x02u;
        case SPI_CLOCK_TMR2:    return 0x03u;
        default:                return 0x00u;
    }
}

void spi_init_master(spi_mode_t mode, spi_clock_t clock)
{
    TRISCbits.RC3 = 0u;
    TRISCbits.RC5 = 0u;
    TRISCbits.RC4 = 1u;

    SSPCON1bits.SSPEN = 0u;
    SSPCON1bits.SSPM = spi_clock_bits(clock);
    spi_apply_mode(mode);
    SSPCON1bits.SSPEN = 1u;
}

void spi_init_slave(spi_mode_t mode)
{
    TRISCbits.RC3 = 1u;
    TRISCbits.RC5 = 0u;
    TRISCbits.RC4 = 1u;

    SSPCON1bits.SSPEN = 0u;
    SSPCON1bits.SSPM = 0x04u;
    spi_apply_mode(mode);
    SSPCON1bits.SSPEN = 1u;
}

uint8_t spi_transfer_byte(uint8_t data)
{
    PIR1bits.SSPIF = 0u;
    SSPBUF = data;
    while (PIR1bits.SSPIF == 0u)
    {
    }
    return SSPBUF;
}

void spi_transfer_buffer(const uint8_t* tx_buffer, uint8_t* rx_buffer, uint16_t length)
{
    uint16_t i;
    uint8_t tx;
    uint8_t rx;

    for (i = 0u; i < length; i++)
    {
        tx = (tx_buffer != (const uint8_t*)0) ? tx_buffer[i] : 0xFFu;
        rx = spi_transfer_byte(tx);

        if (rx_buffer != (uint8_t*)0)
        {
            rx_buffer[i] = rx;
        }
    }
}

void spi_cs_init(volatile uint8_t* tris, volatile uint8_t* port, uint8_t pin)
{
    g_spi_cs_tris = tris;
    g_spi_cs_port = port;
    g_spi_cs_pin = pin;

    gpio_set_output(g_spi_cs_tris, g_spi_cs_pin);
    spi_cs_deselect();
}

void spi_cs_select(void)
{
    if (g_spi_cs_port != (volatile uint8_t*)0)
    {
        gpio_write_low(g_spi_cs_port, g_spi_cs_pin);
    }
}

void spi_cs_deselect(void)
{
    if (g_spi_cs_port != (volatile uint8_t*)0)
    {
        gpio_write_high(g_spi_cs_port, g_spi_cs_pin);
    }
}

#endif
