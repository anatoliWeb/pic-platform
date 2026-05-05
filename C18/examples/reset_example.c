#include <p18f458.h>
#include "core/compiler.h"
#include "drivers/uart/uart.h"
#include "drivers/uart_debug/uart_debug.h"
#include "drivers/reset/reset.h"

void main(void)
{
    reset_cause_t cause;

    uart_init(9600u);
    reset_init();
    cause = reset_get_cause();

    DBG_PRINT("C18 Reset cause=");
    DBG_PRINT_INT((int)cause);
    DBG_PRINTLN("");

    reset_clear_flags();

    while (1)
    {
    }
}
