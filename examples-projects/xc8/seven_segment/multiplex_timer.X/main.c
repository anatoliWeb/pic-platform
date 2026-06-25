#include "project_config.h"

#include "core/compiler.h"
#include "core/types.h"

#include "libraries/display/seven_segment/seven_segment.h"

#if defined(DRV_COMPILER_C18)
#include <p18f452.h>
#endif

#if defined(DRV_COMPILER_XC8)
#include <xc.h>
#endif

/*
 * --------------------------------------------------------------------------
 * REQUIRED PROJECT CONFIGURATION
 * --------------------------------------------------------------------------
 *
 * This example uses only Timer2.
 *
 * In core/pic_platform_config.h, or in the configuration header included
 * by project_config.h, set:
 *
 *   #define SEVEN_SEGMENT_ENABLE_TIMER0 0
 *   #define SEVEN_SEGMENT_ENABLE_TIMER1 0
 *   #define SEVEN_SEGMENT_ENABLE_TIMER2 1
 *   #define SEVEN_SEGMENT_ENABLE_TIMER3 0
 *
 * Add this timer source file to the MPLAB project:
 *
 *   drivers/timers/timer2/timer2.c
 *
 * Do not add timer0.c, timer1.c or timer3.c for this example.
 *
 * The macros only decide which timer backend is compiled into
 * seven_segment.c. The actual refresh timer is selected below in
 * g_display_config:
 *
 *   SEVEN_SEGMENT_REFRESH_TIMER,
 *   SEVEN_SEGMENT_TIMER2,
 *   APP_REFRESH_PERIOD_US
 *
 * For a project that does not use any timer:
 *
 *   #define SEVEN_SEGMENT_ENABLE_TIMER0 0
 *   #define SEVEN_SEGMENT_ENABLE_TIMER1 0
 *   #define SEVEN_SEGMENT_ENABLE_TIMER2 0
 *   #define SEVEN_SEGMENT_ENABLE_TIMER3 0
 *
 * and use:
 *
 *   SEVEN_SEGMENT_REFRESH_MANUAL,
 *   SEVEN_SEGMENT_TIMER_NONE,
 *   0U
 *
 * In manual mode no timerX.c file is required, but the application must call
 * the manual display processing function required by the library.
 * --------------------------------------------------------------------------
 */

/*
 * Fail at compile time when the project configuration does not match
 * this Timer2 example.
 */
#if !defined(SEVEN_SEGMENT_ENABLE_TIMER2)
#error "Define SEVEN_SEGMENT_ENABLE_TIMER2 in the project configuration."
#endif

#if (SEVEN_SEGMENT_ENABLE_TIMER2 != 1)
#error "This example requires SEVEN_SEGMENT_ENABLE_TIMER2 = 1."
#endif

#if defined(SEVEN_SEGMENT_ENABLE_TIMER0) && (SEVEN_SEGMENT_ENABLE_TIMER0 != 0)
#error "Disable SEVEN_SEGMENT_ENABLE_TIMER0 for this Timer2-only example."
#endif

#if defined(SEVEN_SEGMENT_ENABLE_TIMER1) && (SEVEN_SEGMENT_ENABLE_TIMER1 != 0)
#error "Disable SEVEN_SEGMENT_ENABLE_TIMER1 for this Timer2-only example."
#endif

#if defined(SEVEN_SEGMENT_ENABLE_TIMER3) && (SEVEN_SEGMENT_ENABLE_TIMER3 != 0)
#error "Disable SEVEN_SEGMENT_ENABLE_TIMER3 for this Timer2-only example."
#endif

/*
 * PIC18F452 DIP-40
 * Complete Timer2-driven seven-segment library test without UART.
 *
 * Clock:
 *   10 MHz
 *
 * Four-digit common-cathode display:
 *   RD0..RD7 -> A, B, C, D, E, F, G, DP
 *   RC0..RC3 -> DIG1..DIG4
 *   Digit active level: LOW
 *
 * The display library owns Timer2 refresh.
 * main() never calls seven_segment_refresh(), process(),
 * blink_update(), timer2_init(), timer2_start() or timer callbacks.
 *
 * UART debug is intentionally not included in this version.
 * This reduces program-memory usage and leaves RC6/RC7 available.
 */

#define APP_DIGIT_COUNT                       4U
#define APP_DIGIT_ACTIVE_LEVEL                0U
#define APP_REFRESH_PERIOD_US               1000U

