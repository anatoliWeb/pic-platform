#include "drivers/uart/uart.h"
#include "core/device.h"

#define UART_RX_BUFFER_SIZE 32u

static volatile uint8_t uart_rx_buffer[UART_RX_BUFFER_SIZE];
static volatile uint8_t uart_rx_head = 0u;
static volatile uint8_t uart_rx_tail = 0u;

static void uart_rx_push(uint8_t data)
{
    uint8_t next_head = (uint8_t)((uart_rx_head + 1u) % UART_RX_BUFFER_SIZE);

    if (next_head == uart_rx_tail)
    {
        return;
    }

    uart_rx_buffer[uart_rx_head] = data;
    uart_rx_head = next_head;
}

static uint8_t uart_rx_pop(uint8_t* data)
{
    if (uart_rx_head == uart_rx_tail)
    {
        return 0u;
    }

    *data = uart_rx_buffer[uart_rx_tail];
    uart_rx_tail = (uint8_t)((uart_rx_tail + 1u) % UART_RX_BUFFER_SIZE);
    return 1u;
}

static void uart_recover_overrun(void)
{
    if (RCSTAbits.OERR != 0u)
    {
        RCSTAbits.CREN = 0u;
        RCSTAbits.CREN = 1u;
    }
}

static void uart_poll_fill_buffer(void)
{
    uart_recover_overrun();

    while (PIR1bits.RCIF != 0u)
    {
        uart_rx_push(RCREG);
    }
}

void uart_init(uint32_t baudrate)
{
    uint16_t brg;

    if (baudrate == 0u)
    {
        baudrate = 9600u;
    }

    TXSTAbits.SYNC = 0u;
    TXSTAbits.BRGH = 1u;

    brg = (uint16_t)((_XTAL_FREQ / (16u * baudrate)) - 1u);
    SPBRG = (uint8_t)(brg & 0xFFu);

    RCSTAbits.SPEN = 1u;
    TXSTAbits.TXEN = 1u;
    RCSTAbits.CREN = 1u;

    uart_rx_head = 0u;
    uart_rx_tail = 0u;
}

void uart_write_byte(uint8_t data)
{
    while (PIR1bits.TXIF == 0u)
    {
    }

    TXREG = data;
}

void uart_write_string(const char* str)
{
    if (str == (const char*)0)
    {
        return;
    }

    while (*str != '\0')
    {
        uart_write_byte((uint8_t)(*str));
        str++;
    }
}

uint8_t uart_read_byte(void)
{
    uint8_t data;

    uart_poll_fill_buffer();
    if (uart_rx_pop(&data) != 0u)
    {
        return data;
    }

    while (PIR1bits.RCIF == 0u)
    {
    }

    uart_recover_overrun();
    return RCREG;
}

uint8_t uart_is_data_ready(void)
{
    uart_poll_fill_buffer();
    return (uart_rx_head != uart_rx_tail) ? 1u : 0u;
}
