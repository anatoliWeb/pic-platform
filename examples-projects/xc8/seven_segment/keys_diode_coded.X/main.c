/*
 * File: examples-projects/xc8/seven_segment/keys_diode_coded.X/main.c
 */

#include "project_config.h"

#include "core/compiler.h"
#include "core/delay.h"

#include "drivers/communication/uart/uart.h"
#include "drivers/timers/tick/tick.h"
#include "drivers/timers/timer1/timer1.h"

#include "libraries/display/seven_segment/seven_segment.h"
#include "libraries/input/button/button.h"
#include "libraries/input/segment_keys/segment_keys.h"
#include "libraries/system/uart_debug/uart_debug.h"

#if defined(DRV_COMPILER_C18)
#include <p18f452.h>
#endif

#if defined(DRV_COMPILER_XC8)
#include <xc.h>
#endif

/*
 * PIC18F452, 10 MHz.
 *
 * Display:
 *   A..DP -> RD0..RD7
 *   DIG1..DIG4 -> RC0..RC3
 *
 * Diode-coded buttons:
 *   A         -> select next editable digit
 *   A + B     -> increment selected digit
 *   A + B + C -> decrement selected digit
 *
 * The decoder accumulates every observed bit during one physical press
 * and performs the action only after a stable release. This prevents a
 * coded A+B press from being misread as alternating A and A+B events.
 */

#define APP_DIGIT_COUNT                  4U
#define APP_KEY_COUNT                    3U

#define APP_DIGIT_ACTIVE_LEVEL           0U
#define APP_SEGMENT_KEY_ACTIVE_LEVEL     1U
#define APP_BUTTON_INITIAL_RAW_STATE     1U

#define APP_INITIAL_VALUE             1234U
#define APP_INITIAL_ACTIVE_DIGIT         3U
#define APP_INITIAL_BRIGHTNESS          70U

#define APP_MAIN_LOOP_DELAY_MS           1U
#define APP_BLINK_PERIOD_MS            700U
#define APP_RELEASE_DEBOUNCE_MS         50UL
#define APP_DEBUG_STATUS_PERIOD_MS    1000UL
#define APP_UART_BAUD_RATE            9600UL

#define APP_KEY_MASK_A \
    ((uint8_t)SEVEN_SEGMENT_MASK_A)

#define APP_KEY_MASK_AB \
    ((uint8_t)(SEVEN_SEGMENT_MASK_A | SEVEN_SEGMENT_MASK_B))

#define APP_KEY_MASK_ABC \
    ((uint8_t)(SEVEN_SEGMENT_MASK_A | \
               SEVEN_SEGMENT_MASK_B | \
               SEVEN_SEGMENT_MASK_C))

#if defined(SEVEN_SEGMENT_ENABLE_TIMER0) && \
    (SEVEN_SEGMENT_ENABLE_TIMER0 != 0)
#error "Disable SEVEN_SEGMENT_ENABLE_TIMER0."
#endif

#if defined(SEVEN_SEGMENT_ENABLE_TIMER1) && \
    (SEVEN_SEGMENT_ENABLE_TIMER1 != 0)
#error "Disable SEVEN_SEGMENT_ENABLE_TIMER1. Timer1 belongs to tick."
#endif

#if defined(SEVEN_SEGMENT_ENABLE_TIMER2) && \
    (SEVEN_SEGMENT_ENABLE_TIMER2 != 0)
#error "Disable SEVEN_SEGMENT_ENABLE_TIMER2."
#endif

#if defined(SEVEN_SEGMENT_ENABLE_TIMER3) && \
    (SEVEN_SEGMENT_ENABLE_TIMER3 != 0)
#error "Disable SEVEN_SEGMENT_ENABLE_TIMER3."
#endif

static seven_segment_t g_display;
static segment_keys_t g_keys;

static button_t g_button_select;
static button_t g_button_plus;
static button_t g_button_minus;

static const uint16_t g_digit_steps[APP_DIGIT_COUNT] =
{
    1000U,
    100U,
    10U,
    1U
};

static const seven_segment_pin_t g_digit_pins[APP_DIGIT_COUNT] =
{
    { &PORTC, &TRISC, 0U },
    { &PORTC, &TRISC, 1U },
    { &PORTC, &TRISC, 2U },
    { &PORTC, &TRISC, 3U }
};

static const seven_segment_config_t g_display_config =
{
    { &PORTD, &TRISD, 0U },
    { &PORTD, &TRISD, 1U },
    { &PORTD, &TRISD, 2U },
    { &PORTD, &TRISD, 3U },
    { &PORTD, &TRISD, 4U },
    { &PORTD, &TRISD, 5U },
    { &PORTD, &TRISD, 6U },

    { &PORTD, &TRISD, 7U },
    1U,

    g_digit_pins,
    APP_DIGIT_COUNT,
    APP_DIGIT_ACTIVE_LEVEL,

    (const uint8_t *)0,
    0U,

    SEVEN_SEGMENT_COMMON_CATHODE,

    SEVEN_SEGMENT_REFRESH_MANUAL,
    SEVEN_SEGMENT_TIMER_NONE,
    0U
};

