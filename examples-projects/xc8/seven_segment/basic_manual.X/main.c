#include "project_config.h"

#include "core/compiler.h"
#include "core/delay.h"
#include "libraries/display/seven_segment/seven_segment.h"

#if defined(DRV_COMPILER_C18)
#include <p18f452.h>
#endif

#if defined(DRV_COMPILER_XC8)
#include <xc.h>
#endif

/*
 * PIC18F452 single-digit seven-segment library example.
 *
 * Display type:
 *   Common cathode.
 *
 * Refresh mode:
 *   Manual refresh from the main loop.
 *
 * Interrupts:
 *   Not required.
 *
 * --------------------------------------------------------------------------
 * PIC18F452 DIP-40 wiring
 * --------------------------------------------------------------------------
 *
 * Display signal | PIC signal | DIP pin | Connection
 * ---------------|------------|---------|------------------------------
 * Segment A      | RD0        | 19      | Through 220-470 ohm resistor
 * Segment B      | RD1        | 20      | Through 220-470 ohm resistor
 * Segment C      | RD2        | 21      | Through 220-470 ohm resistor
 * Segment D      | RD3        | 22      | Through 220-470 ohm resistor
 * Segment E      | RD4        | 27      | Through 220-470 ohm resistor
 * Segment F      | RD5        | 28      | Through 220-470 ohm resistor
 * Segment G      | RD6        | 29      | Through 220-470 ohm resistor
 * Decimal point  | RD7        | 30      | Through 220-470 ohm resistor
 * Common cathode | RC0        | 15      | Direct test connection
 *
 * Diagnostic outputs:
 *
 * Debug signal   | PIC signal | DIP pin | Meaning
 * ---------------|------------|---------|------------------------------
 * Error          | RE0        | 8       | High when initialization fails
 * Heartbeat      | RE1        | 9       | Toggles when the digit changes
 *
 * Direct common-cathode control:
 *
 *   RC0 = 0 -> display enabled
 *   RC0 = 1 -> display disabled
 *
 * --------------------------------------------------------------------------
 * Expected behavior
 * --------------------------------------------------------------------------
 *
 * 1. The digit changes continuously:
 *
 *      0 -> 1 -> 2 -> ... -> 9 -> 0
 *
 * 2. The decimal point toggles after every digit change.
 *
 * 3. Brightness changes continuously:
 *
 *      10% -> 100% -> 10%
 *
 * 4. RE1 toggles whenever the displayed digit changes.
 */

#define APP_REFRESH_PERIOD_MS              1U

#define APP_DIGIT_UPDATE_TICKS           500U
#define APP_BRIGHTNESS_UPDATE_TICKS       30U

#define APP_MIN_DIGIT                      0U
#define APP_MAX_DIGIT                      9U

#define APP_MIN_BRIGHTNESS                10U
#define APP_MAX_BRIGHTNESS               100U
#define APP_INITIAL_BRIGHTNESS            10U
#define APP_BRIGHTNESS_STEP                5

/*
 * Direct common-cathode control is active low.
 */
#define APP_DIGIT_ACTIVE_LEVEL             0U

/*
 * Diagnostic outputs.
 */
#define APP_ERROR_OFF()                    (LATEbits.LATE0 = 0U)
#define APP_ERROR_ON()                     (LATEbits.LATE0 = 1U)
#define APP_HEARTBEAT_TOGGLE()             (LATEbits.LATE1 ^= 1U)

/*
 * One digit-select pin:
 *
 *   RC0 -> common cathode
 */
static const seven_segment_pin_t g_digit_pins[1] =
{
    {
        &LATC,
        &TRISC,
        0U
    }
};

/*
 * Seven-segment configuration.
 *
 * Segment mapping:
 *
 *   RD0 -> A
 *   RD1 -> B
 *   RD2 -> C
 *   RD3 -> D
 *   RD4 -> E
 *   RD5 -> F
 *   RD6 -> G
 *   RD7 -> DP
 */
static const seven_segment_config_t g_display_config =
{
    { &LATD, &TRISD, 0U }, /* Segment A. */
    { &LATD, &TRISD, 1U }, /* Segment B. */
    { &LATD, &TRISD, 2U }, /* Segment C. */
    { &LATD, &TRISD, 3U }, /* Segment D. */
    { &LATD, &TRISD, 4U }, /* Segment E. */
    { &LATD, &TRISD, 5U }, /* Segment F. */
    { &LATD, &TRISD, 6U }, /* Segment G. */
    { &LATD, &TRISD, 7U }, /* Decimal point. */

    1U,                          /* Decimal point is available. */
    g_digit_pins,                /* Digit-select pin table. */
    1U,                          /* One display digit. */
    APP_DIGIT_ACTIVE_LEVEL,      /* Common cathode is active low. */
    (const uint8_t *)0,          /* No custom symbol table. */
    0U,                          /* No custom symbols. */
//    SEVEN_SEGMENT_COMMON_CATHODE
    SEVEN_SEGMENT_COMMON_ANODE
};

