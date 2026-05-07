#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/communication/uart/uart.h"
#include "drivers/communication/rs485/rs485.h"

static void rs485_master_send_example(void)
{
    uint8_t payload[3];

    payload[0] = 0x10u;
    payload[1] = 0x20u;
    payload[2] = 0x30u;

    rs485_send_frame(payload, 3u);
}

static void rs485_slave_receive_example(void)
{
    uint8_t buffer[16];
    uint8_t len;

    len = rs485_receive_frame(buffer, (uint8_t)sizeof(buffer));
    if (len != 0u)
    {
        /* Valid frame received: CRC check passed. */
        DRV_UNUSED(len);
    }
}

void main(void)
{
    uart_init(9600u);

    /* DE/RE on PORTB bit 2 for demonstration only. */
    rs485_init(&PORTB, &TRISB, 2u);

    while (1)
    {
        rs485_master_send_example();
        rs485_slave_receive_example();
        DRV_DELAY_MS(100);
    }
}
