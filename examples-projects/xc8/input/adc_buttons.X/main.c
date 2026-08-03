/*
 * File: examples-projects/xc8/adc_buttons.X/main.c
 */

#include "project_config.h"
#include "core/compiler.h"
#include "drivers/analog/adc/adc.h"
#include "drivers/timers/tick/tick.h"
#include "drivers/timers/timer1/timer1.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/input/adc_buttons/adc_buttons.h"
#include "libraries/system/uart_debug/uart_debug.h"

#define ADC_BUTTON_CHANNEL  0u

#define ADC_BUTTON_NONE     0u
#define ADC_BUTTON_1        1u
#define ADC_BUTTON_2        2u
#define ADC_BUTTON_3        3u
#define ADC_BUTTON_4        4u

/*
 * Example ADC button ladder for AN0.
 * These ranges are based on observed ADC0 values:
 * Button 1 ~= 6016
 * Button 2 ~= 11264
 * Button 3 ~= 16256
 * Button 4 ~= 21120
 */
static const adc_button_range_t g_adc_button_table[] =
{
    {  5000u,  7500u, ADC_BUTTON_1 },
    { 10000u, 12500u, ADC_BUTTON_2 },
    { 15000u, 17500u, ADC_BUTTON_3 },
    { 20000u, 22500u, ADC_BUTTON_4 }
};

static adc_buttons_t g_buttons;

static uint8_t g_last_button = ADC_BUTTON_NONE;
static uint16_t g_last_adc_raw = 0u;

static uint8_t g_led_1_state = 0u;
static uint8_t g_led_2_state = 0u;
static uint8_t g_led_3_state = 0u;
static uint8_t g_led_4_state = 0u;

/*
 * Timer1 interrupt is required by tick.c.
 * adc_buttons debounce logic depends on tick_get().
 */
void __interrupt() app_isr(void)
{
    timer1_irq_handler();
}

static void app_io_init(void)
{
    /*
     * AN0 / RA0 = ADC input.
     * RB0..RB3 = LED outputs.
     */
    TRISAbits.TRISA0 = 1u;

    TRISBbits.TRISB0 = 0u;
    TRISBbits.TRISB1 = 0u;
    TRISBbits.TRISB2 = 0u;
    TRISBbits.TRISB3 = 0u;

    LATBbits.LATB0 = 0u;
    LATBbits.LATB1 = 0u;
    LATBbits.LATB2 = 0u;
    LATBbits.LATB3 = 0u;
}

static void app_apply_led_states(void)
{
    LATBbits.LATB0 = g_led_1_state;
    LATBbits.LATB1 = g_led_2_state;
    LATBbits.LATB2 = g_led_3_state;
    LATBbits.LATB3 = g_led_4_state;
}

static void app_print_led_states(void)
{
    DBG_PRINT("LED states=");
    DBG_PRINT_INT((int)g_led_1_state);
    DBG_PRINT(",");
    DBG_PRINT_INT((int)g_led_2_state);
    DBG_PRINT(",");
    DBG_PRINT_INT((int)g_led_3_state);
    DBG_PRINT(",");
    DBG_PRINT_INT((int)g_led_4_state);
    DBG_PRINTLN("");
}

static void app_print_adc_change(uint16_t raw)
{
    uint16_t millivolts;

    if ((raw != 0u) && (raw != g_last_adc_raw))
    {
        millivolts = adc_read_voltage(ADC_BUTTON_CHANNEL);

        DBG_PRINT("ADC changed: ADC0=");
        DBG_PRINT_INT((int)raw);

        DBG_PRINT(" mV=");
        DBG_PRINT_INT((int)millivolts);

        DBG_PRINT(" tick=");
        DBG_PRINT_INT((int)tick_get());

        DBG_PRINTLN("");
    }

    g_last_adc_raw = raw;
}

static void app_print_button_change(uint8_t previous_button, uint8_t current_button)
{
    DBG_PRINT("button changed: ");

    DBG_PRINT_INT((int)previous_button);
    DBG_PRINT(" -> ");
    DBG_PRINT_INT((int)current_button);

    if (current_button == ADC_BUTTON_NONE)
    {
        DBG_PRINT(" released");
    }
    else
    {
        DBG_PRINT(" pressed ADC_BUTTON_");
        DBG_PRINT_INT((int)current_button);
    }

    DBG_PRINT(" tick=");
    DBG_PRINT_INT((int)tick_get());

    DBG_PRINTLN("");
}

static void app_toggle_button_state(uint8_t button)
{
    DBG_PRINT("toggle: button=");
    DBG_PRINT_INT((int)button);
    DBG_PRINT(" -> ");

    switch (button)
    {
        case ADC_BUTTON_1:
            DBG_PRINT("case ADC_BUTTON_1, RB0 ");
            g_led_1_state = (g_led_1_state == 0u) ? 1u : 0u;
            DBG_PRINT("state=");
            DBG_PRINT_INT((int)g_led_1_state);
            DBG_PRINTLN("");
            break;

        case ADC_BUTTON_2:
            DBG_PRINT("case ADC_BUTTON_2, RB1 ");
            g_led_2_state = (g_led_2_state == 0u) ? 1u : 0u;
            DBG_PRINT("state=");
            DBG_PRINT_INT((int)g_led_2_state);
            DBG_PRINTLN("");
            break;

        case ADC_BUTTON_3:
            DBG_PRINT("case ADC_BUTTON_3, RB2 ");
            g_led_3_state = (g_led_3_state == 0u) ? 1u : 0u;
            DBG_PRINT("state=");
            DBG_PRINT_INT((int)g_led_3_state);
            DBG_PRINTLN("");
            break;

        case ADC_BUTTON_4:
            DBG_PRINT("case ADC_BUTTON_4, RB3 ");
            g_led_4_state = (g_led_4_state == 0u) ? 1u : 0u;
            DBG_PRINT("state=");
            DBG_PRINT_INT((int)g_led_4_state);
            DBG_PRINTLN("");
            break;

        default:
            DBG_PRINT("default, no action");
            DBG_PRINTLN("");
            return;
    }

    app_apply_led_states();
    app_print_led_states();
}

void main(void)
{
    uint8_t button;
    uint16_t raw;

    app_io_init();

    adc_init();
    uart_init(9600u);

    /*
     * tick_init() uses Timer1 internally.
     * Global and peripheral interrupts must be enabled,
     * otherwise tick_get() will not increment and adc_buttons debounce will not work.
     */
    tick_init();

    INTCONbits.PEIE = 1u;
    INTCONbits.GIE = 1u;

    adc_buttons_init(
        &g_buttons,
        ADC_BUTTON_CHANNEL,
        g_adc_button_table,
        (uint8_t)(sizeof(g_adc_button_table) / sizeof(g_adc_button_table[0]))
    );

    DBG_PRINTLN("XC8 ADC buttons library toggle example started");

    while (1)
    {
        raw = adc_read(ADC_BUTTON_CHANNEL);
        app_print_adc_change(raw);

        adc_buttons_update(&g_buttons);
        button = adc_buttons_get(&g_buttons);

        /*
         * Log only real button state changes.
         */
        if (button != g_last_button)
        {
            app_print_button_change(g_last_button, button);

            /*
             * Toggle when a new stable button appears.
             * This prevents repeated toggling while holding the same button.
             */
            if (button != ADC_BUTTON_NONE)
            {
                app_toggle_button_state(button);
            }

            g_last_button = button;
        }

        tick_delay(20u);
    }
}