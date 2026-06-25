/*
 * File: libraries/display/seven_segment/example.c
 */

#include "project_config.h"

#include "core/compiler.h"
#include "core/types.h"
#include "core/delay.h"

#include "libraries/display/seven_segment/seven_segment.h"

#if defined(DRV_COMPILER_C18)
#include <p18f452.h>
#endif

#if defined(DRV_COMPILER_XC8)
#include <xc.h>
#endif

/*
 * PIC18F452 single-digit seven-segment manual-refresh example.
 *
 * --------------------------------------------------------------------------
 * Required project configuration
 * --------------------------------------------------------------------------
 *
 * This example does not use a hardware timer.
 *
 * In core/pic_platform_config.h, or another configuration header included
 * by project_config.h, keep all seven-segment timer backends disabled:
 *
 *   #define SEVEN_SEGMENT_ENABLE_TIMER0 0
 *   #define SEVEN_SEGMENT_ENABLE_TIMER1 0
 *   #define SEVEN_SEGMENT_ENABLE_TIMER2 0
 *   #define SEVEN_SEGMENT_ENABLE_TIMER3 0
 *
 * No timer0.c, timer1.c, timer2.c or timer3.c file is required.
 *
 * The display configuration below selects:
 *
 *   SEVEN_SEGMENT_REFRESH_MANUAL
 *   SEVEN_SEGMENT_TIMER_NONE
 *   refresh_period_us = 0
 *
 * In manual mode the application must call seven_segment_refresh()
 * periodically. This example does that inside display_wait_ms().
 *
 * --------------------------------------------------------------------------
 * Example wiring
 * --------------------------------------------------------------------------
 *
 * Segment A   -> RB0
 * Segment B   -> RB1
 * Segment C   -> RB2
 * Segment D   -> RB3
 * Segment E   -> RB4
 * Segment F   -> RB5
 * Segment G   -> RB6
 * Decimal dot -> RB7
 * Digit 1     -> RC0
 *
 * The example uses a common-cathode display.
 *
 * Digit active level below is LOW:
 *
 *   RC0 = 0 -> digit enabled
 *   RC0 = 1 -> digit disabled
 *
 * If the hardware uses an inverting transistor, change
 * APP_DIGIT_ACTIVE_LEVEL to 1.
 */

#define APP_DIGIT_COUNT             1U
#define APP_DIGIT_ACTIVE_LEVEL      0U

#define APP_DIGIT_SHOW_MS         250U
#define APP_FIXED_SHOW_MS         400U
#define APP_DOT_SHOW_MS           200U
#define APP_CLEAR_SHOW_MS         200U

static seven_segment_t g_display;

static const seven_segment_pin_t g_digits[APP_DIGIT_COUNT] =
{
    { &LATC, &TRISC, 0U }
};

/*
 * Custom symbol table used by seven_segment_set_hex().
 *
 * Entries correspond to:
 *   10 -> A
 *   11 -> b
 *   12 -> C
 *   13 -> d
 *   14 -> E
 *   15 -> F
 */
static const uint8_t g_hex_symbols[6] =
{
    0x77U, /* A */
    0x7CU, /* b */
    0x39U, /* C */
    0x5EU, /* d */
    0x79U, /* E */
    0x71U  /* F */
};

static const seven_segment_config_t g_display_config =
{
    { &LATB, &TRISB, 0U }, /* Segment A. */
    { &LATB, &TRISB, 1U }, /* Segment B. */
    { &LATB, &TRISB, 2U }, /* Segment C. */
    { &LATB, &TRISB, 3U }, /* Segment D. */
    { &LATB, &TRISB, 4U }, /* Segment E. */
    { &LATB, &TRISB, 5U }, /* Segment F. */
    { &LATB, &TRISB, 6U }, /* Segment G. */

    { &LATB, &TRISB, 7U }, /* Decimal point. */
    1U,                    /* Decimal point is present. */

    g_digits,
    APP_DIGIT_COUNT,
    APP_DIGIT_ACTIVE_LEVEL,

    g_hex_symbols,
    6U,

    SEVEN_SEGMENT_COMMON_CATHODE,

    /*
     * Manual refresh mode:
     * no timer backend is selected or started.
     */
    SEVEN_SEGMENT_REFRESH_MANUAL,
    SEVEN_SEGMENT_TIMER_NONE,
    0U
};

static void display_wait_ms(
    seven_segment_t *display,
    uint16_t duration_ms)
{
    uint16_t elapsed_ms;

    for (elapsed_ms = 0U;
         elapsed_ms < duration_ms;
         elapsed_ms++)
    {
        /*
         * In manual mode the application owns refresh timing.
         *
         * For a one-digit display, one refresh every millisecond
         * keeps the digit continuously visible.
         */
        seven_segment_refresh(display);

        DRV_DELAY_MS(1U);
    }
}

static void app_fatal(void)
{
    /*
     * Keep all display lines in a safe state.
     */
    seven_segment_stop(&g_display);

    while (1)
    {
        /*
         * Initialization failed.
         * Add UART or diagnostic GPIO here when required.
         */
    }
}

void main(void)
{
    uint8_t init_result;
    uint8_t value;
    drv_status_t status;

    /*
     * Configure analog-capable pins as digital GPIO.
     */
    ADCON1 = 0x07U;

#if defined(CMCON)
    CMCON = 0x07U;
#endif

    /*
     * Disable Parallel Slave Port mode.
     */
    TRISEbits.PSPMODE = 0U;

    /*
     * Safe initial output levels.
     *
     * Common-cathode segment lines are inactive at LOW.
     * Digit line is inactive at HIGH because its active level is LOW.
     */
    LATB = 0x00U;
    LATCbits.LATC0 = 1U;

    init_result = seven_segment_init(
        &g_display,
        &g_display_config
    );

    if (init_result == 0U)
    {
        app_fatal();
    }

    while (1)
    {
        /*
         * Show decimal counter 0..9.
         */
        for (value = 0U; value <= 9U; value++)
        {
            seven_segment_set_digit(
                &g_display,
                value
            );

            display_wait_ms(
                &g_display,
                APP_DIGIT_SHOW_MS
            );
        }

        /*
         * Show hexadecimal sequence A..F through the custom table.
         */
        for (value = 10U; value <= 15U; value++)
        {
            seven_segment_set_hex(
                &g_display,
                value
            );

            display_wait_ms(
                &g_display,
                APP_DIGIT_SHOW_MS
            );
        }

        /*
         * Show scaled value 5 with one decimal place.
         *
         * On a one-digit display this demonstrates the decimal point:
         *   5.
         */
        status = seven_segment_set_fixed(
            &g_display,
            5,
            1U
        );

        if (status != DRV_STATUS_OK)
        {
            app_fatal();
        }

        display_wait_ms(
            &g_display,
            APP_FIXED_SHOW_MS
        );

        /*
         * Blink the decimal point manually.
         */
        seven_segment_set_dot(
            &g_display,
            1U
        );

        display_wait_ms(
            &g_display,
            APP_DOT_SHOW_MS
        );

        seven_segment_set_dot(
            &g_display,
            0U
        );

        display_wait_ms(
            &g_display,
            APP_DOT_SHOW_MS
        );

        /*
         * Clear the display before repeating the test.
         */
        seven_segment_clear(
            &g_display
        );

        display_wait_ms(
            &g_display,
            APP_CLEAR_SHOW_MS
        );
    }
}
