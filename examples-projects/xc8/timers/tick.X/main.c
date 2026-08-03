/*
 * File: examples-projects/xc8/tick.X/main.c
 */

#include "project_config.h"

#include "core/compiler.h"
#include "core/types.h"
#include "core/delay.h"

#include "drivers/timers/tick/tick.h"
#include "drivers/timers/timer1/timer1.h"

#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"

#if defined(DRV_COMPILER_C18)
#include <p18f452.h>
#endif

#if defined(DRV_COMPILER_XC8)
#include <xc.h>
#endif

/*
 * Tick example for PIC18F452.
 *
 * Purpose:
 *   Test the millisecond system tick.
 *
 * This example prints the real tick time in milliseconds.
 *
 * What this example checks:
 *   - tick_init()
 *   - tick_get()
 *   - tick_delay()
 *   - Timer1 interrupt flow
 *
 * Important:
 *   The tick driver uses Timer1 as its time base.
 *   Timer1 interrupt must be passed to timer1_irq_handler().
 *
 * Proteus wiring:
 *   RB0 / DIP-40 pin 33 -> resistor 330R -> LED -> GND
 *   RC6/TX / DIP-40 pin 25 -> Virtual Terminal RXD
 *
 * Virtual Terminal:
 *   9600 baud
 *   8N1
 */

#define TICK_LED_TRIS      TRISB
#define TICK_LED_PORT      PORTB
#define TICK_LED_BIT       0u

#define LED_TOGGLE_MS      500UL
#define PRINT_PERIOD_MS    1000UL

/*
 * XC8 interrupt vector.
 *
 * Timer1 interrupt is handled inside timer1_irq_handler().
 * tick.c registers its own callback through Timer1 driver.
 */
#if defined(DRV_COMPILER_XC8)
void __interrupt() isr(void)
{
    /*
     * Let Timer1 driver check and process Timer1 interrupt flag.
     *
     * Without this call:
     *   tick_get() will not increase
     *   tick_delay() will wait forever
     */
    timer1_irq_handler();
}
#endif

/*
 * C18 interrupt vector.
 *
 * Kept here so this example can also be adapted for C18.
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

static void led_init(void)
{
    /*
     * Configure RB0 as output.
     *
     * PIC18F452:
     *   RB0 / DIP-40 pin 33
     */
    TICK_LED_TRIS &= (uint8_t)(~(1u << TICK_LED_BIT));

    /*
     * Start with LED OFF.
     *
     * Wiring:
     *   RB0 -> 330R -> LED -> GND
     *
     * RB0 = 1 -> LED ON
     * RB0 = 0 -> LED OFF
     */
    TICK_LED_PORT &= (uint8_t)(~(1u << TICK_LED_BIT));
}

static void led_toggle(void)
{
    /*
     * Toggle LED on RB0.
     */
    TICK_LED_PORT ^= (uint8_t)(1u << TICK_LED_BIT);
}

static void print_uint32(uint32_t value)
{
    /*
     * Print unsigned 32-bit integer as decimal text.
     *
     * We use this helper because uart_debug does not provide
     * DBG_PRINT_UINT32().
     *
     * Example:
     *   value = 12345
     *   output = "12345"
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

    /*
     * Digits were stored in reverse order.
     * Print them back from the end to the beginning.
     */
    for (i = index; i > 0u; i--)
    {
        uart_write_byte((uint8_t)buffer[i - 1u]);
    }
}

static void print_header(void)
{
    /*
     * Print example description to Virtual Terminal.
     */
    DBG_PRINTLN("");
    DBG_PRINTLN("PIC18F452 tick example");
    DBG_PRINTLN("");
    DBG_PRINTLN("Purpose:");
    DBG_PRINTLN("  Test millisecond system tick.");
    DBG_PRINTLN("");
    DBG_PRINTLN("Proteus wiring:");
    DBG_PRINTLN("  RB0 pin 33 -> LED -> 330R -> GND");
    DBG_PRINTLN("  RC6/TX pin 25 -> Virtual Terminal RXD");
    DBG_PRINTLN("");
    DBG_PRINTLN("Expected:");
    DBG_PRINTLN("  LED toggles every 500 ms.");
    DBG_PRINTLN("  Terminal prints current time every 1000 ms.");
    DBG_PRINTLN("");
}

static void print_tick_time(uint32_t value)
{
    /*
     * Print current tick time.
     *
     * tick_get() returns milliseconds from tick_init().
     */
    DBG_PRINT("time = ");
    print_uint32(value);
    DBG_PRINTLN(" ms");
}

void main(void)
{
    uint32_t now;
    uint32_t last_led_tick;
    uint32_t last_print_tick;

    /*
     * Initialize UART for debug output.
     *
     * PIC18F452:
     *   RC6/TX / DIP-40 pin 25 -> Virtual Terminal RXD
     *
     * Settings:
     *   9600 baud
     *   8N1
     */
    uart_init(9600u);

    /*
     * Initialize LED output on RB0.
     */
    led_init();

    /*
     * Initialize millisecond tick.
     *
     * Internally this driver uses Timer1.
     * The ISR above is required for tick counter updates.
     */
    tick_init();

    /*
     * Important for PIC18F452:
     *
     * Timer1 interrupt is a peripheral interrupt.
     * Therefore both PEIE and GIE must be enabled.
     *
     * If these bits are not enabled:
     *   Timer1 interrupt will not enter ISR
     *   tick_get() will not increase
     *   tick_delay() will wait forever
     */
    INTCONbits.PEIE = 1u;
    INTCONbits.GIE  = 1u;

    /*
     * Print startup information.
     */
    print_header();

    /*
     * Test blocking tick delay.
     *
     * If the Timer1 interrupt works, this delay finishes after
     * about 1000 ms and the next line is printed.
     */
    DBG_PRINTLN("Testing tick_delay(1000)...");
    tick_delay(1000UL);
    DBG_PRINTLN("tick_delay() OK");
    DBG_PRINTLN("");

    /*
     * Initialize timestamps for non-blocking periodic tasks.
     */
    last_led_tick = tick_get();
    last_print_tick = tick_get();

    while (1)
    {
        /*
         * Read current millisecond tick.
         */
        now = tick_get();

        /*
         * Toggle LED every 500 ms without blocking the main loop.
         */
        if ((now - last_led_tick) >= LED_TOGGLE_MS)
        {
            last_led_tick = now;
            led_toggle();
        }

        /*
         * Print current time every 1000 ms.
         */
        if ((now - last_print_tick) >= PRINT_PERIOD_MS)
        {
            last_print_tick = now;
            print_tick_time(now);
        }
    }
}