static const segment_keys_entry_t g_key_entries[APP_KEY_COUNT] =
{
    {
        APP_KEY_MASK_A,
        'S',
        &g_button_select
    },
    {
        APP_KEY_MASK_AB,
        '+',
        &g_button_plus
    },
    {
        APP_KEY_MASK_ABC,
        '-',
        &g_button_minus
    }
};

static const segment_keys_config_t g_keys_config =
{
    &g_display,
    g_key_entries,
    APP_KEY_COUNT,
    APP_SEGMENT_KEY_ACTIVE_LEVEL
};

static void app_fatal(const char *message)
{
    LATD = 0x00U;

    LATCbits.LATC0 = 1U;
    LATCbits.LATC1 = 1U;
    LATCbits.LATC2 = 1U;
    LATCbits.LATC3 = 1U;

    DBG_PRINT("FATAL: ");

    if (message != (const char *)0)
    {
        DBG_PRINTLN(message);
    }
    else
    {
        DBG_PRINTLN("unknown");
    }

    while (1)
    {
    }
}

static void app_apply_display(
    uint16_t value,
    uint8_t active_digit)
{
    seven_segment_number_format_t format;
    drv_status_t status;
    uint8_t digit;

    if (active_digit >= APP_DIGIT_COUNT)
    {
        active_digit = 0U;
    }

    format.start_digit = 0U;
    format.width = APP_DIGIT_COUNT;
    format.alignment = SEVEN_SEGMENT_ALIGN_RIGHT;

    /*
     * Keep all four digits visible, including zeroes.
     */
    format.leading_zero = 1U;
    format.clear_unused = 1U;

    status = seven_segment_set_number_formatted(
        &g_display,
        (int32_t)value,
        &format
    );

    if (status != DRV_STATUS_OK)
    {
        app_fatal("display update");
    }

    for (digit = 0U; digit < APP_DIGIT_COUNT; digit++)
    {
        seven_segment_set_dot_at(
            &g_display,
            digit,
            0U
        );
    }

    /*
     * Exactly one digit blinks.
     */
    seven_segment_set_blink_mask(
        &g_display,
        (uint16_t)((uint16_t)1U << active_digit)
    );

    seven_segment_set_blink_enabled(
        &g_display,
        1U
    );
}

static void app_change_selected_digit(
    uint16_t *value,
    uint8_t active_digit,
    int8_t direction)
{
    uint16_t step;
    uint16_t current_digit;
    uint16_t new_digit;

    if ((value == (uint16_t *)0) ||
        (active_digit >= APP_DIGIT_COUNT))
    {
        return;
    }

    step = g_digit_steps[active_digit];

    current_digit =
        (uint16_t)(
            ((uint16_t)(*value / step)) % 10U
        );

    if (direction > 0)
    {
        new_digit =
            (uint16_t)(
                current_digit >= 9U
                ? 0U
                : current_digit + 1U
            );
    }
    else
    {
        new_digit =
            (uint16_t)(
                current_digit == 0U
                ? 9U
                : current_digit - 1U
            );
    }

    *value =
        (uint16_t)(
            *value -
            (uint16_t)(current_digit * step) +
            (uint16_t)(new_digit * step)
        );
}

static void app_execute_key(
    uint8_t key_mask,
    uint16_t *value,
    uint8_t *active_digit)
{
    if ((value == (uint16_t *)0) ||
        (active_digit == (uint8_t *)0))
    {
        return;
    }

    if (key_mask == APP_KEY_MASK_A)
    {
        (*active_digit)++;

        if (*active_digit >= APP_DIGIT_COUNT)
        {
            *active_digit = 0U;
        }

        DBG_PRINT("ACTION: SELECT digit=");
        DBG_PRINT_INT((int)(*active_digit));
        DBG_PRINTLN("");
    }
    else if (key_mask == APP_KEY_MASK_AB)
    {
        app_change_selected_digit(
            value,
            *active_digit,
            1
        );

        DBG_PRINT("ACTION: PLUS digit=");
        DBG_PRINT_INT((int)(*active_digit));
        DBG_PRINT(" value=");
        DBG_PRINT_INT((int)(*value));
        DBG_PRINTLN("");
    }
    else if (key_mask == APP_KEY_MASK_ABC)
    {
        app_change_selected_digit(
            value,
            *active_digit,
            -1
        );

        DBG_PRINT("ACTION: MINUS digit=");
        DBG_PRINT_INT((int)(*active_digit));
        DBG_PRINT(" value=");
        DBG_PRINT_INT((int)(*value));
        DBG_PRINTLN("");
    }
    else
    {
        DBG_PRINT("IGNORED accumulated mask=");
        DBG_PRINT_INT((int)key_mask);
        DBG_PRINTLN("");
        return;
    }

    app_apply_display(
        *value,
        *active_digit
    );
}

#if defined(DRV_COMPILER_XC8)

void __interrupt() app_isr(void)
{
    timer1_irq_handler();
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
    timer1_irq_handler();
}

#endif

