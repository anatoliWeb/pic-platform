#include "core/compiler.h"
#include "drivers/system/clock/clock.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"

void main(void)
{
    uart_init(9600u);

    DBG_PRINTLN("Clock driver example");

    DBG_PRINT("Fosc=");
    DBG_PRINT_INT((int)clock_get_frequency_khz());
    DBG_PRINTLN(" kHz");

    DBG_PRINT("Instruction clock=");
    DBG_PRINT_INT((int)clock_get_instruction_frequency_khz());
    DBG_PRINTLN(" kHz");

    DBG_PRINT("Legacy clock_get_frequency=");
    DBG_PRINT_INT((int)(clock_get_frequency() / 1000UL));
    DBG_PRINTLN(" kHz");

    while (1)
    {
    }
}