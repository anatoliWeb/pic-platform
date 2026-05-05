#include "core/compiler.h"
#include "drivers/clock/clock.h"
#include "drivers/uart/uart.h"
#include "drivers/uart_debug/uart_debug.h"

void main(void)
{
    uart_init(9600u);

    DBG_PRINT("Clock: ");
    DBG_PRINT_INT((int)clock_get_frequency());
    DBG_PRINTLN(" Hz");

    while (1)
    {
    }
}
