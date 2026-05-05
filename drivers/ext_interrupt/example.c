#include "core/compiler.h"
#include "drivers/gpio/gpio.h"
#include "drivers/ext_interrupt/ext_interrupt.h"

static volatile uint8_t g_button_event = 0u;

static void on_button_interrupt(void)
{
    g_button_event = 1u;
    gpio_toggle(&PORTB, 0u);
}

void main(void)
{
    gpio_set_output(&TRISB, 0u);

    ext_interrupt_init();
    ext_interrupt_set_edge(EXT_INT0, EXT_INT_EDGE_FALLING);
    ext_interrupt_set_callback(EXT_INT0, on_button_interrupt);
    ext_interrupt_enable(EXT_INT0);

    while (1)
    {
        if (g_button_event != 0u)
        {
            g_button_event = 0u;
        }
    }
}

/*
 * ISR integration note:
 * In your real MCU ISR, call ext_interrupt_irq_handler();
 */
