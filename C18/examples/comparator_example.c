#include <p18f458.h>
#include "core/compiler.h"
#include "drivers/gpio/gpio.h"
#include "drivers/comparator/comparator.h"
#include "drivers/uart_debug/uart_debug.h"

void main(void)
{
    gpio_set_output(&TRISB, 0u);
    comparator_init(CMP_MODE_1);
    comparator_enable();

    while (1)
    {
        uint8_t out = comparator_get_output();

        if (out != 0u)
        {
            gpio_write_high(&PORTB, 0u);
        }
        else
        {
            gpio_write_low(&PORTB, 0u);
        }

        DBG_PRINT("CMP=");
        DBG_PRINT_INT((int)out);
        DBG_PRINTLN("");
    }
}
