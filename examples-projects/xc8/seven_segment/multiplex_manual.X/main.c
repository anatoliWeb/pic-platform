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
 * PIC18F452 four-digit seven-segment final demonstration.
 *
 * Display type:
 *   Common cathode.
 *
 * Refresh mode:
 *   Manual multiplex refresh from the main loop.
 *
 * Interrupts:
 *   Not used.
 *
 * --------------------------------------------------------------------------
 * Demonstrated library features
 * --------------------------------------------------------------------------
 *
 * 1. Random integer values.
 * 2. Random fixed-point values with a decimal point.
 * 3. Per-digit blinking.
 * 4. Left alignment.
 * 5. Right alignment.
 * 6. Leading zeroes.
 * 7. Individual decimal-point control.
 * 8. Global software brightness.
 * 9. Negative fixed-point values.
 * 10. Buffered display updates.
 *
 * --------------------------------------------------------------------------
 * PIC18F452 DIP-40 wiring
 * --------------------------------------------------------------------------
 *
 * Segment lines are shared by all four display digits.
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
 *
 * Digit-select lines:
 *
 * Display digit  | PIC signal | DIP pin | Connection
 * ---------------|------------|---------|------------------------------
 * Digit 1        | RC0        | 15      | Common cathode of digit 1
 * Digit 2        | RC1        | 16      | Common cathode of digit 2
 * Digit 3        | RC2        | 17      | Common cathode of digit 3
 * Digit 4        | RC3        | 18      | Common cathode of digit 4
 *
 * Direct common-cathode control:
 *
 *   RCx = 0 -> digit enabled
 *   RCx = 1 -> digit disabled
 *
 * Diagnostic outputs:
 *
 * Debug signal   | PIC signal | DIP pin | Meaning
 * ---------------|------------|---------|------------------------------
 * Error output   | RE0        | 8       | High when an API call fails
 * Heartbeat      | RE1        | 9       | Toggles whenever scene changes
 *
 * --------------------------------------------------------------------------
 * Expected scene sequence
 * --------------------------------------------------------------------------
 *
 * Scene 0:
 *   Random integer numbers from 0 to 9999.
 *
 * Scene 1:
 *   Random fixed-point numbers:
 *
 *      0.00 ... 99.99
 *
 * Scene 2:
 *   "1234", digit 1 blinks.
 *
 * Scene 3:
 *   "1234", digit 2 blinks.
 *
 * Scene 4:
 *   "1234", digit 3 blinks.
 *
 * Scene 5:
 *   "1234", digit 4 blinks.
 *
 * Scene 6:
 *   42 aligned left:
 *
 *      "42  "
 *
 * Scene 7:
 *   42 aligned right:
 *
 *      "  42"
 *
 * Scene 8:
 *   42 with leading zeroes:
 *
 *      "0042"
 *
 * Scene 9:
 *   "1234" with a moving decimal point.
 *
 * Scene 10:
 *   "8888" with brightness changing between 10% and 100%.
 *
 * Scene 11:
 *   Fixed-point value:
 *
 *      "12.34"
 *
 * Scene 12:
 *   Fixed-point value:
 *
 *      "0.05"
 *
 * Scene 13:
 *   Negative fixed-point value:
 *
 *      "-12.3"
 *
 * Then the sequence repeats.
 */

#define APP_DIGIT_COUNT                    4U
#define APP_DIGIT_ACTIVE_LEVEL             0U

#define APP_REFRESH_PERIOD_MS              1U

#define APP_SCENE_COUNT                   14U

/*
 * General scene duration.
 */
#define APP_SCENE_DURATION_MS           3000UL

/*
 * Dynamic value update periods.
 */
#define APP_RANDOM_INTEGER_UPDATE_MS     350UL
#define APP_RANDOM_FIXED_UPDATE_MS       450UL
#define APP_MOVING_DOT_UPDATE_MS         450UL
#define APP_BRIGHTNESS_UPDATE_MS          35UL

/*
 * Blink timing.
 */
#define APP_BLINK_PERIOD_MS              600U

/*
 * Brightness limits.
 */
#define APP_MIN_BRIGHTNESS                10U
#define APP_MAX_BRIGHTNESS               100U
#define APP_DEFAULT_BRIGHTNESS           100U
#define APP_BRIGHTNESS_STEP                5

