/*
 * File: examples-projects/xc8/timer.X/main.c
 */

#include "project_config.h"

#include "core/compiler.h"
#include "core/types.h"
#include "core/delay.h"

#include "drivers/timers/timer0/timer0.h"
#include "drivers/timers/timer1/timer1.h"
#include "drivers/timers/timer2/timer2.h"
#include "drivers/timers/timer3/timer3.h"

#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"

#if defined(DRV_COMPILER_C18)
#include <p18f452.h>
#endif

#if defined(DRV_COMPILER_XC8)
#include <xc.h>
#endif

/*
 * PIC18F452 timer example.
 *
 * Purpose:
 *   Test Timer0, Timer1, Timer2 and Timer3 interrupts.
 *
 * This example does NOT use tick_init().
 *
 * Why:
 *   The tick driver uses Timer1 internally.
 *   If tick_init() and timer1_init() are used together,
 *   Timer1 configuration may be overwritten.
 *
 * What this example does:
 *   Timer0 interrupt -> toggles RB0 LED and increments counter
 *   Timer1 interrupt -> toggles RB1 LED and increments counter
 *   Timer2 interrupt -> toggles RB2 LED and increments counter
 *   Timer3 interrupt -> toggles RB3 LED and increments counter
 *
 * Proteus wiring:
 *   RB0 / pin 33 -> Timer0 LED -> 330R -> GND
 *   RB1 / pin 34 -> Timer1 LED -> 330R -> GND
 *   RB2 / pin 35 -> Timer2 LED -> 330R -> GND
 *   RB3 / pin 36 -> Timer3 LED -> 330R -> GND
 *
 * UART debug:
 *   RC6/TX / pin 25 -> Virtual Terminal RXD
 *   Baud: 9600
 *   Mode: 8N1
 *
 * Important Proteus setting:
 *   PIC18F452 Clock Frequency must be 10MHz.
 */

#define TIMER_LED_TRIS      TRISB
#define TIMER_LED_PORT      PORTB

#define TIMER0_LED_BIT      0u
#define TIMER1_LED_BIT      1u
#define TIMER2_LED_BIT      2u
#define TIMER3_LED_BIT      3u

/*
 * Timer interrupt counters.
 *
 * volatile is required because these variables are changed inside
 * interrupt callbacks and read from the main loop.
 */
static volatile uint32_t g_timer0_count = 0UL;
static volatile uint32_t g_timer1_count = 0UL;
static volatile uint32_t g_timer2_count = 0UL;
static volatile uint32_t g_timer3_count = 0UL;

/*
 * XC8 interrupt vector.
 *
 * Each timer driver should check its own interrupt flag internally.
 * Calling all handlers from the common ISR is OK for this test.
 */
#if defined(DRV_COMPILER_XC8)
void __interrupt() isr(void)
{
    /*
     * Timer0 interrupt handler.
     */
    timer0_irq_handler();

    /*
     * Timer1 interrupt handler.
     */
    timer1_irq_handler();

    /*
     * Timer2 interrupt handler.
     */
    timer2_irq_handler();

    /*
     * Timer3 interrupt handler.
     */
    timer3_irq_handler();
}
#endif

/*
 * C18 interrupt vector.
 */
#if defined(DRV_COMPILER_C18)
#pragma interrupt high_isr
void high_isr(void)
{
    timer0_irq_handler();
    timer1_irq_handler();
    timer2_irq_handler();
    timer3_irq_handler();
}

#pragma code high_vector = 0x08
void high_vector(void)
{
    _asm goto high_isr _endasm
}
#pragma code
#endif

static void led_init(void)
{
    /*
     * Configure RB0..RB3 as outputs.
     *
     * PIC18F452 DIP-40:
     *   RB0 / pin 33 -> Timer0 LED
     *   RB1 / pin 34 -> Timer1 LED
     *   RB2 / pin 35 -> Timer2 LED
     *   RB3 / pin 36 -> Timer3 LED
     */
    TIMER_LED_TRIS &= (uint8_t)(~0x0Fu);

    /*
     * Start with all LEDs OFF.
     *
     * Wiring:
     *   RBx -> 330R -> LED -> GND
     *
     * RBx = 1 -> LED ON
     * RBx = 0 -> LED OFF
     */
    TIMER_LED_PORT &= (uint8_t)(~0x0Fu);
}

static void led_toggle(uint8_t bit)
{
    /*
     * Toggle one LED.
     */
    TIMER_LED_PORT ^= (uint8_t)(1u << bit);
}

