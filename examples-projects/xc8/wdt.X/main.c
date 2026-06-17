#include "project_config.h"

#include "core/compiler.h"
#include "core/types.h"
#include "core/delay.h"

#include "drivers/system/wdt/wdt.h"

#if defined(DRV_COMPILER_C18)
#include <p18f452.h>
#endif

#if defined(DRV_COMPILER_XC8)
#include <xc.h>
#endif

/*
 * PIC18F452 WDT example.
 *
 * Purpose:
 *   Test the Watchdog Timer driver.
 *
 * What this example does:
 *   1. RB0 LED blinks while the main loop is alive.
 *   2. wdt_clear() is called regularly during normal work.
 *   3. If RB1 button is pressed, the code stops calling wdt_clear().
 *   4. Watchdog Timer should reset the MCU.
 *   5. After reset, the program starts again and RB0 LED blinks again.
 *
 * Proteus wiring:
 *   RB0 / DIP-40 pin 33 -> 330R -> LED -> GND
 *   RB1 / DIP-40 pin 34 -> button -> GND
 *   RB1 / DIP-40 pin 34 -> 10k pull-up -> +5V
 *
 * Button logic:
 *   released = HIGH
 *   pressed  = LOW
 *
 * Important:
 *   WDT must be enabled in config_bits.c.
 *
 * Example:
 *   #pragma config WDT = ON
 *
 * If WDT is disabled in config bits, the MCU will not reset
 * even if wdt_clear() is not called.
 */

#define WDT_LED_TRIS        TRISB
#define WDT_LED_PORT        PORTB
#define WDT_LED_BIT         0u

#define WDT_BUTTON_TRIS     TRISB
#define WDT_BUTTON_PORT     PORTB
#define WDT_BUTTON_BIT      1u

static void led_init(void)
{
    /*
     * Configure RB0 as output.
     *
     * PIC18F452:
     *   RB0 / DIP-40 pin 33
     */
    WDT_LED_TRIS &= (uint8_t)(~(1u << WDT_LED_BIT));

    /*
     * Start with LED OFF.
     */
    WDT_LED_PORT &= (uint8_t)(~(1u << WDT_LED_BIT));
}

static void led_toggle(void)
{
    /*
     * Toggle RB0 LED.
     *
     * Wiring:
     *   RB0 -> 330R -> LED -> GND
     *
     * RB0 = 1 -> LED ON
     * RB0 = 0 -> LED OFF
     */
    WDT_LED_PORT ^= (uint8_t)(1u << WDT_LED_BIT);
}

static void button_init(void)
{
    /*
     * Configure RB1 as input.
     *
     * Proteus wiring:
     *   +5V -> 10k pull-up -> RB1 -> button -> GND
     */
    WDT_BUTTON_TRIS |= (uint8_t)(1u << WDT_BUTTON_BIT);
}

static uint8_t button_is_pressed(void)
{
    /*
     * Active-low button.
     *
     * released = 1
     * pressed  = 0
     */
    if ((WDT_BUTTON_PORT & (uint8_t)(1u << WDT_BUTTON_BIT)) == 0u)
    {
        return 1u;
    }

    return 0u;
}

static void startup_blink(void)
{
    uint8_t i;

    /*
     * Startup blink.
     *
     * This helps visually see that the MCU restarted.
     * After WDT reset, this blink sequence should appear again.
     */
    for (i = 0u; i < 6u; i++)
    {
        led_toggle();
        DRV_DELAY_MS(150u);

        /*
         * Clear WDT during startup blinking.
         * This prevents watchdog reset during the indication sequence.
         */
        wdt_clear();
    }
}

void main(void)
{
    /*
     * Initialize visible test hardware.
     */
    led_init();
    button_init();

    /*
     * Show startup pattern.
     *
     * If WDT resets the MCU, this pattern will repeat.
     */
    startup_blink();

    while (1)
    {
        /*
         * If button is pressed, simulate a software hang.
         *
         * We intentionally stop calling wdt_clear().
         * The Watchdog Timer should reset the MCU.
         */
        if (button_is_pressed() != 0u)
        {
            /*
             * Keep LED ON to show that we entered the hang state.
             */
            WDT_LED_PORT |= (uint8_t)(1u << WDT_LED_BIT);

            /*
             * Infinite loop without wdt_clear().
             *
             * Expected:
             *   WDT timeout occurs
             *   MCU resets
             *   startup_blink() runs again
             */
            while (1)
            {
                /*
                 * Do not clear WDT here.
                 */
            }
        }

        /*
         * Normal main loop work.
         *
         * LED toggles to show that the application is alive.
         */
        led_toggle();

        /*
         * Clear watchdog during normal operation.
         *
         * If this line is removed or not called often enough,
         * WDT will reset the MCU.
         */
        wdt_clear();

        /*
         * Simulate some useful work.
         *
         * Keep this delay shorter than the WDT timeout.
         */
        DRV_DELAY_MS(250u);
    }
}