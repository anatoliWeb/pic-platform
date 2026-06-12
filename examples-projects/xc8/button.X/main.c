#include "project_config.h"
#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"
#include "libraries/input/button/button.h"

#define BUTTON_PIN_PORT     PORTBbits.RB0
#define BUTTON_PIN_TRIS     TRISBbits.TRISB0

#define LED_PIN_LAT         LATBbits.LATB1
#define LED_PIN_TRIS        TRISBbits.TRISB1

#define BUTTON_RELEASED     1u
#define BUTTON_PRESSED      0u

#define BUTTON_DEBOUNCE_MS  30u

static uint8_t g_led_state = 0u;
static uint8_t g_last_raw_state = BUTTON_RELEASED;
static uint8_t g_stable_state = BUTTON_RELEASED;
static uint8_t g_last_reported_state = BUTTON_RELEASED;
static uint8_t g_debounce_counter = 0u;

static void app_io_init(void)
{
    /*
     * RB0 = button input.
     * RB1 = LED output.
     *
     * For Proteus:
     * - connect RB0 to +5V through pull-up resistor
     * - connect button from RB0 to GND
     * - connect LED to RB1
     */
    ADCON1 = 0x0Fu;

    BUTTON_PIN_TRIS = 1u;
    LED_PIN_TRIS = 0u;

    LED_PIN_LAT = 0u;
}

static void app_apply_led_state(void)
{
    LED_PIN_LAT = g_led_state;
}

static void app_toggle_led(void)
{
    g_led_state = (g_led_state == 0u) ? 1u : 0u;
    app_apply_led_state();

    DBG_PRINT("LED toggled, state=");
    DBG_PRINT_INT((int)g_led_state);
    DBG_PRINTLN("");
}

static void app_print_button_state(uint8_t state)
{
    DBG_PRINT("Button state changed: ");

    if (state == BUTTON_PRESSED)
    {
        DBG_PRINTLN("PRESSED");
    }
    else
    {
        DBG_PRINTLN("RELEASED");
    }
}

static void app_button_update(void)
{
    uint8_t raw_state;

    raw_state = BUTTON_PIN_PORT;

    /*
     * Very simple debounce:
     * if raw state changed, wait BUTTON_DEBOUNCE_MS stable samples.
     */
    if (raw_state != g_last_raw_state)
    {
        g_last_raw_state = raw_state;
        g_debounce_counter = 0u;
        return;
    }

    if (g_debounce_counter < BUTTON_DEBOUNCE_MS)
    {
        g_debounce_counter++;
        return;
    }

    if (g_stable_state != raw_state)
    {
        g_stable_state = raw_state;

        if (g_stable_state != g_last_reported_state)
        {
            app_print_button_state(g_stable_state);

            /*
             * Toggle only on new press, not on release.
             */
            if (g_stable_state == BUTTON_PRESSED)
            {
                app_toggle_led();
            }

            g_last_reported_state = g_stable_state;
        }
    }
}

void main(void)
{
    app_io_init();
    uart_init(9600u);

    DBG_PRINTLN("XC8 button example started");
    DBG_PRINTLN("RB0 button input, RB1 LED output");

    while (1)
    {
        app_button_update();
        delay_ms(1u);
    }
}