static void print_uint32(uint32_t value)
{
    /*
     * Print unsigned 32-bit integer as decimal text.
     *
     * uart_debug does not provide DBG_PRINT_UINT32(),
     * so this helper prints uint32_t directly through UART.
     */
    char buffer[11];
    uint8_t index = 0u;
    uint8_t i;

    if (value == 0UL)
    {
        uart_write_byte((uint8_t)'0');
        return;
    }

    while ((value > 0UL) && (index < sizeof(buffer)))
    {
        buffer[index] = (char)('0' + (value % 10UL));
        value /= 10UL;
        index++;
    }

    for (i = index; i > 0u; i--)
    {
        uart_write_byte((uint8_t)buffer[i - 1u]);
    }
}

static void timer0_callback(void)
{
    /*
     * Timer0 callback.
     *
     * Keep interrupt callbacks short.
     * Do not print from interrupt.
     */
    g_timer0_count++;
    led_toggle(TIMER0_LED_BIT);
}

static void timer1_callback(void)
{
    /*
     * Timer1 callback.
     */
    g_timer1_count++;
    led_toggle(TIMER1_LED_BIT);
}

static void timer2_callback(void)
{
    /*
     * Timer2 callback.
     */
    g_timer2_count++;
    led_toggle(TIMER2_LED_BIT);
}

static void timer3_callback(void)
{
    /*
     * Timer3 callback.
     */
    g_timer3_count++;
    led_toggle(TIMER3_LED_BIT);
}

static void print_header(void)
{
    DBG_PRINTLN("");
    DBG_PRINTLN("PIC18F452 timer example");
    DBG_PRINTLN("");
    DBG_PRINTLN("Purpose:");
    DBG_PRINTLN("  Test Timer0, Timer1, Timer2 and Timer3 interrupts.");
    DBG_PRINTLN("");
    DBG_PRINTLN("Proteus wiring:");
    DBG_PRINTLN("  RB0 pin 33 -> Timer0 LED -> 330R -> GND");
    DBG_PRINTLN("  RB1 pin 34 -> Timer1 LED -> 330R -> GND");
    DBG_PRINTLN("  RB2 pin 35 -> Timer2 LED -> 330R -> GND");
    DBG_PRINTLN("  RB3 pin 36 -> Timer3 LED -> 330R -> GND");
    DBG_PRINTLN("  RC6/TX pin 25 -> Virtual Terminal RXD");
    DBG_PRINTLN("");
    DBG_PRINTLN("Expected:");
    DBG_PRINTLN("  LEDs should toggle from timer interrupts.");
    DBG_PRINTLN("  Terminal prints interrupt counters every second.");
    DBG_PRINTLN("");
}

static void print_counters(void)
{
    /*
     * Print timer interrupt counters.
     *
     * If a counter increases, that timer interrupt is working.
     */
    DBG_PRINT("T0=");
    print_uint32(g_timer0_count);

    DBG_PRINT(" T1=");
    print_uint32(g_timer1_count);

    DBG_PRINT(" T2=");
    print_uint32(g_timer2_count);

    DBG_PRINT(" T3=");
    print_uint32(g_timer3_count);

    DBG_PRINTLN("");
}

void main(void)
{
    /*
     * Initialize UART first.
     *
     * If this text is unreadable in Proteus, check:
     *   - Virtual Terminal baud = 9600
     *   - PIC Clock Frequency = 10MHz
     *   - RC6/TX pin 25 -> Virtual Terminal RXD
     */
    uart_init(9600u);

    /*
     * Initialize LEDs on RB0..RB3.
     */
    led_init();

    /*
     * Print example information before timers start.
     */
    print_header();

    /*
     * Initialize timers.
     *
     * The argument meaning depends on the driver implementation.
     * These are the same values from the original example.
     */
    timer0_init(256u);
    timer1_init(8u);
    timer2_init(16u);
    timer3_init(8u);

    /*
     * Register interrupt callbacks.
     */
    timer0_set_callback(timer0_callback);
    timer1_set_callback(timer1_callback);
    timer2_set_callback(timer2_callback);
    timer3_set_callback(timer3_callback);

    /*
     * Enable timer interrupts.
     */
    timer0_enable_interrupt();
    timer1_enable_interrupt();
    timer2_enable_interrupt();
    timer3_enable_interrupt();

    /*
     * Start timers.
     */
    timer0_start();
    timer1_start();
    timer2_start();
    timer3_start();

    /*
     * Enable interrupt system.
     *
     * Timer0 needs global interrupts.
     * Timer1, Timer2 and Timer3 are peripheral interrupts,
     * so PEIE must also be enabled.
     */
    INTCONbits.PEIE = 1u;
    INTCONbits.GIE  = 1u;

    DBG_PRINTLN("Timers started.");
    DBG_PRINTLN("");

    while (1)
    {
        /*
         * Print counters once per second.
         *
         * We intentionally use DRV_DELAY_MS() here,
         * not tick_delay(), because tick uses Timer1 internally.
         */
        DRV_DELAY_MS(1000u);

        print_counters();
    }
}