#define APP_SCENE_COUNT                      16U
#define APP_SCENE_DURATION_MS              3000UL

#define APP_COUNTER_UPDATE_MS               250UL
#define APP_RANDOM_UPDATE_MS                350UL
#define APP_DOT_UPDATE_MS                   400UL
#define APP_BRIGHTNESS_UPDATE_MS             50UL
#define APP_TIME_UPDATE_MS                 1000UL

#define APP_BLINK_PERIOD_MS                 600U
#define APP_DEFAULT_BRIGHTNESS              100U
#define APP_MIN_BRIGHTNESS                   10U
#define APP_MAX_BRIGHTNESS                  100U
#define APP_BRIGHTNESS_STEP                   5

#define APP_COUNTER_MAX                    9999U

#define APP_BLINK_DIGIT_1_MASK      ((uint16_t)0x0001U)
#define APP_BLINK_DIGIT_2_MASK      ((uint16_t)0x0002U)
#define APP_BLINK_DIGIT_3_MASK      ((uint16_t)0x0004U)
#define APP_BLINK_DIGIT_4_MASK      ((uint16_t)0x0008U)

static seven_segment_t g_display;

static uint16_t g_random_state = 0xACE1U;

static const seven_segment_pin_t g_digit_pins[APP_DIGIT_COUNT] =
{
    { &LATC, &TRISC, 0U },
    { &LATC, &TRISC, 1U },
    { &LATC, &TRISC, 2U },
    { &LATC, &TRISC, 3U }
};

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
    1U,

    g_digit_pins,
    APP_DIGIT_COUNT,
    APP_DIGIT_ACTIVE_LEVEL,

    (const uint8_t *)0,
    0U,

    SEVEN_SEGMENT_COMMON_CATHODE,
    SEVEN_SEGMENT_REFRESH_TIMER,
    SEVEN_SEGMENT_TIMER2,
    APP_REFRESH_PERIOD_US
};

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

static uint16_t app_random_range(uint16_t maximum)
{
    uint32_t scaled;

    scaled =
        (uint32_t)app_random_next() *
        ((uint32_t)maximum + 1UL);

    return (uint16_t)(scaled >> 16U);
}

static void app_clear_dots(void)
{
    uint8_t digit;

    for (digit = 0U; digit < APP_DIGIT_COUNT; digit++)
    {
        seven_segment_set_dot_at(
            &g_display,
            digit,
            0U
        );
    }
}

static void app_disable_blink(void)
{
    seven_segment_set_blink_enabled(
        &g_display,
        0U
    );

    seven_segment_set_blink_mask(
        &g_display,
        0U
    );
}

static void app_enable_blink(uint16_t mask)
{
    seven_segment_set_blink_mask(
        &g_display,
        mask
    );

    seven_segment_set_blink_period_ms(
        &g_display,
        APP_BLINK_PERIOD_MS
    );

    seven_segment_set_blink_enabled(
        &g_display,
        1U
    );
}

static drv_status_t app_show_number(
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
        &g_display,
        value,
        &format
    );
}

static drv_status_t app_show_fixed(
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
        &g_display,
        scaled_value,
        decimal_places,
        &format
    );
}

static void app_fatal(void)
{
    seven_segment_stop(&g_display);

    while (1)
    {
        /*
         * Fatal state.
         * Add a diagnostic LED here when required.
         */
    }
}

