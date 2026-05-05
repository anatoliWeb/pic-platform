#include "drivers/i2c/i2c.h"

#if defined(DRV_COMPILER_C18)
    #include "../../C18/drivers/i2c/i2c.c"
#elif defined(DRV_COMPILER_XC8)
    #include "../../XC8/drivers/i2c/i2c.c"
#else

#include "core/device.h"

static void i2c_wait_idle(void)
{
    while ((SSPCON2 & 0x1Fu) != 0u)
    {
    }
    while (SSPSTATbits.R_W != 0u)
    {
    }
}

void i2c_init(uint32_t clock_hz)
{
    uint32_t add;

    if (clock_hz == 0u)
    {
        clock_hz = 100000u;
    }

    TRISCbits.RC3 = 1u;
    TRISCbits.RC4 = 1u;

    SSPCON1 = 0x28u;
    SSPCON2 = 0x00u;
    SSPSTAT = 0x80u;

    add = ((_XTAL_FREQ / (4u * clock_hz)) - 1u);
    if (add > 255u)
    {
        add = 255u;
    }
    SSPADD = (uint8_t)add;
}

void i2c_start(void)
{
    i2c_wait_idle();
    SSPCON2bits.SEN = 1u;
    while (SSPCON2bits.SEN != 0u)
    {
    }
}

void i2c_restart(void)
{
    i2c_wait_idle();
    SSPCON2bits.RSEN = 1u;
    while (SSPCON2bits.RSEN != 0u)
    {
    }
}

void i2c_stop(void)
{
    i2c_wait_idle();
    SSPCON2bits.PEN = 1u;
    while (SSPCON2bits.PEN != 0u)
    {
    }
}

uint8_t i2c_write_byte(uint8_t data)
{
    i2c_wait_idle();
    PIR1bits.SSPIF = 0u;
    SSPBUF = data;
    while (PIR1bits.SSPIF == 0u)
    {
    }
    return SSPCON2bits.ACKSTAT;
}

uint8_t i2c_read_byte(uint8_t ack)
{
    uint8_t value;

    i2c_wait_idle();
    SSPCON2bits.RCEN = 1u;
    while (SSPCON2bits.RCEN != 0u)
    {
    }

    value = SSPBUF;

    i2c_wait_idle();
    SSPCON2bits.ACKDT = (ack != 0u) ? 0u : 1u;
    SSPCON2bits.ACKEN = 1u;
    while (SSPCON2bits.ACKEN != 0u)
    {
    }

    return value;
}

uint8_t i2c_write_register(uint8_t device_addr, uint8_t reg_addr, uint8_t data)
{
    i2c_start();
    if (i2c_write_byte((uint8_t)(device_addr << 1u)) != 0u) { i2c_stop(); return 1u; }
    if (i2c_write_byte(reg_addr) != 0u) { i2c_stop(); return 1u; }
    if (i2c_write_byte(data) != 0u) { i2c_stop(); return 1u; }
    i2c_stop();
    return 0u;
}

uint8_t i2c_read_register(uint8_t device_addr, uint8_t reg_addr, uint8_t* data)
{
    if (data == (uint8_t*)0)
    {
        return 1u;
    }

    i2c_start();
    if (i2c_write_byte((uint8_t)(device_addr << 1u)) != 0u) { i2c_stop(); return 1u; }
    if (i2c_write_byte(reg_addr) != 0u) { i2c_stop(); return 1u; }

    i2c_restart();
    if (i2c_write_byte((uint8_t)((device_addr << 1u) | 0x01u)) != 0u) { i2c_stop(); return 1u; }

    *data = i2c_read_byte(0u);
    i2c_stop();
    return 0u;
}

uint8_t i2c_device_ready(uint8_t device_addr)
{
    uint8_t nack;

    i2c_start();
    nack = i2c_write_byte((uint8_t)(device_addr << 1u));
    i2c_stop();

    return (uint8_t)(nack == 0u ? 1u : 0u);
}

uint8_t i2c_scan(uint8_t* found_addresses, uint8_t max_devices)
{
    uint8_t addr;
    uint8_t count = 0u;

    for (addr = 0x08u; addr <= 0x77u; addr++)
    {
        if (i2c_device_ready(addr) != 0u)
        {
            if ((found_addresses != (uint8_t*)0) && (count < max_devices))
            {
                found_addresses[count] = addr;
            }
            if (count < 0xFFu)
            {
                count++;
            }
        }
    }

    return count;
}

#endif
