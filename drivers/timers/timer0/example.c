#include "core/compiler.h"
#include "drivers/gpio/gpio.h"
#include "drivers/timers/timer0/timer0.h"

static void timer0_led_toggle_cb(void)
{
    gpio_toggle(&PORTB, 0u);
}

void main(void)
{
    gpio_set_output(&TRISB, 0u);

    timer0_init(256u);
    timer0_set_callback(timer0_led_toggle_cb);
    timer0_enable_interrupt();
    timer0_start();

    while (1)
    {
        /* Main loop is free while LED toggles in timer callback. */
    }
}