static drv_status_t app_apply_scene(
    uint8_t scene,
    uint16_t *counter,
    uint8_t *moving_dot,
    uint8_t *blink_digit,
    uint8_t *brightness,
    int8_t *brightness_step,
    uint8_t *hex_value,
    uint8_t *hours,
    uint8_t *minutes,
    uint8_t *raw_position)
{
    drv_status_t status;

    status = DRV_STATUS_OK;

    app_disable_blink();
    app_clear_dots();

    seven_segment_set_brightness(
        &g_display,
        APP_DEFAULT_BRIGHTNESS
    );

    *counter = 0U;
    *moving_dot = 0U;
    *blink_digit = 0U;
    *brightness = APP_DEFAULT_BRIGHTNESS;
    *brightness_step = -APP_BRIGHTNESS_STEP;
    *hex_value = 0U;
    *hours = 12U;
    *minutes = 34U;
    *raw_position = 0U;

    switch (scene)
    {
        case 0U:
            status = app_show_number(
                0,
                SEVEN_SEGMENT_ALIGN_RIGHT,
                1U
            );
            break;

        case 1U:
            status = app_show_number(
                (int32_t)app_random_range(9999U),
                SEVEN_SEGMENT_ALIGN_RIGHT,
                0U
            );
            break;

        case 2U:
            status = app_show_fixed(
                (int32_t)app_random_range(9999U),
                2U,
                SEVEN_SEGMENT_ALIGN_RIGHT,
                1U
            );
            break;

        case 3U:
            status = app_show_number(
                42,
                SEVEN_SEGMENT_ALIGN_LEFT,
                0U
            );
            break;

        case 4U:
            status = app_show_number(
                42,
                SEVEN_SEGMENT_ALIGN_RIGHT,
                0U
            );
            break;

        case 5U:
            status = app_show_number(
                42,
                SEVEN_SEGMENT_ALIGN_RIGHT,
                1U
            );
            break;

        case 6U:
            status = app_show_number(
                -123,
                SEVEN_SEGMENT_ALIGN_RIGHT,
                0U
            );
            break;

        case 7U:
            status = app_show_fixed(
                1234,
                2U,
                SEVEN_SEGMENT_ALIGN_RIGHT,
                0U
            );
            break;

        case 8U:
            status = app_show_fixed(
                5,
                2U,
                SEVEN_SEGMENT_ALIGN_RIGHT,
                1U
            );
            break;

        case 9U:
            status = app_show_fixed(
                -123,
                1U,
                SEVEN_SEGMENT_ALIGN_RIGHT,
                0U
            );
            break;

        case 10U:
            status = app_show_number(
                1234,
                SEVEN_SEGMENT_ALIGN_RIGHT,
                0U
            );

            if (status == DRV_STATUS_OK)
            {
                seven_segment_set_dot_at(
                    &g_display,
                    0U,
                    1U
                );
            }
            break;

        case 11U:
            status = app_show_number(
                1234,
                SEVEN_SEGMENT_ALIGN_RIGHT,
                0U
            );

            if (status == DRV_STATUS_OK)
            {
                app_enable_blink(
                    APP_BLINK_DIGIT_1_MASK
                );
            }
            break;

        case 12U:
            status = app_show_number(
                8888,
                SEVEN_SEGMENT_ALIGN_RIGHT,
                0U
            );
            break;

        case 13U:
            seven_segment_clear(&g_display);

            seven_segment_set_hex(
                &g_display,
                0x00U
            );
            break;

        case 14U:
            status = seven_segment_set_time_hhmm(
                &g_display,
                0U,
                *hours,
                *minutes,
                1U
            );
            break;

        case 15U:
            seven_segment_clear(&g_display);

            seven_segment_set_raw_at(
                &g_display,
                0U,
                SEVEN_SEGMENT_MASK_A
            );
            break;

        default:
            status = DRV_STATUS_ERROR;
            break;
    }

    return status;
}

