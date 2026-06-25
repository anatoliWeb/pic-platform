/*
 * File: examples-projects/c18/blink.X/main.c
 */

#include <p18f452.h>

#include "project_config.h"
#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/gpio/gpio.h"

/*
 * C18 blink example
 *
 * MCU:
 *   PIC18F452
 *
 * Function:
 *   Toggle LED connected to RB0 every 500 ms.
 *
 * Required source files:
 *   - core/delay.c
 *   - drivers/gpio/gpio.c
 *
 * Notes:
 *   - RB0 must have LED + resistor connected
 *   - This example is intended for MPLAB C18
 */

void main(void)
{
    /*
     * Configure RB0 as output
     *
     * TRIS bit:
     *   1 = input
     *   0 = output
     */
    gpio_set_output(&TRISB, 0);

    while (1)
    {
        /*
         * Toggle RB0 pin state
         */
        gpio_toggle(&PORTB, 0);

        /*
         * Delay 500 milliseconds
         */
        delay_ms(500);
    }
}
