#include "core/compiler.h"
#include "drivers/gpio/gpio.h"
#include "drivers/analog/comparator/comparator.h"

void main(void)
{
    gpio_set_output(&TRISB, 0u);
    comparator_init(CMP_MODE_1);
    comparator_enable();

    while (1)
    {
        if (comparator_get_output() != 0u)
        {
            gpio_toggle(&PORTB, 0u);
        }
        else
        {
            gpio_write_low(&PORTB, 0u);
        }
    }
}
