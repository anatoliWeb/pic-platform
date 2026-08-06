/*
 * File: C18/drivers/communication/rs485/rs485.c
 */

#include "drivers/communication/rs485/rs485.h"
#include "core/delay.h"
#include "drivers/gpio/gpio.h"
#include "drivers/communication/uart/uart.h"
#include "core/crc/crc.h"

#define RS485_START_BYTE              0xAAu
#define RS485_TIMEOUT_TICKS           200u
#define RS485_TX_COMPLETE_TIMEOUT     200u

static uint8_t rs485_wait_tx_complete(void)
{
    uint16_t timeout = RS485_TX_COMPLETE_TIMEOUT;

    while ((TXSTAbits.TRMT == 0u) && (timeout > 0u))
    {
        timeout--;
    }

    return (timeout > 0u) ? 1u : 0u;
}

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
    return crc8_dallas(data, len);
}

uint8_t rs485_send_frame(uint8_t* data, uint8_t len)
{
    uint16_t crc;
    uint8_t i;
    uint8_t result = 0u;

    if ((data == (uint8_t*)0) || (len == 0u))
    {
        return 0u;
    }

    crc = crc16_modbus(data, len);

    rs485_set_tx();
    DRV_DELAY_US(50u);

    rs485_send_byte(RS485_START_BYTE);
    rs485_send_byte(len);

    for (i = 0u; i < len; i++)
    {
        rs485_send_byte(data[i]);
    }

    rs485_send_byte((uint8_t)(crc & 0x00FFu));
    rs485_send_byte((uint8_t)((crc >> 8u) & 0x00FFu));

    if (rs485_wait_tx_complete() != 0u)
    {
        DRV_DELAY_US(50u);
        result = 1u;
    }

    rs485_set_rx();

    return result;
}

uint8_t rs485_receive_frame(uint8_t* buffer, uint8_t max_len)
{
    uint8_t timeout = RS485_TIMEOUT_TICKS;
    uint8_t start;
    uint8_t len;
    uint8_t i;
    uint8_t crc_low;
    uint8_t crc_high;
    uint16_t recv_crc;
    uint16_t calc_crc;

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

        DRV_DELAY_MS(1u);
        timeout--;
    }

    if (timeout == 0u)
    {
        return 0u;
    }

    timeout = RS485_TIMEOUT_TICKS;
    while ((uart_is_data_ready() == 0u) && (timeout > 0u))
    {
        DRV_DELAY_MS(1u);
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
            DRV_DELAY_MS(1u);
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
        DRV_DELAY_MS(1u);
        timeout--;
    }
    if (timeout == 0u)
    {
        return 0u;
    }
    crc_low = rs485_read_byte();

    timeout = RS485_TIMEOUT_TICKS;
    while ((uart_is_data_ready() == 0u) && (timeout > 0u))
    {
        DRV_DELAY_MS(1u);
        timeout--;
    }
    if (timeout == 0u)
    {
        return 0u;
    }
    crc_high = rs485_read_byte();

    recv_crc = (uint16_t)(((uint16_t)crc_high << 8u) | crc_low);
    calc_crc = crc16_modbus(buffer, len);

    if (recv_crc != calc_crc)
    {
        return 0u;
    }

    return len;
}
