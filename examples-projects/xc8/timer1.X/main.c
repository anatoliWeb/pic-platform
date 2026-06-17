#include "project_config.h"

#include "core/compiler.h"
#include "core/types.h"

#include "drivers/timers/timer1/timer1.h"

#if defined(DRV_COMPILER_C18)
#include <p18f452.h>
#endif

#if defined(DRV_COMPILER_XC8)
#include <xc.h>
#endif

/*
 * PIC18F452 Timer1 LED-only example.
 *
 * Purpose:
 *   Test Timer1 interrupt without UART and without delay.c.
 *
 * Source Files:
 *   config_bits.c
 *   main.c
 *   ../../../drivers/timers/timer1/timer1.c
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
 *   Timer1 is a peripheral interrupt source.
 *   Therefore both PEIE and GIE must be enabled.
 *
 * Proteus:
 *   PIC Clock Frequency must be 10MHz.
 */

#define TIMER1_LED_TRIS      TRISB
#define TIMER1_LED_PORT      PORTB
#define TIMER1_LED_MASK      0x0Fu

/*
 * Software counter updated from Timer1 interrupt callback.
 *
 * volatile is required because this variable is changed inside
 * interrupt context.
 */
static volatile uint8_t g_timer1_led_counter = 0u;

/*
 * XC8 interrupt vector.
 */
#if defined(DRV_COMPILER_XC8)
void __interrupt() isr(void)
{
    /*
     * Let Timer1 driver process Timer1 interrupt.
     *
     * The driver should:
     *   - check Timer1 interrupt flag
     *   - clear Timer1 interrupt flag
     *   - reload Timer1 if needed
     *   - call registered callback
     */
    timer1_irq_handler();
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
     * Let Timer1 driver process Timer1 interrupt.
     */
    timer1_irq_handler();
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
     *
     * PIC18F452 DIP-40:
     *   RB0 / pin 33
     *   RB1 / pin 34
     *   RB2 / pin 35
     *   RB3 / pin 36
     */
    TIMER1_LED_TRIS &= (uint8_t)(~TIMER1_LED_MASK);

    /*
     * Start with all LEDs OFF.
     */
    TIMER1_LED_PORT &= (uint8_t)(~TIMER1_LED_MASK);
}

static void leds_write_nibble(uint8_t value)
{
    /*
     * Show lower 4 bits on RB0..RB3.
     *
     * Only RB0..RB3 are changed.
     * RB4..RB7 are not touched.
     */
    TIMER1_LED_PORT &= (uint8_t)(~TIMER1_LED_MASK);
    TIMER1_LED_PORT |= (uint8_t)(value & TIMER1_LED_MASK);
}

static void timer1_callback(void)
{
    /*
     * Timer1 callback.
     *
     * This function is called from Timer1 interrupt.
     * Keep it short.
     *
     * Every Timer1 interrupt increments a small counter.
     * The lower 4 bits are displayed on RB0..RB3.
     */
    g_timer1_led_counter++;

    leds_write_nibble(g_timer1_led_counter);
}

void main(void)
{
    /*
     * Initialize LEDs on RB0..RB3.
     */
    leds_init();

    /*
     * Initialize Timer1.
     *
     * Value 8u is from the original common timer example.
     */
    timer1_init(8u);

    /*
     * Register callback that will be called from Timer1 interrupt.
     */
    timer1_set_callback(timer1_callback);

    /*
     * Enable Timer1 interrupt.
     */
    timer1_enable_interrupt();

    /*
     * Start Timer1.
     */
    timer1_start();

    /*
     * Enable peripheral and global interrupts.
     *
     * Timer1 is a peripheral interrupt source.
     * Without PEIE and GIE, Timer1 interrupt will not run.
     */
    INTCONbits.PEIE = 1u;
    INTCONbits.GIE  = 1u;

    while (1)
    {
        /*
         * Main loop is intentionally empty.
         *
         * LEDs are updated from Timer1 interrupt callback.
         */
    }
}