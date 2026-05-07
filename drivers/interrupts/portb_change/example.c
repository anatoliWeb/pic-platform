#include "core/compiler.h"
#include "drivers/interrupts/portb_change/portb_change.h"

static volatile uint8_t g_btn_event = 0u;

static void on_portb_change(uint8_t changed_mask, uint8_t current_state)
{
    g_btn_event = changed_mask;
    DRV_UNUSED(current_state);
}

void main(void)
{
    portb_change_init();
    portb_change_set_callback(on_portb_change);
    portb_change_enable();

    while (1)
    {
        if (g_btn_event != 0u)
        {
            /* Buttons/keypad logic belongs to project layer. */
            g_btn_event = 0u;
        }
    }
}

/*
 * ISR integration note:
 * Call portb_change_irq_handler() inside your real MCU ISR.
 */