/*
 * Blink masks.
 *
 * Bit 0 controls digit index 0.
 * Bit 1 controls digit index 1.
 * Bit 2 controls digit index 2.
 * Bit 3 controls digit index 3.
 */
#define APP_BLINK_DIGIT_1_MASK    ((uint16_t)0x0001U)
#define APP_BLINK_DIGIT_2_MASK    ((uint16_t)0x0002U)
#define APP_BLINK_DIGIT_3_MASK    ((uint16_t)0x0004U)
#define APP_BLINK_DIGIT_4_MASK    ((uint16_t)0x0008U)

/*
 * Diagnostic outputs.
 */
#define APP_ERROR_OFF()            (LATEbits.LATE0 = 0U)
#define APP_ERROR_ON()             (LATEbits.LATE0 = 1U)
#define APP_HEARTBEAT_TOGGLE()     (LATEbits.LATE1 ^= 1U)

/*
 * Digit-select pin table.
 *
 * The array order defines the logical digit order used by formatting,
 * decimal-point masks and blink masks.
 */
static const seven_segment_pin_t g_digit_pins[APP_DIGIT_COUNT] =
{
    { &LATC, &TRISC, 0U }, /* Digit index 0. */
    { &LATC, &TRISC, 1U }, /* Digit index 1. */
    { &LATC, &TRISC, 2U }, /* Digit index 2. */
    { &LATC, &TRISC, 3U }  /* Digit index 3. */
};

/*
 * Seven-segment hardware configuration.
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
    1U,                    /* Decimal point pin is available. */

    g_digit_pins,
    APP_DIGIT_COUNT,
    APP_DIGIT_ACTIVE_LEVEL,

    (const uint8_t *)0,
    0U,

    SEVEN_SEGMENT_COMMON_CATHODE
};

/*
 * Internal state of the small pseudo-random generator.
 *
 * A non-zero initial value is required by this LFSR implementation.
 */
static uint16_t g_random_state = 0xACE1U;

/*
 * Configure RE0 and RE1 as diagnostic outputs.
 */
static void app_debug_init(void)
{
    LATEbits.LATE0 = 0U;
    LATEbits.LATE1 = 0U;

    TRISEbits.TRISE0 = 0U;
    TRISEbits.TRISE1 = 0U;
}

/*
 * Disable all digits and clear every segment output.
 */
static void app_display_off(void)
{
    LATCbits.LATC0 = 1U;
    LATCbits.LATC1 = 1U;
    LATCbits.LATC2 = 1U;
    LATCbits.LATC3 = 1U;

    LATD = 0x00U;
}

/*
 * Stop the application and expose the failure on RE0.
 */
static void app_error(void)
{
    app_display_off();
    APP_ERROR_ON();

    while (1)
    {
        /*
         * Application stopped because initialization or formatting failed.
         */
    }
}

/*
 * Generate the next deterministic pseudo-random 16-bit value.
 *
 * This is a compact Galois LFSR. It avoids rand(), malloc and large
 * runtime dependencies, which is useful on PIC18 devices.
 */
static uint16_t app_random_next(void)
{
    uint16_t lsb;

    lsb = (uint16_t)(g_random_state & 0x0001U);
    g_random_state >>= 1U;

    if (lsb != 0U)
    {
        g_random_state ^= 0xB400U;
    }

    return g_random_state;
}

/*
 * Generate a value in the inclusive range 0..maximum.
 */
static uint16_t app_random_range(uint16_t maximum)
{
    uint32_t scaled;

    scaled =
        (uint32_t)app_random_next() *
        (uint32_t)(maximum + 1U);

    return (uint16_t)(scaled >> 16U);
}

/*
 * Clear all decimal points from the display buffer.
 */
static void app_clear_all_dots(seven_segment_t *display)
{
    uint8_t digit;

    for (digit = 0U; digit < APP_DIGIT_COUNT; digit++)
    {
        seven_segment_set_dot_at(
            display,
            digit,
            0U
        );
    }
}

/*
 * Disable blinking and clear the blink mask.
 */
static void app_disable_blink(seven_segment_t *display)
{
    seven_segment_set_blink_enabled(
        display,
        0U
    );

    seven_segment_set_blink_mask(
        display,
        0U
    );
}

/*
 * Configure one or more digits to blink.
 */