static drv_status_t app_update_scene(
    uint8_t scene,
    uint32_t now_ms,
    uint32_t *last_update_ms,
    uint16_t *counter,
    uint8_t *moving_dot,
    uint8_t *blink_digit,
    uint8_t *brightness,
    int8_t *brightness_step,
    uint8_t *hex_value,
    uint8_t *hours,
    uint8_t *minutes,
    uint8_t *raw_position)
{
    drv_status_t status;
    int16_t next_brightness;
    uint8_t raw_pattern;

    status = DRV_STATUS_OK;

    switch (scene)
    {
        case 0U:
            if ((uint32_t)(now_ms - *last_update_ms) >=
                APP_COUNTER_UPDATE_MS)
            {
                *last_update_ms = now_ms;

                (*counter)++;

                if (*counter > APP_COUNTER_MAX)
                {
                    *counter = 0U;
                }

                status = app_show_number(
                    (int32_t)*counter,
                    SEVEN_SEGMENT_ALIGN_RIGHT,
                    1U
                );
            }
            break;

        case 1U:
            if ((uint32_t)(now_ms - *last_update_ms) >=
                APP_RANDOM_UPDATE_MS)
            {
                *last_update_ms = now_ms;

                status = app_show_number(
                    (int32_t)app_random_range(9999U),
                    SEVEN_SEGMENT_ALIGN_RIGHT,
                    0U
                );
            }
            break;

        case 2U:
            if ((uint32_t)(now_ms - *last_update_ms) >=
                APP_RANDOM_UPDATE_MS)
            {
                *last_update_ms = now_ms;

                status = app_show_fixed(
                    (int32_t)app_random_range(9999U),
                    2U,
                    SEVEN_SEGMENT_ALIGN_RIGHT,
                    1U
                );
            }
            break;

        case 10U:
            if ((uint32_t)(now_ms - *last_update_ms) >=
                APP_DOT_UPDATE_MS)
            {
                *last_update_ms = now_ms;

                app_clear_dots();

                (*moving_dot)++;

                if (*moving_dot >= APP_DIGIT_COUNT)
                {
                    *moving_dot = 0U;
                }

                seven_segment_set_dot_at(
                    &g_display,
                    *moving_dot,
                    1U
                );
            }
            break;

        case 11U:
            if ((uint32_t)(now_ms - *last_update_ms) >=
                APP_BLINK_PERIOD_MS)
            {
                *last_update_ms = now_ms;

                (*blink_digit)++;

                if (*blink_digit >= APP_DIGIT_COUNT)
                {
                    *blink_digit = 0U;
                }

                app_enable_blink(
                    (uint16_t)((uint16_t)1U << *blink_digit)
                );
            }
            break;

        case 12U:
            if ((uint32_t)(now_ms - *last_update_ms) >=
                APP_BRIGHTNESS_UPDATE_MS)
            {
                *last_update_ms = now_ms;

                next_brightness =
                    (int16_t)(*brightness) +
                    (int16_t)(*brightness_step);

                if (next_brightness >=
                    (int16_t)APP_MAX_BRIGHTNESS)
                {
                    next_brightness =
                        (int16_t)APP_MAX_BRIGHTNESS;

                    *brightness_step =
                        -APP_BRIGHTNESS_STEP;
                }
                else if (next_brightness <=
                         (int16_t)APP_MIN_BRIGHTNESS)
                {
                    next_brightness =
                        (int16_t)APP_MIN_BRIGHTNESS;

                    *brightness_step =
                        APP_BRIGHTNESS_STEP;
                }

                *brightness =
                    (uint8_t)next_brightness;

                seven_segment_set_brightness(
                    &g_display,
                    *brightness
                );
            }
            break;

        case 13U:
            if ((uint32_t)(now_ms - *last_update_ms) >=
                APP_DOT_UPDATE_MS)
            {
                *last_update_ms = now_ms;

                (*hex_value)++;

                if (*hex_value > 0x0FU)
                {
                    *hex_value = 0U;
                }

                seven_segment_set_hex(
                    &g_display,
                    *hex_value
                );
            }
            break;

        case 14U:
            if ((uint32_t)(now_ms - *last_update_ms) >=
                APP_TIME_UPDATE_MS)
            {
                *last_update_ms = now_ms;

                (*minutes)++;

                if (*minutes >= 60U)
                {
                    *minutes = 0U;
                    (*hours)++;

                    if (*hours >= 24U)
                    {
                        *hours = 0U;
                    }
                }

                status = seven_segment_set_time_hhmm(
                    &g_display,
                    0U,
                    *hours,
                    *minutes,
                    1U
                );
            }
            break;

        case 15U:
            if ((uint32_t)(now_ms - *last_update_ms) >=
                APP_DOT_UPDATE_MS)
            {
                *last_update_ms = now_ms;

                seven_segment_set_raw_at(
                    &g_display,
                    *raw_position,
                    0U
                );

                (*raw_position)++;

                if (*raw_position >= APP_DIGIT_COUNT)
                {
                    *raw_position = 0U;
                }

                switch (*raw_position)
                {
                    case 0U:
                        raw_pattern = SEVEN_SEGMENT_MASK_A;
                        break;

                    case 1U:
                        raw_pattern = SEVEN_SEGMENT_MASK_B |
                                      SEVEN_SEGMENT_MASK_C;
                        break;

                    case 2U:
                        raw_pattern = SEVEN_SEGMENT_MASK_G;
                        break;

                    default:
                        raw_pattern =
                            SEVEN_SEGMENT_ALL_SEGMENTS_MASK;
                        break;
                }

                seven_segment_set_raw_at(
                    &g_display,
                    *raw_position,
                    raw_pattern
                );
            }
            break;

        default:
            break;
    }

    return status;
}

#if defined(DRV_COMPILER_XC8)

