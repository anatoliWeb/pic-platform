#include "core/compiler.h"
#include "drivers/uart/uart.h"
#include "drivers/uart_debug/uart_debug.h"
#include "drivers/onewire/onewire.h"

void main(void)
{
    uint8_t present;
    uint8_t read_back;

    uart_init(9600u);
    onewire_init(&PORTB, &TRISB, 1u);

    present = onewire_reset();

    if (present != 0u)
    {
        DBG_PRINT("Device found\r\n");

        onewire_write_byte(0xA5u);
        read_back = onewire_read_byte();

        DBG_PRINT("Read byte: 0x");
        DBG_PRINT_HEX(read_back);
        DBG_PRINT("\r\n");
    }
    else
    {
        DBG_PRINT("No device\r\n");
    }

    while (1)
    {
    }
}
