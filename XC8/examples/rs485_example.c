#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/uart/uart.h"
#include "drivers/rs485/rs485.h"

void main(void)
{
    uint8_t frame[2];

    uart_init(9600u);
    rs485_init(&PORTB, &TRISB, 2u);

    frame[0] = 0x01u;
    frame[1] = 0x55u;

    while (1)
    {
        rs485_send_frame(frame, 2u);
        DRV_DELAY_MS(200);
    }
}

