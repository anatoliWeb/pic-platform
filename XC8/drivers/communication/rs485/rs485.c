#include "drivers/communication/rs485/rs485.h"

#include "core/device.h"
#include "core/delay.h"
#include "drivers/gpio/gpio.h"
#include "drivers/communication/uart/uart.h"
#include "core/crc/crc.h"

#define RS485_START_BYTE       0xAAu
#define RS485_TIMEOUT_TICKS    200u

/*
 * RS485 direction control pin.
 *
 * Typical MAX485/MAX487 half-duplex wiring:
 *   DE  -> direction pin
 *   /RE -> direction pin
 *
 * Direction pin state:
 *   1 = transmit mode
 *   0 = receive mode
 */
static volatile uint8_t* rs485_dir_port = (volatile uint8_t*)0;
static volatile uint8_t* rs485_dir_tris = (volatile uint8_t*)0;
static uint8_t rs485_dir_pin = 0u;

static void rs485_wait_tx_complete(void)
{
    /*
     * Wait until UART transmit shift register is empty.
     *
     * Important:
     *   TXIF only means TXREG is empty and ready for next byte.
     *   TRMT means the last byte was fully shifted out on TX pin.
     *
     * If we switch RS485 direction before TRMT becomes 1,
     * the final byte or stop bit can be cut and receiver will see garbage.
     */
    while (TXSTAbits.TRMT == 0u)
    {
    }
}

void rs485_init(volatile uint8_t* dir_port, volatile uint8_t* dir_tris, uint8_t dir_pin)
{
    /*
     * Save direction control GPIO.
     *
     * Example for PIC18F452:
     *   rs485_init(&PORTB, &TRISB, 2u)
     *
     * This means:
     *   RB2 / pin 35 controls MAX487 DE and /RE.
     */
    rs485_dir_port = dir_port;
    rs485_dir_tris = dir_tris;
    rs485_dir_pin = dir_pin;

    /*
     * Direction pin must be output.
     */
    gpio_set_output(rs485_dir_tris, rs485_dir_pin);

    /*
     * Start in receive/idle mode.
     * This keeps RS485 bus released when we are not sending.
     */
    rs485_set_rx();
}

void rs485_set_tx(void)
{
    /*
     * Transmit mode:
     *   DE  = 1, driver enabled
     *   /RE = 1, receiver disabled if tied to the same pin
     */
    gpio_write_high(rs485_dir_port, rs485_dir_pin);
}

void rs485_set_rx(void)
{
    /*
     * Receive mode:
     *   DE  = 0, driver disabled
     *   /RE = 0, receiver enabled if tied to the same pin
     */
    gpio_write_low(rs485_dir_port, rs485_dir_pin);
}

void rs485_send_byte(uint8_t data)
{
    /*
     * Send one byte through UART.
     * RS485 direction must already be set to TX before calling this.
     */
    uart_write_byte(data);
}

uint8_t rs485_read_byte(void)
{
    /*
     * Read one byte from UART.
     * RS485 direction must be RX before receiving.
     */
    return uart_read_byte();
}

uint8_t rs485_crc8(uint8_t* data, uint8_t len)
{
    /*
     * Optional helper for protocols that need Dallas CRC8.
     */
    return crc8_dallas(data, len);
}

uint8_t rs485_send_frame(uint8_t* data, uint8_t len)
{
    uint16_t crc;
    uint8_t i;

    if ((data == (uint8_t*)0) || (len == 0u))
    {
        return 0u;
    }

    /*
     * Calculate CRC only over payload data.
     *
     * Frame format:
     *   0xAA
     *   LEN
     *   DATA[0..LEN-1]
     *   CRC_LOW
     *   CRC_HIGH
     *
     * If this frame is viewed directly in Virtual Terminal,
     * 0xAA and CRC bytes can look like random characters.
     * That is normal for a binary frame protocol.
     */
    crc = crc16_modbus(data, len);

    /*
     * Enable RS485 transmitter.
     */
    rs485_set_tx();

    /*
     * Guard delay before first byte.
     * Gives MAX487 time to enable its driver.
     */
    DRV_DELAY_US(50u);

    /*
     * Send frame header.
     */
    rs485_send_byte(RS485_START_BYTE);
    rs485_send_byte(len);

    /*
     * Send payload.
     */
    for (i = 0u; i < len; i++)
    {
        rs485_send_byte(data[i]);
    }

    /*
     * Send CRC16 Modbus, little-endian.
     */
    rs485_send_byte((uint8_t)(crc & 0x00FFu));
    rs485_send_byte((uint8_t)((crc >> 8u) & 0x00FFu));

    /*
     * Critical wait:
     * Do not disable MAX487 transmitter until the last stop bit
     * has physically left the UART TX pin.
     */
    rs485_wait_tx_complete();

    /*
     * Guard delay after final bit.
     * Helps Proteus/MAX487 avoid direction-switch glitches.
     */
    DRV_DELAY_US(50u);

    /*
     * Return RS485 transceiver to receive/idle mode.
     */
    rs485_set_rx();

    return 1u;
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

    /*
     * Make sure transceiver is in receive mode.
     */
    rs485_set_rx();

    /*
     * Wait for frame start byte.
     */
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

    /*
     * Read payload length.
     */
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

    /*
     * Read payload bytes.
     */
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

    /*
     * Read CRC low byte.
     */
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

    /*
     * Read CRC high byte.
     */
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

    /*
     * Validate CRC.
     */
    recv_crc = (uint16_t)(((uint16_t)crc_high << 8u) | crc_low);
    calc_crc = crc16_modbus(buffer, len);

    if (recv_crc != calc_crc)
    {
        return 0u;
    }

    /*
     * Return number of payload bytes received.
     */
    return len;
}