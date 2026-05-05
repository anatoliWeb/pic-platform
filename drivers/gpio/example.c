#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/gpio/gpio.h"

#define LED_BIT     0u
#define BUTTON_BIT  1u

static void gpio_example_blink(volatile uint8_t* led_port)
{
    /* Blink by toggling LED state with fixed delay. */
    gpio_toggle(led_port, LED_BIT);
    DRV_DELAY_MS(200);
}

static void gpio_example_button_to_led(volatile uint8_t* button_port, volatile uint8_t* led_port)
{
    /* Mirror button level to LED output. */
    if (gpio_read(button_port, BUTTON_BIT) != 0u)
    {
        gpio_write_high(led_port, LED_BIT);
    }
    else
    {
        gpio_write_low(led_port, LED_BIT);
    }
}

static void gpio_example_led_control(volatile uint8_t* led_port)
{
    /* Explicit high/low control example. */
    gpio_write_high(led_port, LED_BIT);
    DRV_DELAY_MS(100);
    gpio_write_low(led_port, LED_BIT);
    DRV_DELAY_MS(100);
}

void main(void)
{
    /* Register pointers keep API generic and pin-independent. */
    volatile uint8_t* led_tris = &TRISB;
    volatile uint8_t* led_port = &PORTB;
    volatile uint8_t* button_tris = &TRISB;
    volatile uint8_t* button_port = &PORTB;

    gpio_init();
    gpio_set_output(led_tris, LED_BIT);
    gpio_set_input(button_tris, BUTTON_BIT);

    while (1)
    {
        /* 1) Toggle pin, 2) read input, 3) explicit LED control. */
        gpio_example_blink(led_port);
        gpio_example_button_to_led(button_port, led_port);
        gpio_example_led_control(led_port);
    }
}