static void app_enable_blink(
    seven_segment_t *display,
    uint16_t mask)
{
    seven_segment_set_blink_mask(
        display,
        mask
    );

    seven_segment_set_blink_period_ms(
        display,
        APP_BLINK_PERIOD_MS
    );

    seven_segment_set_blink_enabled(
        display,
        1U
    );
}

/*
 * Display an integer using configurable alignment and leading zeroes.
 */
static drv_status_t app_show_integer_formatted(
    seven_segment_t *display,
    int32_t value,
    seven_segment_alignment_t alignment,
    uint8_t leading_zero)
{
    seven_segment_number_format_t format;

    format.start_digit = 0U;
    format.width = APP_DIGIT_COUNT;
    format.alignment = alignment;
    format.leading_zero = leading_zero;
    format.clear_unused = 1U;

    return seven_segment_set_number_formatted(
        display,
        value,
        &format
    );
}

/*
 * Display a fixed-point value using configurable formatting.
 *
 * scaled_value:
 *   Integer value with the decimal point removed.
 *
 * decimal_places:
 *   Number of digits shown after the decimal point.
 *
 * Example:
 *   scaled_value = 1234
 *   decimal_places = 2
 *   result = 12.34
 */
static drv_status_t app_show_fixed_formatted(
    seven_segment_t *display,
    int32_t scaled_value,
    uint8_t decimal_places,
    seven_segment_alignment_t alignment,
    uint8_t leading_zero)
{
    seven_segment_number_format_t format;

    format.start_digit = 0U;
    format.width = APP_DIGIT_COUNT;
    format.alignment = alignment;
    format.leading_zero = leading_zero;
    format.clear_unused = 1U;

    return seven_segment_set_fixed_formatted(
        display,
        scaled_value,
        decimal_places,
        &format
    );
}

/*
 * Change brightness by one configured step.
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

    seven_segment_set_brightness(
        display,
        *brightness
    );
}

/*
 * Prepare one demonstration scene.
 *
 * Dynamic scenes receive their initial value here and are updated later
 * by app_update_dynamic_scene().
 */