static void app_debug_init(void)
{
    /*
     * RE0 and RE1 are used as diagnostic outputs.
     */
    LATEbits.LATE0 = 0U;
    LATEbits.LATE1 = 0U;

    TRISEbits.TRISE0 = 0U;
    TRISEbits.TRISE1 = 0U;
}

static void app_display_off(void)
{
    /*
     * Disable the common cathode and clear all segment outputs.
     */
    LATCbits.LATC0 = 1U;
    LATD = 0x00U;
}

static void app_error(void)
{
    app_display_off();
    APP_ERROR_ON();

    while (1)
    {
        /*
         * RE0 remains high when initialization fails.
         */
    }
}

static uint8_t app_next_digit(uint8_t value)
{
    value++;

    if (value > APP_MAX_DIGIT)
    {
        value = APP_MIN_DIGIT;
    }

    return value;
}

/*
 * Move brightness between the minimum and maximum values.
 *
 * Positive step:
 *   brightness increases.
 *
 * Negative step:
 *   brightness decreases.
 */
static void app_update_brightness(
    seven_segment_t *display,
    uint8_t *brightness,
    int8_t *step)
{
    int16_t next_brightness;

    next_brightness =
        (int16_t)(*brightness) +
        (int16_t)(*step);

    if (next_brightness >= (int16_t)APP_MAX_BRIGHTNESS)
    {
        next_brightness = (int16_t)APP_MAX_BRIGHTNESS;
        *step = -APP_BRIGHTNESS_STEP;
    }
    else if (next_brightness <= (int16_t)APP_MIN_BRIGHTNESS)
    {
        next_brightness = (int16_t)APP_MIN_BRIGHTNESS;
        *step = APP_BRIGHTNESS_STEP;
    }

    *brightness = (uint8_t)next_brightness;

    /*
     * Update software brightness.
     *
     * The library changes the display duty cycle during refresh.
     * No hardware CCP/PWM peripheral is used.
     */
    seven_segment_set_brightness(
        display,
        *brightness
    );
}

void main(void)
{
    seven_segment_t display;

    uint16_t digit_ticks = 0U;
    uint16_t brightness_ticks = 0U;

    uint8_t value = APP_MIN_DIGIT;
    uint8_t dot_enabled = 0U;
    uint8_t brightness = APP_INITIAL_BRIGHTNESS;
    uint8_t init_result;

    int8_t brightness_step = APP_BRIGHTNESS_STEP;

    /*
     * Manual refresh does not require interrupts.
     */
    INTCONbits.GIE = 0U;
    INTCONbits.PEIE = 0U;

    /*
     * Configure analog-capable pins as digital GPIO.
     */
    ADCON1 = 0x07U;

#if defined(CMCON)
    /*
     * Disable comparators.
     */
    CMCON = 0x07U;
#endif

    /*
     * Disable Parallel Slave Port mode before using PORTD as GPIO.
     */
    TRISEbits.PSPMODE = 0U;

    app_debug_init();
    APP_ERROR_OFF();

    /*
     * Configure safe output states before library initialization.
     */
    LATD = 0x00U;
    LATCbits.LATC0 = 1U;

    TRISD = 0x00U;
    TRISCbits.TRISC0 = 0U;

    /*
     * seven_segment_init() returns:
     *
     *   1 -> initialization succeeded
     *   0 -> initialization failed
     */
    init_result = seven_segment_init(
        &display,
        &g_display_config
    );

    if (init_result == 0U)
    {
        app_error();
    }

    /*
     * Store initial content in the internal display buffer.
     */
    seven_segment_set_digit(
        &display,
        value
    );

    seven_segment_set_dot(
        &display,
        dot_enabled
    );

    seven_segment_set_brightness(
        &display,
        brightness
    );

    while (1)
    {
        /*
         * Perform one manual display refresh step.
         *
         * This function must be called continuously because no hardware
         * timer is used in this example.
         */
        seven_segment_process(&display);

        delay_ms(APP_REFRESH_PERIOD_MS);

        digit_ticks++;
        brightness_ticks++;

        /*
         * Change the digit and decimal point approximately every 500 ms.
         */
        if (digit_ticks >= APP_DIGIT_UPDATE_TICKS)
        {
            digit_ticks = 0U;

            value = app_next_digit(value);

            dot_enabled =
                (dot_enabled == 0U) ? 1U : 0U;

            seven_segment_set_digit(
                &display,
                value
            );

            seven_segment_set_dot(
                &display,
                dot_enabled
            );

            APP_HEARTBEAT_TOGGLE();
        }

        /*
         * Change brightness approximately every 30 ms.
         *
         * With a step of 5, brightness moves smoothly between
         * 10% and 100%.
         */
        if (brightness_ticks >= APP_BRIGHTNESS_UPDATE_TICKS)
        {
            brightness_ticks = 0U;

            app_update_brightness(
                &display,
                &brightness,
                &brightness_step
            );
        }
    }
}