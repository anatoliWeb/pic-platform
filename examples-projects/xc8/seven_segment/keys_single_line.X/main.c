/*
 * File: examples-projects/xc8/seven_segment/keys_single_line.X/main.c
 */

#include "project_config.h"

#include "core/compiler.h"
#include "core/delay.h"

#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"

#include "drivers/timers/tick/tick.h"
#include "drivers/timers/timer1/timer1.h"

#include "libraries/display/seven_segment/seven_segment.h"
#include "libraries/input/button/button.h"
#include "libraries/input/segment_keys/segment_keys.h"

#if defined(DRV_COMPILER_C18)
#include <p18f452.h>
#endif

#if defined(DRV_COMPILER_XC8)
#include <xc.h>
#endif

/*
 * PIC18F452 DIP-40
 *
 * Four-digit common-cathode seven-segment display with buttons
 * connected to shared segment lines.
 *
 * Display wiring:
 *
 *   Segment A  -> RD0, DIP pin 19
 *   Segment B  -> RD1, DIP pin 20
 *   Segment C  -> RD2, DIP pin 21
 *   Segment D  -> RD3, DIP pin 22
 *   Segment E  -> RD4, DIP pin 27
 *   Segment F  -> RD5, DIP pin 28
 *   Segment G  -> RD6, DIP pin 29
 *   Decimal DP -> RD7, DIP pin 30
 *
 *   Digit 1 -> RC0, DIP pin 15
 *   Digit 2 -> RC1, DIP pin 16
 *   Digit 3 -> RC2, DIP pin 17
 *   Digit 4 -> RC3, DIP pin 18
 *
 * Buttons:
 *
 *   UP   -> segment A line, RD0
 *   DOWN -> segment B line, RD1
 *   OK   -> segment C line, RD2
 *
 * Button electrical connection:
 *
 *   +5V -> button -> 1 kOhm -> segment line
 *                               |
 *                              10 kOhm
 *                               |
 *                              GND
 *
 * Therefore the button active level is HIGH.
 *
 * Timer ownership:
 *
 *   Timer1 -> tick library
 *   Display -> manual refresh from main loop
 *
 * The display must not own Timer1 because Timer1 is already used
 * by tick_init().
 *
 * Required seven-segment configuration:
 *
 *   #define SEVEN_SEGMENT_ENABLE_TIMER0 0
 *   #define SEVEN_SEGMENT_ENABLE_TIMER1 0
 *   #define SEVEN_SEGMENT_ENABLE_TIMER2 0
 *   #define SEVEN_SEGMENT_ENABLE_TIMER3 0
 *
 * The following files must be present in the MPLAB project:
 *
 *   timer1.c
 *   tick.c
 *   seven_segment.c
 *   button.c
 *   segment_keys.c
 *   gpio.c
 *   delay.c
 *   uart.c
 *   uart_debug.c
 */

#define APP_DIGIT_COUNT                 4U
#define APP_KEY_COUNT                   3U

/*
 * Direct common-cathode digit control:
 *
 *   RCx = 0 -> digit enabled
 *   RCx = 1 -> digit disabled
 *
 * Change to 1U only if external digit transistors invert the signal.
 */
#define APP_DIGIT_ACTIVE_LEVEL          0U

/*
 * Electrical level seen on the shared segment line while pressed.
 *
 * The external button connects the segment line to +5 V, therefore
 * segment_keys must treat HIGH as the active electrical level.
 */
#define APP_SEGMENT_KEY_ACTIVE_LEVEL    1U

/*
 * Initial raw state passed to the button library.
 *
 * button uses active-low logic internally:
 *   0 = pressed
 *   1 = released
 *
 * The application starts with all buttons released.
 */
#define APP_BUTTON_INITIAL_RAW_STATE    1U

#define APP_INITIAL_VALUE               7U
#define APP_MAX_VALUE                9999U

#define APP_INITIAL_ACTIVE_DIGIT        3U
#define APP_INITIAL_BRIGHTNESS         60U

#define APP_BLINK_PERIOD_MS           600U
#define APP_DIGIT_SELECT_PERIOD_MS   3000UL
#define APP_MAIN_LOOP_DELAY_MS          1U

#define APP_UART_BAUD_RATE           9600UL
#define APP_DEBUG_STATUS_PERIOD_MS   1000UL

/*
 * Ensure this manual-refresh example is not accidentally compiled
 * with a seven-segment timer backend enabled.
 */