static drv_status_t app_apply_scene(
    seven_segment_t *display,
    uint8_t scene,
    uint8_t *moving_dot,
    uint8_t *brightness,
    int8_t *brightness_step)
{
    drv_status_t status;
    uint16_t random_value;

    /*
     * Start each scene from a predictable state.
     */
    app_disable_blink(display);
    app_clear_all_dots(display);

    *brightness = APP_DEFAULT_BRIGHTNESS;
    *brightness_step = -APP_BRIGHTNESS_STEP;
    *moving_dot = 0U;

    seven_segment_set_brightness(
        display,
        *brightness
    );

    switch (scene)
    {
        case 0U:
            /*
             * Random integer value from 0 to 9999.
             */
            random_value = app_random_range(9999U);

            status = app_show_integer_formatted(
                display,
                (int32_t)random_value,
                SEVEN_SEGMENT_ALIGN_RIGHT,
                0U
            );
            break;

        case 1U:
            /*
             * Random fixed-point value from 0.00 to 99.99.
             *
             * Example:
             *   random value 2537 -> 25.37
             */
            random_value = app_random_range(9999U);

            status = app_show_fixed_formatted(
                display,
                (int32_t)random_value,
                2U,
                SEVEN_SEGMENT_ALIGN_RIGHT,
                0U
            );
            break;

        case 2U:
            status = app_show_integer_formatted(
                display,
                1234,
                SEVEN_SEGMENT_ALIGN_RIGHT,
                0U
            );

            if (status == DRV_STATUS_OK)
            {
                app_enable_blink(
                    display,
                    APP_BLINK_DIGIT_1_MASK
                );
            }
            break;

        case 3U:
            status = app_show_integer_formatted(
                display,
                1234,
                SEVEN_SEGMENT_ALIGN_RIGHT,
                0U
            );

            if (status == DRV_STATUS_OK)
            {
                app_enable_blink(
                    display,
                    APP_BLINK_DIGIT_2_MASK
                );
            }
            break;

        case 4U:
            status = app_show_integer_formatted(
                display,
                1234,
                SEVEN_SEGMENT_ALIGN_RIGHT,
                0U
            );

            if (status == DRV_STATUS_OK)
            {
                app_enable_blink(
                    display,
                    APP_BLINK_DIGIT_3_MASK
                );
            }
            break;

        case 5U:
            status = app_show_integer_formatted(
                display,
                1234,
                SEVEN_SEGMENT_ALIGN_RIGHT,
                0U
            );

            if (status == DRV_STATUS_OK)
            {
                app_enable_blink(
                    display,
                    APP_BLINK_DIGIT_4_MASK
                );
            }
            break;

        case 6U:
            /*
             * Left alignment:
             *
             *   "42  "
             */
            status = app_show_integer_formatted(
                display,
                42,
                SEVEN_SEGMENT_ALIGN_LEFT,
                0U
            );
            break;

        case 7U:
            /*
             * Right alignment:
             *
             *   "  42"
             */
            status = app_show_integer_formatted(
                display,
                42,
                SEVEN_SEGMENT_ALIGN_RIGHT,
                0U
            );
            break;

        case 8U:
            /*
             * Right alignment with leading zeroes:
             *
             *   "0042"
             */
            status = app_show_integer_formatted(
                display,
                42,
                SEVEN_SEGMENT_ALIGN_RIGHT,
                1U
            );
            break;

        case 9U:
            /*
             * Moving decimal point over the value "1234".
             */
            status = app_show_integer_formatted(
                display,
                1234,
                SEVEN_SEGMENT_ALIGN_RIGHT,
                0U
            );

            if (status == DRV_STATUS_OK)
            {
                seven_segment_set_dot_at(
                    display,
                    *moving_dot,
                    1U
                );
            }
            break;

        case 10U:
            /*
             * Brightness demonstration.
             *
             * Start from full brightness and move toward the minimum.
             */
            status = app_show_integer_formatted(
                display,
                8888,
                SEVEN_SEGMENT_ALIGN_RIGHT,
                0U
            );
            break;

        case 11U:
            /*
             * Fixed-point example:
             *
             *   1234 with two decimal places -> 12.34
             */
            status = app_show_fixed_formatted(
                display,
                1234,
                2U,
                SEVEN_SEGMENT_ALIGN_RIGHT,
                0U
            );
            break;

        case 12U:
            /*
             * Value below one:
             *
             *   5 with two decimal places -> 0.05
             */
            status = app_show_fixed_formatted(
                display,
                5,
                2U,
                SEVEN_SEGMENT_ALIGN_RIGHT,
                0U
            );
            break;

        case 13U:
            /*
             * Negative fixed-point value:
             *
             *   -123 with one decimal place -> -12.3
             */
            status = app_show_fixed_formatted(
                display,
                -123,
                1U,
                SEVEN_SEGMENT_ALIGN_RIGHT,
                0U
            );
            break;

        default:
            status = DRV_STATUS_ERROR;
            break;
    }

    return status;
}

/*
 * Update features that change while a scene remains active.
 */
static drv_status_t app_update_dynamic_scene(
    seven_segment_t *display,
    uint8_t scene,
    uint32_t now_ms,
    uint32_t *last_dynamic_update_ms,
    uint8_t *moving_dot,
    uint8_t *brightness,
    int8_t *brightness_step)
{
    drv_status_t status = DRV_STATUS_OK;
    uint16_t random_value;

    switch (scene)
    {
        case 0U:
            /*
             * Generate a new random integer periodically.
             */
            if ((uint32_t)(now_ms - *last_dynamic_update_ms) >=
                APP_RANDOM_INTEGER_UPDATE_MS)
            {
                *last_dynamic_update_ms = now_ms;

                random_value = app_random_range(9999U);

                status = app_show_integer_formatted(
                    display,
                    (int32_t)random_value,
                    SEVEN_SEGMENT_ALIGN_RIGHT,
                    0U
                );
            }
            break;

        case 1U:
            /*
             * Generate a new random fixed-point value periodically.
             */
            if ((uint32_t)(now_ms - *last_dynamic_update_ms) >=
                APP_RANDOM_FIXED_UPDATE_MS)
            {
                *last_dynamic_update_ms = now_ms;

                random_value = app_random_range(9999U);

                status = app_show_fixed_formatted(
                    display,
                    (int32_t)random_value,
                    2U,
                    SEVEN_SEGMENT_ALIGN_RIGHT,
                    0U
                );
            }
            break;

        case 9U:
            /*
             * Move the decimal point to the next display digit.
             */
            if ((uint32_t)(now_ms - *last_dynamic_update_ms) >=
                APP_MOVING_DOT_UPDATE_MS)
            {
                *last_dynamic_update_ms = now_ms;

                app_clear_all_dots(display);

                (*moving_dot)++;

                if (*moving_dot >= APP_DIGIT_COUNT)
                {
                    *moving_dot = 0U;
                }

                seven_segment_set_dot_at(
                    display,
                    *moving_dot,
                    1U
                );
            }
            break;

        case 10U:
            /*
             * Animate global software brightness.
             */
            if ((uint32_t)(now_ms - *last_dynamic_update_ms) >=
                APP_BRIGHTNESS_UPDATE_MS)
            {
                *last_dynamic_update_ms = now_ms;

                app_update_brightness(
                    display,
                    brightness,
                    brightness_step
                );
            }
            break;

        default:
            /*
             * Static scenes need no additional buffer updates.
             */
            break;
    }

    return status;
}

