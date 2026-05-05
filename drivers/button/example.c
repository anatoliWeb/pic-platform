#include "core/compiler.h"
#include "drivers/tick/tick.h"
#include "drivers/button/button.h"
#include "drivers/uart/uart.h"
#include "drivers/uart_debug/uart_debug.h"

static button_t g_button;
static uint8_t g_led_state = 0u;
static uint8_t g_screen = 0u;

void main(void)
{
    uint32_t last_update = 0u;
    uint32_t now;

    TRISBbits.TRISB0 = 0u;
    PORTBbits.RB0 = 0u;

    uart_init(9600u);
    tick_init();

    button_init(&g_button, &PORTB, &TRISB, 1u);

    while (1)
    {
        now = tick_get();

        if ((now - last_update) >= 10u)
        {
            last_update = now;
            button_update(&g_button);

            /* One button: click toggles LED. */
            if (button_is_clicked(&g_button) != 0u)
            {
                g_led_state ^= 1u;
                PORTBbits.RB0 = g_led_state;

                /* Menu button: click switches screen. */
                g_screen = (uint8_t)((g_screen + 1u) % 3u);
                DBG_PRINT("Click -> screen: ");
                DBG_PRINT_INT(g_screen);
                DBG_PRINT("\r\n");
            }

            /* Menu button: hold enters menu mode. */
            if (button_is_held(&g_button) != 0u)
            {
                DBG_PRINT("Hold -> enter menu\r\n");
            }
        }
    }
}