void __interrupt() app_isr(void)
{
    seven_segment_irq_handler();
}

#endif

#if defined(DRV_COMPILER_C18)

#pragma code high_vector = 0x08

void app_high_vector(void)
{
    _asm
        goto app_high_isr
    _endasm
}

#pragma code

#pragma interrupt app_high_isr

void app_high_isr(void)
{
    seven_segment_irq_handler();
}

#endif

void main(void)
{
    uint8_t init_ok;
    uint8_t scene;
    uint8_t moving_dot;
    uint8_t blink_digit;
    uint8_t brightness;
    uint8_t hex_value;
    uint8_t hours;
    uint8_t minutes;
    uint8_t raw_position;

    int8_t brightness_step;

    uint16_t counter;

    uint32_t now_ms;
    uint32_t scene_started_ms;
    uint32_t last_update_ms;

    drv_status_t status;

    scene = 0U;
    moving_dot = 0U;
    blink_digit = 0U;
    brightness = APP_DEFAULT_BRIGHTNESS;
    brightness_step = -APP_BRIGHTNESS_STEP;
    hex_value = 0U;
    hours = 12U;
    minutes = 34U;
    raw_position = 0U;
    counter = 0U;

    now_ms = 0UL;
    scene_started_ms = 0UL;
    last_update_ms = 0UL;

    /*
     * Keep interrupts disabled during initialization.
     */
    INTCONbits.GIE = 0U;
    INTCONbits.PEIE = 0U;
    RCONbits.IPEN = 0U;

    /*
     * Digital GPIO mode.
     */
    ADCON1 = 0x07U;

#if defined(CMCON)
    CMCON = 0x07U;
#endif

    TRISEbits.PSPMODE = 0U;

    /*
     * Safe display output state.
     */
    LATD = 0x00U;
    TRISD = 0x00U;

    LATCbits.LATC0 = 1U;
    LATCbits.LATC1 = 1U;
    LATCbits.LATC2 = 1U;
    LATCbits.LATC3 = 1U;

    TRISCbits.TRISC0 = 0U;
    TRISCbits.TRISC1 = 0U;
    TRISCbits.TRISC2 = 0U;
    TRISCbits.TRISC3 = 0U;

    init_ok = seven_segment_init(
        &g_display,
        &g_display_config
    );

    if (init_ok == 0U)
    {
        app_fatal();
    }

    status = app_apply_scene(
        scene,
        &counter,
        &moving_dot,
        &blink_digit,
        &brightness,
        &brightness_step,
        &hex_value,
        &hours,
        &minutes,
        &raw_position
    );

    if (status != DRV_STATUS_OK)
    {
        app_fatal();
    }

    /*
     * Timer2 may have become pending during initialization.
     * Clear only that old request. Normal clearing is performed
     * inside the corrected seven_segment.c.
     */
    PIR1bits.TMR2IF = 0U;

    INTCONbits.PEIE = 1U;
    INTCONbits.GIE = 1U;

    while (
        seven_segment_get_refresh_count(
            &g_display
        ) == 0UL
    )
    {
        /*
         * Wait for the first actual Timer2 refresh.
         */
    }

    now_ms =
        seven_segment_get_refresh_count(
            &g_display
        );

    scene_started_ms = now_ms;
    last_update_ms = now_ms;

    while (1)
    {
        now_ms =
            seven_segment_get_refresh_count(
                &g_display
            );

        status = app_update_scene(
            scene,
            now_ms,
            &last_update_ms,
            &counter,
            &moving_dot,
            &blink_digit,
            &brightness,
            &brightness_step,
            &hex_value,
            &hours,
            &minutes,
            &raw_position
        );

        if (status != DRV_STATUS_OK)
        {
            app_fatal();
        }

        if ((uint32_t)(
                now_ms -
                scene_started_ms
            ) >= APP_SCENE_DURATION_MS)
        {
            scene_started_ms = now_ms;
            last_update_ms = now_ms;

            scene++;

            if (scene >= APP_SCENE_COUNT)
            {
                scene = 0U;
            }

            status = app_apply_scene(
                scene,
                &counter,
                &moving_dot,
                &blink_digit,
                &brightness,
                &brightness_step,
                &hex_value,
                &hours,
                &minutes,
                &raw_position
            );

            if (status != DRV_STATUS_OK)
            {
                app_fatal();
            }
        }
    }
}