void main(void)
{
    seven_segment_t display;

    uint8_t init_result;
    uint8_t scene = 0U;
    uint8_t moving_dot = 0U;
    uint8_t brightness = APP_DEFAULT_BRIGHTNESS;

    int8_t brightness_step = -APP_BRIGHTNESS_STEP;

    uint32_t now_ms = 0UL;
    uint32_t scene_started_ms = 0UL;
    uint32_t last_dynamic_update_ms = 0UL;

    drv_status_t status;

    /*
     * This example performs manual multiplex refresh.
     * Global and peripheral interrupts are not required.
     */
    INTCONbits.GIE = 0U;
    INTCONbits.PEIE = 0U;

    /*
     * Configure analog-capable pins as digital GPIO.
     */
    ADCON1 = 0x07U;

#if defined(CMCON)
    /*
     * Disable the comparator module.
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
     * Configure safe initial display states.
     */
    LATD = 0x00U;

    LATCbits.LATC0 = 1U;
    LATCbits.LATC1 = 1U;
    LATCbits.LATC2 = 1U;
    LATCbits.LATC3 = 1U;

    TRISD = 0x00U;

    TRISCbits.TRISC0 = 0U;
    TRISCbits.TRISC1 = 0U;
    TRISCbits.TRISC2 = 0U;
    TRISCbits.TRISC3 = 0U;

    /*
     * Initialize the reusable display object.
     *
     * Return value:
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
     * Set the initial full brightness.
     */
    seven_segment_set_brightness(
        &display,
        APP_DEFAULT_BRIGHTNESS
    );

    /*
     * Prepare the first demonstration scene.
     */
    status = app_apply_scene(
        &display,
        scene,
        &moving_dot,
        &brightness,
        &brightness_step
    );

    if (status != DRV_STATUS_OK)
    {
        app_error();
    }

    while (1)
    {
        /*
         * Refresh one physical digit.
         *
         * Continuous calls create the complete four-digit image.
         */
        seven_segment_process(
            &display
        );

        delay_ms(APP_REFRESH_PERIOD_MS);

        now_ms++;

        /*
         * Update the software blink phase.
         *
         * This call is harmless while blinking is disabled.
         */
        seven_segment_blink_update(
            &display,
            now_ms
        );

        /*
         * Update random values, moving decimal point or brightness
         * while the current scene remains active.
         */
        status = app_update_dynamic_scene(
            &display,
            scene,
            now_ms,
            &last_dynamic_update_ms,
            &moving_dot,
            &brightness,
            &brightness_step
        );

        if (status != DRV_STATUS_OK)
        {
            app_error();
        }

        /*
         * Switch to the next demonstration scene.
         */
        if ((uint32_t)(now_ms - scene_started_ms) >=
            APP_SCENE_DURATION_MS)
        {
            scene_started_ms = now_ms;
            last_dynamic_update_ms = now_ms;

            scene++;

            if (scene >= APP_SCENE_COUNT)
            {
                scene = 0U;
            }

            status = app_apply_scene(
                &display,
                scene,
                &moving_dot,
                &brightness,
                &brightness_step
            );

            if (status != DRV_STATUS_OK)
            {
                app_error();
            }

            /*
             * Toggle RE1 to show that scene sequencing is alive.
             */
            APP_HEARTBEAT_TOGGLE();
        }
    }
}