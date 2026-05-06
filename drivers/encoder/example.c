#include "core/compiler.h"
#include "drivers/encoder/encoder.h"
#include "drivers/tick/tick.h"
#include "drivers/uart/uart.h"
#include "drivers/uart_debug/uart_debug.h"

static encoder_t g_encoder;
static int16_t g_menu_index = 0;

/* Interrupt hook example:
 * Call this from INT/PORTB-change ISR when encoder A/B changes.
 */
void encoder_irq_handler(void)
{
    encoder_update(&g_encoder);
}

void main(void)
{
    uint32_t last_poll = 0u;
    int8_t step;

    uart_init(9600u);
    tick_init();

    encoder_init(&g_encoder,
                 &PORTB, &TRISB,
                 4u, 5u,
                 &PORTB, &TRISB, 6u);

    while (1)
    {
        /* Polling fallback: call every 1-5 ms. */
        if ((tick_get() - last_poll) >= 2u)
        {
            last_poll = tick_get();
            encoder_update(&g_encoder);
        }

        step = encoder_get_delta(&g_encoder);

        if (step > 0)
        {
            g_menu_index++;
            DBG_PRINT("CW -> next, menu=");
            DBG_PRINT_INT(g_menu_index);
            DBG_PRINT("\r\n");
        }
        else if (step < 0)
        {
            g_menu_index--;
            DBG_PRINT("CCW -> prev, menu=");
            DBG_PRINT_INT(g_menu_index);
            DBG_PRINT("\r\n");
        }

        if (button_is_clicked(&g_encoder.button) != 0u)
        {
            DBG_PRINT("Button click -> select\r\n");
        }

        if (button_is_held(&g_encoder.button) != 0u)
        {
            DBG_PRINT("Button hold -> enter/exit menu\r\n");
        }
    }
}