#if defined(SEVEN_SEGMENT_ENABLE_TIMER0) && \
    (SEVEN_SEGMENT_ENABLE_TIMER0 != 0)
#error "Disable SEVEN_SEGMENT_ENABLE_TIMER0 for this example."
#endif

#if defined(SEVEN_SEGMENT_ENABLE_TIMER1) && \
    (SEVEN_SEGMENT_ENABLE_TIMER1 != 0)
#error "Disable SEVEN_SEGMENT_ENABLE_TIMER1. Timer1 belongs to tick."
#endif

#if defined(SEVEN_SEGMENT_ENABLE_TIMER2) && \
    (SEVEN_SEGMENT_ENABLE_TIMER2 != 0)
#error "Disable SEVEN_SEGMENT_ENABLE_TIMER2 for this example."
#endif

#if defined(SEVEN_SEGMENT_ENABLE_TIMER3) && \
    (SEVEN_SEGMENT_ENABLE_TIMER3 != 0)
#error "Disable SEVEN_SEGMENT_ENABLE_TIMER3 for this example."
#endif

static seven_segment_t g_display;
static segment_keys_t g_keys;

static button_t g_button_up;
static button_t g_button_down;
static button_t g_button_ok;

/*
 * Value change associated with each selected digit.
 *
 * Digit 0 -> thousands
 * Digit 1 -> hundreds
 * Digit 2 -> tens
 * Digit 3 -> units
 */
static const uint16_t g_digit_steps[APP_DIGIT_COUNT] =
{
    1000U,
    100U,
    10U,
    1U
};

/*
 * PORT registers are intentionally used in pin descriptors.
 *
 * The GPIO and segment-key libraries change pin directions and read
 * shared segment lines through these pointers. XC8 cannot generate
 * the required indirect GPIO operations when LAT registers are used
 * in this structure.
 *
 * LAT registers are still used later for direct initial output writes.
 */
static const seven_segment_pin_t g_digit_pins[APP_DIGIT_COUNT] =
{
    { &PORTC, &TRISC, 0U },
    { &PORTC, &TRISC, 1U },
    { &PORTC, &TRISC, 2U },
    { &PORTC, &TRISC, 3U }
};

static const seven_segment_config_t g_display_config =
{
    { &PORTD, &TRISD, 0U }, /* Segment A. */
    { &PORTD, &TRISD, 1U }, /* Segment B. */
    { &PORTD, &TRISD, 2U }, /* Segment C. */
    { &PORTD, &TRISD, 3U }, /* Segment D. */
    { &PORTD, &TRISD, 4U }, /* Segment E. */
    { &PORTD, &TRISD, 5U }, /* Segment F. */
    { &PORTD, &TRISD, 6U }, /* Segment G. */

    { &PORTD, &TRISD, 7U }, /* Decimal point. */
    1U,                     /* Decimal point is present. */

    g_digit_pins,
    APP_DIGIT_COUNT,
    APP_DIGIT_ACTIVE_LEVEL,

    /*
     * Use the default library symbol table.
     */
    (const uint8_t *)0,
    0U,

    SEVEN_SEGMENT_COMMON_CATHODE,

    /*
     * Display refresh is performed manually in the main loop.
     * No hardware timer is owned by seven_segment.
     */
    SEVEN_SEGMENT_REFRESH_MANUAL,
    SEVEN_SEGMENT_TIMER_NONE,
    0U
};

/*
 * Buttons share the segment lines.
 *
 * The character is a logical key identifier.
 */
static const segment_keys_entry_t g_key_entries[APP_KEY_COUNT] =
{
    {
        SEVEN_SEGMENT_MASK_A,
        'U',
        &g_button_up
    },
    {
        SEVEN_SEGMENT_MASK_B,
        'D',
        &g_button_down
    },
    {
        SEVEN_SEGMENT_MASK_C,
        'O',
        &g_button_ok
    }
};

/*
 * Scan buttons while using physical display digit 0.
 */
static const segment_keys_config_t g_keys_config =
{
    &g_display,
    g_key_entries,
    APP_KEY_COUNT,

    /*
     * Pressed button drives the shared segment line HIGH.
     */
    APP_SEGMENT_KEY_ACTIVE_LEVEL
};

static void app_fatal(const char *message)
{
    /*
     * Put display outputs into a safe state.
     */
    LATD = 0x00U;

    /*
     * Digits are active LOW, therefore HIGH disables them.
     */
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
        /*
         * Fatal error.
         */
    }
}

