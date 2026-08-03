/*
 * File: examples-projects/xc8/timer0.X/main.c
 */

#include "project_config.h"

#include "core/compiler.h"
#include "core/types.h"

#include "drivers/timers/timer0/timer0.h"

#if defined(DRV_COMPILER_C18)
#include <p18f452.h>
#endif

#if defined(DRV_COMPILER_XC8)
#include <xc.h>
#endif

/*
 * PIC18F452 Timer0 LED-only example.
 *
 * Purpose:
 *   Test Timer0 interrupt without UART and without delay.c.
 *
 * Source Files:
 *   config_bits.c
 *   main.c
 *   ../../../../drivers/timers/timer0/timer0.c
 *
 * Proteus wiring:
 *   RB0 / DIP-40 pin 33 -> LED0 -> 330R -> GND
 *   RB1 / DIP-40 pin 34 -> LED1 -> 330R -> GND
 *   RB2 / DIP-40 pin 35 -> LED2 -> 330R -> GND
 *   RB3 / DIP-40 pin 36 -> LED3 -> 330R -> GND
 *
 * Expected:
 *   RB0..RB3 should work as a small binary counter.
 *
 * Important:
 *   PIC Clock Frequency in Proteus must be 10MHz.
 */

#define TIMER0_LED_TRIS      TRISB
#define TIMER0_LED_PORT      PORTB
#define TIMER0_LED_MASK      0x0Fu

/*
 * Software counter updated from Timer0 interrupt callback.
 *
 * volatile is required because this variable is changed inside
 * interrupt context and used outside normal main flow.
 */
static volatile uint8_t g_timer0_led_counter = 0u;

/*
 * XC8 interrupt vector.
 */
#if defined(DRV_COMPILER_XC8)
void __interrupt() isr(void)
{
    /*
     * Let Timer0 driver process Timer0 interrupt.
     *
     * The driver should:
     *   - check Timer0 interrupt flag
     *   - clear Timer0 interrupt flag
     *   - reload Timer0 if needed
     *   - call registered callback
     */
    timer0_irq_handler();
}
#endif

/*
 * C18 interrupt vector.
 */
#if defined(DRV_COMPILER_C18)
#pragma interrupt high_isr
void high_isr(void)
{
    /*
     * Let Timer0 driver process Timer0 interrupt.
     */
    timer0_irq_handler();
}

#pragma code high_vector = 0x08
void high_vector(void)
{
    _asm goto high_isr _endasm
}
#pragma code
#endif

static void leds_init(void)
{
    /*
     * Configure RB0..RB3 as outputs.
     */
    TIMER0_LED_TRIS &= (uint8_t)(~TIMER0_LED_MASK);

    /*
     * Start with all LEDs OFF.
     */
    TIMER0_LED_PORT &= (uint8_t)(~TIMER0_LED_MASK);
}

static void leds_write_nibble(uint8_t value)
{
    /*
     * Show lower 4 bits on RB0..RB3.
     *
     * Only RB0..RB3 are changed.
     * RB4..RB7 are not touched.
     */
    TIMER0_LED_PORT &= (uint8_t)(~TIMER0_LED_MASK);
    TIMER0_LED_PORT |= (uint8_t)(value & TIMER0_LED_MASK);
}

static void timer0_callback(void)
{
    /*
     * Timer0 callback.
     *
     * This function is called from Timer0 interrupt.
     * Keep it short.
     *
     * Every Timer0 interrupt increments a small counter.
     * The lower 4 bits are displayed on RB0..RB3.
     */
    g_timer0_led_counter++;

    leds_write_nibble(g_timer0_led_counter);
}

void main(void)
{
    /*
     * Initialize LEDs on RB0..RB3.
     */
    leds_init();

    /*
     * Initialize Timer0.
     *
     * Value 256u is from the original example.
     * If LEDs do not change, try 128u or 64u later
     * depending on how timer0_init() is implemented.
     */
    timer0_init(256u);

    /*
     * Register callback that will be called from Timer0 interrupt.
     */
    timer0_set_callback(timer0_callback);

    /*
     * Enable Timer0 interrupt.
     */
    timer0_enable_interrupt();

    /*
     * Start Timer0.
     */
    timer0_start();

    /*
     * Enable global interrupts.
     *
     * Timer0 uses INTCONbits.GIE.
     * PEIE is not required for Timer0, but it does not hurt.
     */
    INTCONbits.PEIE = 1u;
    INTCONbits.GIE  = 1u;

    while (1)
    {
        /*
         * Main loop is intentionally empty.
         *
         * LEDs are updated from Timer0 interrupt callback.
         */
    }
}