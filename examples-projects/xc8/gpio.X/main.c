/*
 * File: examples-projects/xc8/gpio.X/main.c
 */

#include "project_config.h"

#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/gpio/gpio.h"

#define LED_BIT     0u
#define BUTTON_BIT  1u

void main(void)
{
    /*
     * GPIO example for PIC18F452.
     *
     * RB0 / pin 33: LED output
     * RB1 / pin 34: button input
     *
     * Button wiring:
     *   +5V -> 10k pull-up -> RB1 -> button -> GND
     *
     * With this wiring:
     *   button released = HIGH
     *   button pressed  = LOW
     */

    gpio_init();

    /*
     * Configure RB0 as output for LED.
     * Configure RB1 as input for button.
     */
    gpio_set_output(&TRISB, LED_BIT);
    gpio_set_input(&TRISB, BUTTON_BIT);

    /*
     * Start with LED turned off.
     */
    gpio_write_low(&PORTB, LED_BIT);

    while (1)
    {
        /*
         * Active-low button:
         *   0 = pressed
         *   1 = released
         */
        if (gpio_read(&PORTB, BUTTON_BIT) == 0u)
        {
            gpio_write_high(&PORTB, LED_BIT);
        }
        else
        {
            gpio_write_low(&PORTB, LED_BIT);
        }

        /*
         * Small delay for simple debounce and stable Proteus behavior.
         */
        DRV_DELAY_MS(20);
    }
}