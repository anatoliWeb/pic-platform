#include "core/compiler.h"
#include "drivers/uart/uart.h"
#include "drivers/uart_debug/uart_debug.h"
#include "drivers/onewire/onewire.h"

void main(void)
{
    uint8_t present;

    uart_init(9600u);
    onewire_init(&PORTB, &TRISB, 1u);

    present = onewire_reset();

    if (present != 0u)
    {
        DBG_PRINT("Device found\r\n");
    }
    else
    {
        DBG_PRINT("No device\r\n");
    }

    while (1)
    {
    }
}