static void app_apply_display(
    uint16_t value,
    uint8_t leading_zero,
    seven_segment_alignment_t alignment,
    uint8_t active_digit)
{
    seven_segment_number_format_t format;
    drv_status_t status;
    uint8_t digit;

    if (active_digit >= APP_DIGIT_COUNT)
    {
        active_digit =
            (uint8_t)(APP_DIGIT_COUNT - 1U);
    }

    format.start_digit = 0U;
    format.width = APP_DIGIT_COUNT;
    format.alignment = alignment;
    format.leading_zero = leading_zero;
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

    /*
     * Clear decimal points on all digits.
     */
    for (digit = 0U;
         digit < APP_DIGIT_COUNT;
         digit++)
    {
        seven_segment_set_dot_at(
            &g_display,
            digit,
            0U
        );
    }

    /*
     * Blink the currently selected digit.
     */
    seven_segment_set_blink_mask(
        &g_display,
        (uint16_t)(
            (uint16_t)1U << active_digit
        )
    );

    seven_segment_set_blink_enabled(
        &g_display,
        1U
    );
}

static void app_step_value(
    uint16_t *value,
    uint8_t active_digit,
    int8_t direction)
{
    uint16_t step;
    uint32_t next_value;

    if (value == (uint16_t *)0)
    {
        return;
    }

    if (active_digit >= APP_DIGIT_COUNT)
    {
        active_digit =
            (uint8_t)(APP_DIGIT_COUNT - 1U);
    }

    step = g_digit_steps[active_digit];

    if (direction > 0)
    {
        next_value =
            (uint32_t)(*value) +
            (uint32_t)step;

        if (next_value > APP_MAX_VALUE)
        {
            *value = APP_MAX_VALUE;
        }
        else
        {
            *value = (uint16_t)next_value;
        }
    }
    else if (direction < 0)
    {
        if (*value >= step)
        {
            *value =
                (uint16_t)(*value - step);
        }
        else
        {
            *value = 0U;
        }
    }
}

static void app_cycle_brightness(
    uint8_t *brightness)
{
    static const uint8_t levels[5] =
    {
        20U,
        40U,
        60U,
        80U,
        100U
    };

    uint8_t index;

    if (brightness == (uint8_t *)0)
    {
        return;
    }

    for (index = 0U; index < 5U; index++)
    {
        if (levels[index] == *brightness)
        {
            index++;

            if (index >= 5U)
            {
                index = 0U;
            }

            *brightness = levels[index];

            seven_segment_set_brightness(
                &g_display,
                *brightness
            );

            return;
        }
    }

    /*
     * Restore a valid default level.
     */
    *brightness = APP_INITIAL_BRIGHTNESS;

    seven_segment_set_brightness(
        &g_display,
        *brightness
    );
}

/*
 * Timer1 interrupt is owned by the tick library.
 */
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
    uint8_t leading_zero;
    seven_segment_alignment_t alignment;
    uint8_t active_digit;
    uint8_t brightness;

    uint32_t last_select_ms;
    uint32_t last_debug_status_ms;
    uint32_t now_ms;

    uint8_t display_init_result;
    uint8_t keys_init_result;
    uint16_t previous_raw_mask;
    uint16_t raw_mask;

    value = APP_INITIAL_VALUE;
    leading_zero = 0U;
    alignment = SEVEN_SEGMENT_ALIGN_RIGHT;
    active_digit = APP_INITIAL_ACTIVE_DIGIT;
    brightness = APP_INITIAL_BRIGHTNESS;

    last_select_ms = 0UL;
    last_debug_status_ms = 0UL;
    now_ms = 0UL;

    previous_raw_mask = 0xFFFFU;
    raw_mask = 0U;

    /*
     * Disable interrupt gates during initialization.
     */
    INTCONbits.GIE = 0U;
    INTCONbits.PEIE = 0U;

    /*
     * Use the single interrupt vector.
     */
    RCONbits.IPEN = 0U;

    /*
     * Configure analog-capable pins as digital GPIO.
     */
    ADCON1 = 0x07U;

#if defined(CMCON)
    /*
     * Disable analog comparators.
     */
    CMCON = 0x07U;
