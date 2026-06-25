/*
 * File: drivers/system/reset/example.c
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

    switch (cause)
    {
        case RESET_CAUSE_POWER_ON:
            DBG_PRINT("Power-on reset\r\n");
            break;
        case RESET_CAUSE_WATCHDOG:
            DBG_PRINT("Watchdog reset\r\n");
            break;
        case RESET_CAUSE_BROWN_OUT:
            DBG_PRINT("Brown-out reset\r\n");
            break;
        case RESET_CAUSE_EXTERNAL:
            DBG_PRINT("External reset\r\n");
            break;
        default:
            DBG_PRINT("Unknown reset\r\n");
            break;
    }

    reset_clear_flags();

    while (1)
    {
    }
}
