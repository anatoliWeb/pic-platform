#include "core/compiler.h"
#include "drivers/analog/adc/adc.h"
#include "drivers/timers/tick/tick.h"
#include "libraries/input/adc_buttons/adc_buttons.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"

static const adc_button_range_t g_ladder_table[] =
{
    {0u,   100u, 1u},
    {150u, 250u, 2u},
    {300u, 400u, 3u},
    {450u, 550u, 4u}
};

static adc_buttons_t g_btns;
static uint8_t g_menu_index = 0u;

void main(void)
{
    uint32_t last_update = 0u;
    uint32_t now;
    uint8_t current;

    uart_init(9600u);
    adc_init();
    tick_init();

    adc_buttons_init(&g_btns, 0u, g_ladder_table, 4u);

    while (1)
    {
        now = tick_get();

        if ((now - last_update) >= 10u)
        {
            last_update = now;
            adc_buttons_update(&g_btns);

            current = adc_buttons_get(&g_btns);

            if (adc_buttons_is_clicked(&g_btns) != 0u)
            {
                if (current == 1u)
                {
                    g_menu_index++;
                    DBG_PRINT("Next\r\n");
                }
                else if (current == 2u)
                {
                    if (g_menu_index > 0u)
                    {
                        g_menu_index--;
                    }
                    DBG_PRINT("Prev\r\n");
                }
                else if (current == 3u)
                {
                    DBG_PRINT("Select\r\n");
                }
                else if (current == 4u)
                {
                    DBG_PRINT("Button4 click\r\n");
                }
            }

            if (adc_buttons_is_held(&g_btns) != 0u)
            {
                DBG_PRINT("Hold -> enter menu\r\n");
            }
        }
    }
}