#endif

    /*
     * Disable Parallel Slave Port mode before using PORTD.
     */
    TRISEbits.PSPMODE = 0U;

    /*
     * Safe initial segment state.
     *
     * Common-cathode segments are off at LOW.
     */
    LATD = 0x00U;
    TRISD = 0x00U;

    /*
     * Disable all digits before initialization.
     */
    LATCbits.LATC0 = 1U;
    LATCbits.LATC1 = 1U;
    LATCbits.LATC2 = 1U;
    LATCbits.LATC3 = 1U;

    TRISCbits.TRISC0 = 0U;
    TRISCbits.TRISC1 = 0U;
    TRISCbits.TRISC2 = 0U;
    TRISCbits.TRISC3 = 0U;

    /*
     * Initialize UART debug output.
     *
     * Proteus connection:
     *   PIC RC6/TX, DIP pin 25 -> Virtual Terminal RXD
     *   9600 baud, 8 data bits, no parity, 1 stop bit
     */
    uart_init(APP_UART_BAUD_RATE);

    DBG_PRINTLN("");
    DBG_PRINTLN("PIC18F452 segment keys UART debug");
    DBG_PRINTLN("BOOT");
    DBG_PRINT("PORTD=");
    DBG_PRINT_INT((int)PORTD);
    DBG_PRINT(" TRISD=");
    DBG_PRINT_INT((int)TRISD);
    DBG_PRINTLN("");

    /*
     * Initialize the display in manual-refresh mode.
     */
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

    DBG_PRINTLN("DISPLAY INIT OK");

    seven_segment_set_brightness(
        &g_display,
        brightness
    );

    seven_segment_set_blink_period_ms(
        &g_display,
        APP_BLINK_PERIOD_MS
    );

    app_apply_display(
        value,
        leading_zero,
        alignment,
        active_digit
    );

    /*
     * Button state is supplied externally by segment_keys_update().
     *
     * Active level HIGH matches the recommended pull-down wiring.
     */
    button_init_external(
        &g_button_up,
        APP_BUTTON_INITIAL_RAW_STATE
    );

    button_init_external(
        &g_button_down,
        APP_BUTTON_INITIAL_RAW_STATE
    );

    button_init_external(
        &g_button_ok,
        APP_BUTTON_INITIAL_RAW_STATE
    );

    /*
     * Initialize button scanning through shared segment lines.
     */
    keys_init_result = (uint8_t)segment_keys_init(
        &g_keys,
        &g_keys_config
    );

    DBG_PRINT("KEYS INIT result=");
    DBG_PRINT_INT((int)keys_init_result);
    DBG_PRINT(" active_level=");
    DBG_PRINT_INT((int)APP_SEGMENT_KEY_ACTIVE_LEVEL);
    DBG_PRINT(" button_initial_raw=");
    DBG_PRINT_INT((int)APP_BUTTON_INITIAL_RAW_STATE);
    DBG_PRINTLN("");

    if (keys_init_result == 0U)
    {
        app_fatal("segment keys init");
    }

    /*
     * Initialize the millisecond tick.
     *
     * tick_init() configures and starts Timer1.
     */
    tick_init();

    /*
     * Clear a possible pending Timer1 flag before enabling
     * global interrupts.
     */
    PIR1bits.TMR1IF = 0U;

    INTCONbits.PEIE = 1U;
    INTCONbits.GIE = 1U;

    DBG_PRINTLN("TICK STARTED");
    DBG_PRINTLN("Press UP/DOWN/OK");
    DBG_PRINTLN("pressed mask: UP=1 DOWN=2 OK=4");

    while (1)
    {
        /*
         * Manual display multiplexing.
         */
        seven_segment_process(
            &g_display
        );

        /*
         * Scan buttons connected to the shared segment lines.
         */
        segment_keys_update(
            &g_keys
        );

        /*
         * Print the physical key mask only when it changes.
         * This helps separate wiring/scan issues from button-event issues.
         */
        raw_mask = segment_keys_get_raw_mask(
            &g_keys
        );

        if (raw_mask != previous_raw_mask)
        {
            previous_raw_mask = raw_mask;

            DBG_PRINT("RAW MASK=");
            DBG_PRINT_INT((int)raw_mask);
            DBG_PRINT(" PORTD=");
            DBG_PRINT_INT((int)PORTD);
            DBG_PRINT(" TRISD=");
            DBG_PRINT_INT((int)TRISD);

            if ((raw_mask & SEVEN_SEGMENT_MASK_A) != 0U)
            {
                DBG_PRINT(" UP_RAW");
            }

            if ((raw_mask & SEVEN_SEGMENT_MASK_B) != 0U)
            {
                DBG_PRINT(" DOWN_RAW");
            }

            if ((raw_mask & SEVEN_SEGMENT_MASK_C) != 0U)
            {
                DBG_PRINT(" OK_RAW");
            }

            DBG_PRINTLN("");
        }

        /*
         * Keep the manual refresh and key scan period close to 1 ms.
         */
        delay_ms(
            APP_MAIN_LOOP_DELAY_MS
        );

        now_ms = tick_get();

        /*
         * Blink timing uses Timer1's millisecond tick.
         */
        seven_segment_blink_update(
            &g_display,
            now_ms
        );

        if ((uint32_t)(
                now_ms -
                last_debug_status_ms
            ) >= APP_DEBUG_STATUS_PERIOD_MS)
        {
            last_debug_status_ms = now_ms;

            DBG_PRINT("ALIVE ms=");
            DBG_PRINT_INT((int)(now_ms & 0x7FFFUL));
            DBG_PRINT(" raw=");
            DBG_PRINT_INT((int)raw_mask);
            DBG_PRINT(" value=");
            DBG_PRINT_INT((int)value);
            DBG_PRINT(" digit=");
            DBG_PRINT_INT((int)active_digit);
            DBG_PRINTLN("");
        }

        /*
         * Automatically select the next editable digit
         * every three seconds.
         */
        if ((uint32_t)(
                now_ms -
                last_select_ms
            ) >= APP_DIGIT_SELECT_PERIOD_MS)
        {
            last_select_ms = now_ms;

            active_digit++;

            if (active_digit >= APP_DIGIT_COUNT)
            {
                active_digit = 0U;
            }

            app_apply_display(
                value,
                leading_zero,
                alignment,
                active_digit
            );
        }

        /*
         * UP single click.
         */
        if (button_clicked(
                &g_button_up
            ) != 0U)
        {
            DBG_PRINTLN("EVENT: UP CLICK");

            app_step_value(
                &value,
                active_digit,
                1
            );

            app_apply_display(
                value,
                leading_zero,
                alignment,
                active_digit
            );
        }

        /*
         * DOWN single click.
         */
        if (button_clicked(
                &g_button_down
            ) != 0U)
        {
            DBG_PRINTLN("EVENT: DOWN CLICK");

            app_step_value(
                &value,
                active_digit,
                -1
            );

            app_apply_display(
                value,
                leading_zero,
                alignment,
                active_digit
            );
        }

        /*
         * Repeated UP while held.
         */
        if (button_hold_repeated(
                &g_button_up
            ) != 0U)
        {
            DBG_PRINTLN("EVENT: UP HOLD REPEAT");

            app_step_value(
                &value,
                active_digit,
                1
            );

            app_apply_display(
                value,
                leading_zero,
                alignment,
                active_digit
            );
        }

        /*
         * Repeated DOWN while held.
         */
        if (button_hold_repeated(
                &g_button_down
            ) != 0U)
        {
            DBG_PRINTLN("EVENT: DOWN HOLD REPEAT");

            app_step_value(
                &value,
                active_digit,
                -1
            );

            app_apply_display(
                value,
                leading_zero,
                alignment,
                active_digit
            );
        }

        /*
         * OK double click:
         * toggle left/right number alignment.
         */
        if (button_double_clicked(
                &g_button_ok
            ) != 0U)
        {
            DBG_PRINTLN("EVENT: OK DOUBLE CLICK");

            if (alignment ==
                SEVEN_SEGMENT_ALIGN_RIGHT)
            {
                alignment =
                    SEVEN_SEGMENT_ALIGN_LEFT;
            }
            else
            {
                alignment =
                    SEVEN_SEGMENT_ALIGN_RIGHT;
            }

            app_apply_display(
                value,
                leading_zero,
                alignment,
                active_digit
            );
        }
        /*
         * OK hold:
         * cycle through brightness levels.
         */
        else if (button_hold_repeated(
                     &g_button_ok
                 ) != 0U)
        {
            DBG_PRINTLN("EVENT: OK HOLD REPEAT");

            app_cycle_brightness(
                &brightness
            );

            app_apply_display(
                value,
                leading_zero,
                alignment,
                active_digit
            );
        }
        /*
         * OK single click:
         * enable or disable leading zeroes.
         */
        else if (button_clicked(
                     &g_button_ok
                 ) != 0U)
        {
            DBG_PRINTLN("EVENT: OK CLICK");

            leading_zero =
                (uint8_t)(
                    leading_zero == 0U
                    ? 1U
                    : 0U
                );

            app_apply_display(
                value,
                leading_zero,
                alignment,
                active_digit
            );
        }
    }
}