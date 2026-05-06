#include "core/compiler.h"
#include "drivers/portb_change/portb_change.h"

static volatile uint8_t g_changed = 0u;

static void portb_cb(uint8_t changed_mask, uint8_t current_state)
{
    g_changed = changed_mask;
    DRV_UNUSED(current_state);
}

void main(void)
{
    portb_change_init();
    portb_change_set_callback(portb_cb);
    portb_change_enable();

    while (1)
    {
        if (g_changed != 0u)
        {
            g_changed = 0u;
        }
    }
}

/* C18 ISR hook example:
void high_isr(void)
{
    portb_change_irq_handler();
}
*/

