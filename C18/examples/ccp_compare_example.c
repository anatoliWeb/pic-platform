/*
 * File: C18/examples/ccp_compare_example.c
 */

#include "core/compiler.h"
#include "drivers/gpio/gpio.h"
#include "drivers/timers/ccp_compare/ccp_compare.h"

static void compare_cb(void)
{
    gpio_toggle(&PORTB, 0u);
}

void main(void)
{
    gpio_set_output(&TRISB, 0u);

    ccp_compare_init();
    ccp_compare_set_value(0x3000u);
    ccp_compare_set_callback(compare_cb);
    ccp_compare_enable();

    while (1)
    {
    }
}

/* C18 ISR hook example:
void high_isr(void)
{
    ccp_compare_irq_handler();
}
*/

