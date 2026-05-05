#include <xc.h>
#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/gpio/gpio.h"

#define LED_BIT     0u
#define BUTTON_BIT  1u

void main(void)
{
    /* Configure LED pin as output and button pin as input. */
    gpio_init();
    gpio_set_output(&TRISB, LED_BIT);
    gpio_set_input(&TRISB, BUTTON_BIT);

    while (1)
    {
        /* Toggle LED periodically. */
        gpio_toggle(&PORTB, LED_BIT);

        /* Drive LED based on button level. */
        if (gpio_read(&PORTB, BUTTON_BIT) != 0u)
        {
            gpio_write_high(&PORTB, LED_BIT);
        }
        else
        {
            gpio_write_low(&PORTB, LED_BIT);
        }

        DRV_DELAY_MS(100);
    }
}
