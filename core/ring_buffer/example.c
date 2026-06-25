/*
 * File: core/ring_buffer/example.c
 */

#include "core/compiler.h"
#include "core/ring_buffer/ring_buffer.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"

static ring_buffer_t g_rx_rb;
static uint8_t g_rx_storage[64];

/* ISR hook example: call this from UART RX ISR with received byte. */
void uart_rx_isr_handler(uint8_t received_byte)
{
    (void)rb_push(&g_rx_rb, received_byte);
}

void main(void)
{
    uint8_t data;

    uart_init(9600u);
    rb_init(&g_rx_rb, g_rx_storage, sizeof(g_rx_storage));

    while (1)
    {
        while (rb_pop(&g_rx_rb, &data) != 0u)
        {
            /* Process byte from FIFO (example: echo/debug). */
            DBG_WRITE_BYTE(data);
        }
    }
}