void main(void)
{
    uint16_t value;
    uint8_t active_digit;

    uint8_t display_init_result;
    uint8_t keys_init_result;

    uint8_t raw_mask;
    uint8_t previous_raw_mask;
    uint8_t accumulated_mask;

    uint8_t press_active;
    uint8_t release_pending;

    uint32_t release_started_ms;
    uint32_t last_debug_status_ms;
    uint32_t now_ms;

    value = APP_INITIAL_VALUE;
    active_digit = APP_INITIAL_ACTIVE_DIGIT;

    raw_mask = 0U;
    previous_raw_mask = 0xFFU;
    accumulated_mask = 0U;

    press_active = 0U;
    release_pending = 0U;

    release_started_ms = 0UL;
    last_debug_status_ms = 0UL;
    now_ms = 0UL;

    INTCONbits.GIE = 0U;
    INTCONbits.PEIE = 0U;
    RCONbits.IPEN = 0U;

    ADCON1 = 0x07U;

#if defined(CMCON)
    CMCON = 0x07U;
#endif

    TRISEbits.PSPMODE = 0U;

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

    uart_init(APP_UART_BAUD_RATE);

    DBG_PRINTLN("");
    DBG_PRINTLN("PIC18F452 diode-coded digit editor");
    DBG_PRINTLN("A=select, A+B=plus, A+B+C=minus");
    DBG_PRINTLN("Actions execute after release");

    display_init_result = seven_segment_init(
        &g_display,
        &g_display_config
    );

    if (display_init_result == 0U)
    {
        DBG_PRINT("DISPLAY INIT FAILED code=");
        DBG_PRINT_INT((int)seven_segment_get_init_error(&g_display));
        DBG_PRINTLN("");

        app_fatal("display init");
    }

    seven_segment_set_brightness(
        &g_display,
        APP_INITIAL_BRIGHTNESS
    );

    seven_segment_set_blink_period_ms(
        &g_display,
        APP_BLINK_PERIOD_MS
    );

    app_apply_display(
        value,
        active_digit
    );

    button_init_external(
        &g_button_select,
        APP_BUTTON_INITIAL_RAW_STATE
    );

    button_init_external(
        &g_button_plus,
        APP_BUTTON_INITIAL_RAW_STATE
    );

    button_init_external(
        &g_button_minus,
        APP_BUTTON_INITIAL_RAW_STATE
    );

    keys_init_result = segment_keys_init(
        &g_keys,
        &g_keys_config
    );

    if (keys_init_result == 0U)
    {
        app_fatal("segment keys init");
    }

    tick_init();

    PIR1bits.TMR1IF = 0U;

    INTCONbits.PEIE = 1U;
    INTCONbits.GIE = 1U;

    DBG_PRINTLN("READY");

    while (1)
    {
        seven_segment_process(
            &g_display
        );

        segment_keys_update(
            &g_keys
        );

        delay_ms(
            APP_MAIN_LOOP_DELAY_MS
        );

        now_ms = tick_get();

        seven_segment_blink_update(
            &g_display,
            now_ms
        );

        raw_mask =
            (uint8_t)(
                segment_keys_get_raw_mask(
                    &g_keys
                ) &
                APP_KEY_MASK_ABC
            );

        if (raw_mask != previous_raw_mask)
        {
            previous_raw_mask = raw_mask;

            DBG_PRINT("RAW=");
            DBG_PRINT_INT((int)raw_mask);
            DBG_PRINT(" ACC=");
            DBG_PRINT_INT((int)accumulated_mask);
            DBG_PRINTLN("");
        }

        if (raw_mask != 0U)
        {
            /*
             * Keep every bit seen during this physical press.
             * A noisy A+B key may alternate between 1 and 3, but the
             * accumulated mask remains 3.
             */
            accumulated_mask =
                (uint8_t)(
                    accumulated_mask |
                    raw_mask
                );

            press_active = 1U;
            release_pending = 0U;
        }
        else if (press_active != 0U)
        {
            if (release_pending == 0U)
            {
                release_pending = 1U;
                release_started_ms = now_ms;
            }
            else if ((uint32_t)(
                         now_ms -
                         release_started_ms
                     ) >= APP_RELEASE_DEBOUNCE_MS)
            {
                DBG_PRINT("RELEASE action mask=");
                DBG_PRINT_INT((int)accumulated_mask);
                DBG_PRINTLN("");

                app_execute_key(
                    accumulated_mask,
                    &value,
                    &active_digit
                );

                accumulated_mask = 0U;
                press_active = 0U;
                release_pending = 0U;
            }
        }

        if ((uint32_t)(
                now_ms -
                last_debug_status_ms
            ) >= APP_DEBUG_STATUS_PERIOD_MS)
        {
            last_debug_status_ms = now_ms;

            DBG_PRINT("ALIVE value=");
            DBG_PRINT_INT((int)value);
            DBG_PRINT(" digit=");
            DBG_PRINT_INT((int)active_digit);
            DBG_PRINT(" raw=");
            DBG_PRINT_INT((int)raw_mask);
            DBG_PRINT(" acc=");
            DBG_PRINT_INT((int)accumulated_mask);
            DBG_PRINTLN("");
        }
    }
}
