#include "drivers/rs485/rs485.h"

#if defined(DRV_COMPILER_C18)
    #include "../../C18/drivers/rs485/rs485.c"
#elif defined(DRV_COMPILER_XC8)
    #include "../../XC8/drivers/rs485/rs485.c"
#else

#include "core/delay.h"
#include "drivers/gpio/gpio.h"
#include "drivers/uart/uart.h"

#define RS485_START_BYTE       0xAAu
#define RS485_TIMEOUT_TICKS    200u

static volatile uint8_t* rs485_dir_port = (volatile uint8_t*)0;
static volatile uint8_t* rs485_dir_tris = (volatile uint8_t*)0;
static uint8_t rs485_dir_pin = 0u;

void rs485_init(volatile uint8_t* dir_port, volatile uint8_t* dir_tris, uint8_t dir_pin)
{
    rs485_dir_port = dir_port;
    rs485_dir_tris = dir_tris;
    rs485_dir_pin = dir_pin;

    gpio_set_output(rs485_dir_tris, rs485_dir_pin);
    rs485_set_rx();
}

void rs485_set_tx(void)
{
    gpio_write_high(rs485_dir_port, rs485_dir_pin);
}

void rs485_set_rx(void)
{
    gpio_write_low(rs485_dir_port, rs485_dir_pin);
}

void rs485_send_byte(uint8_t data)
{
    uart_write_byte(data);
}

uint8_t rs485_read_byte(void)
{
    return uart_read_byte();
}

uint8_t rs485_crc8(uint8_t* data, uint8_t len)
{
    uint8_t crc = 0x00u;
    uint8_t i;
    uint8_t j;

    for (i = 0u; i < len; i++)
    {
        crc ^= data[i];
        for (j = 0u; j < 8u; j++)
        {
            if ((crc & 0x80u) != 0u)
            {
                crc = (uint8_t)((crc << 1u) ^ 0x07u);
            }
            else
            {
                crc <<= 1u;
            }
        }
    }

    return crc;
}

uint8_t rs485_send_frame(uint8_t* data, uint8_t len)
{
    uint8_t crc;
    uint8_t i;

    if ((data == (uint8_t*)0) || (len == 0u))
    {
        return 0u;
    }

    crc = rs485_crc8(data, len);

    rs485_set_tx();
    DRV_DELAY_US(50);

    rs485_send_byte(RS485_START_BYTE);
    rs485_send_byte(len);
    for (i = 0u; i < len; i++)
    {
        rs485_send_byte(data[i]);
    }
    rs485_send_byte(crc);

    DRV_DELAY_US(50);
    rs485_set_rx();

    return 1u;
}

uint8_t rs485_receive_frame(uint8_t* buffer, uint8_t max_len)
{
    uint8_t timeout = RS485_TIMEOUT_TICKS;
    uint8_t start;
    uint8_t len;
    uint8_t i;
    uint8_t recv_crc;
    uint8_t calc_crc;

    if ((buffer == (uint8_t*)0) || (max_len == 0u))
    {
        return 0u;
    }

    rs485_set_rx();

    while (timeout > 0u)
    {
        if (uart_is_data_ready() != 0u)
        {
            start = rs485_read_byte();
            if (start == RS485_START_BYTE)
            {
                break;
            }
        }

        DRV_DELAY_MS(1);
        timeout--;
    }

    if (timeout == 0u)
    {
        return 0u;
    }

    timeout = RS485_TIMEOUT_TICKS;
    while ((uart_is_data_ready() == 0u) && (timeout > 0u))
    {
        DRV_DELAY_MS(1);
        timeout--;
    }
    if (timeout == 0u)
    {
        return 0u;
    }

    len = rs485_read_byte();
    if ((len == 0u) || (len > max_len))
    {
        return 0u;
    }

    for (i = 0u; i < len; i++)
    {
        timeout = RS485_TIMEOUT_TICKS;
        while ((uart_is_data_ready() == 0u) && (timeout > 0u))
        {
            DRV_DELAY_MS(1);
            timeout--;
        }

        if (timeout == 0u)
        {
            return 0u;
        }

        buffer[i] = rs485_read_byte();
    }

    timeout = RS485_TIMEOUT_TICKS;
    while ((uart_is_data_ready() == 0u) && (timeout > 0u))
    {
        DRV_DELAY_MS(1);
        timeout--;
    }
    if (timeout == 0u)
    {
        return 0u;
    }

    recv_crc = rs485_read_byte();
    calc_crc = rs485_crc8(buffer, len);

    if (recv_crc != calc_crc)
    {
        return 0u;
    }

    return len;
}

#endif
