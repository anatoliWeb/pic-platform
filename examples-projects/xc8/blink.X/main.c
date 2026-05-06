#include <xc.h>

#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/gpio/gpio.h"

/*
 * XC8 blink example
 *
 * MCU:
 *   PIC18F452
 *
 * Function:
 *   Toggle LED connected to RB0 every 500 ms.
 *
 * Required:
 *   - gpio.c
 *   - delay.c
 *
 * Notes:
 *   - RB0 must have LED + resistor connected
 *   - Oscillator frequency must match real hardware
 */

void main(void)
{
    /*
     * Configure RB0 pin as output
     *
     * TRISB:
     *   1 = input
     *   0 = output
     */
    gpio_set_output(&TRISB, 0);

    while (1)
    {
        /*
         * Toggle RB0 state:
         *   LOW  -> HIGH
         *   HIGH -> LOW
         */
        gpio_toggle(&PORTB, 0);

        /* Delay 500 milliseconds */
        delay_ms(500);
    }
}
