/*
 * File: XC8/examples/reset_example.c
 */

#include "core/compiler.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"
#include "drivers/system/reset/reset.h"

void main(void)
{
    reset_cause_t cause;

    uart_init(9600u);
    reset_init();
    cause = reset_get_cause();

    DBG_PRINT("XC8 Reset cause=");
    DBG_PRINT_INT((int)cause);
    DBG_PRINTLN("");

    reset_clear_flags();

    while (1)
    {
    }
}

