#include "core/compiler.h"
#include "drivers/gpio/gpio.h"
#include "drivers/ext_interrupt/ext_interrupt.h"

static volatile uint8_t g_int0_flag = 0u;

static void on_int0(void)
{
    g_int0_flag = 1u;
    gpio_toggle(&PORTB, 0u);
}

void main(void)
{
    gpio_set_output(&TRISB, 0u);

    ext_interrupt_init();
    ext_interrupt_set_edge(EXT_INT0, EXT_INT_EDGE_FALLING);
    ext_interrupt_set_callback(EXT_INT0, on_int0);
    ext_interrupt_enable(EXT_INT0);

    while (1)
    {
        if (g_int0_flag != 0u)
        {
            g_int0_flag = 0u;
        }
    }
}

/* C18 ISR hook example:
void high_isr(void)
{
    ext_interrupt_irq_handler();
}
*/

