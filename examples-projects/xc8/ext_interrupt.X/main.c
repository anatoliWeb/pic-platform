#include "project_config.h"

#include "core/compiler.h"
#include "drivers/gpio/gpio.h"
#include "drivers/interrupts/ext_interrupt/ext_interrupt.h"

#define INT0_BUTTON_PIN  0u
#define LED_PIN          1u

static volatile uint8_t g_int0_flag = 0u;

static void on_int0(void)
{
    g_int0_flag = 1u;

    /*
     * Toggle LED on RB1.
     * RB0 is INT0 input and must not be used as LED output.
     */
    gpio_toggle(&PORTB, LED_PIN);
}

void __interrupt() isr(void)
{
    ext_interrupt_irq_handler();
}

void main(void)
{
    /*
     * RB0 / INT0 / pin 33: button input.
     * RB1 / pin 34: LED output.
     */
    gpio_set_input(&TRISB, INT0_BUTTON_PIN);
    gpio_set_output(&TRISB, LED_PIN);
    gpio_write_low(&PORTB, LED_PIN);

    ext_interrupt_init();
    ext_interrupt_set_edge(EXT_INT0, EXT_INT_EDGE_FALLING);
    ext_interrupt_set_callback(EXT_INT0, on_int0);
    ext_interrupt_enable(EXT_INT0);

    while (1)
    {
        if (g_int0_flag != 0u)
        {
            g_int0_flag = 0u;

            /*
             * Main loop can handle non-critical work here.
             * Keep ISR short.
             */
        }
    }
}