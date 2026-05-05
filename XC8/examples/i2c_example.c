#include <xc.h>
#include "core/compiler.h"
#include "drivers/i2c/i2c.h"
#include "drivers/uart/uart.h"
#include "drivers/uart_debug/uart_debug.h"

void main(void)
{
    uint8_t value = 0u;
    uint8_t count;

    uart_init(9600u);
    i2c_init(100000u);

    count = i2c_scan((uint8_t*)0, 0u);
    DBG_PRINT("XC8 I2C count=");
    DBG_PRINT_INT((int)count);
    DBG_PRINTLN("");

    if (i2c_read_register(0x50u, 0x00u, &value) == 0u)
    {
        DBG_PRINT("XC8 REG=");
        DBG_PRINT_HEX(value);
        DBG_PRINTLN("");
    }

    while (1)
    {
    }
}
