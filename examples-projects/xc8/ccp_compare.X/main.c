#include "project_config.h"
#include "core/compiler.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"
#include "drivers/timers/ccp_compare/ccp_compare.h"

#define COMPARE_STEP        0x3000u

static volatile uint8_t g_compare_ready = 0u;
static volatile uint16_t g_compare_count = 0u;
static volatile uint16_t g_next_compare = COMPARE_STEP;

static uint8_t g_led_state = 0u;

static void app_apply_led_state(void)
{
    LATBbits.LATB0 = g_led_state;
}

static void app_toggle_led(void)
{
    g_led_state = (g_led_state == 0u) ? 1u : 0u;
    app_apply_led_state();
}

/*
 * CCP Compare callback.
 * Called from ccp_compare_irq_handler().
 *
 * Do only fast work here:
 * - toggle test LED
 * - schedule next compare value
 * - set flag for main loop logging
 */
static void compare_cb(void)
{
    app_toggle_led();

    g_compare_count++;
    g_compare_ready = 1u;

    g_next_compare = (uint16_t)(g_next_compare + COMPARE_STEP);
    ccp_compare_set_value(g_next_compare);
}

/*
 * CCP Compare interrupt hook.
 */
void __interrupt() isr(void)
{
    ccp_compare_irq_handler();
}

static void app_io_init(void)
{
    /*
     * RB0 = LED output.
     */
    TRISBbits.TRISB0 = 0u;
    LATBbits.LATB0 = 0u;

    /*
     * RC2 / CCP1 can be left unconnected for this example,
     * because we verify compare interrupt/callback behavior.
     */
    TRISCbits.TRISC2 = 0u;
}

static void app_timer1_init_for_compare(void)
{
    /*
     * CCP Compare uses Timer1 as the time base.
     *
     * Timer1 settings:
     * - internal clock: Fosc / 4
     * - prescaler: 1:1
     * - Timer1 ON
     */
    T1CONbits.TMR1ON = 0u;

    T1CONbits.TMR1CS = 0u;     /* Internal clock: Fosc / 4 */
    T1CONbits.T1CKPS0 = 0u;    /* Prescaler 1:1 */
    T1CONbits.T1CKPS1 = 0u;

    TMR1H = 0u;
    TMR1L = 0u;

    PIR1bits.TMR1IF = 0u;

    T1CONbits.TMR1ON = 1u;
}

static uint16_t app_timer1_get(void)
{
    uint8_t high;
    uint8_t low;

    high = TMR1H;
    low = TMR1L;

    return ((uint16_t)high << 8) | low;
}

static void app_print_startup(void)
{
    DBG_PRINTLN("XC8 CCP compare driver example started");
    DBG_PRINTLN("Timer1 is used as CCP compare time base");
    DBG_PRINTLN("RB0 toggles on every compare match");
}

static void app_print_compare_event(void)
{
    DBG_PRINT("CCP compare event count=");
    DBG_PRINT_INT((int)g_compare_count);

    DBG_PRINT(" next=");
    DBG_PRINT_INT((int)g_next_compare);

    DBG_PRINT(" TMR1=");
    DBG_PRINT_INT((int)app_timer1_get());

    DBG_PRINT(" RB0=");
    DBG_PRINT_INT((int)g_led_state);

    DBG_PRINTLN("");
}

void main(void)
{
    app_io_init();

    uart_init(9600u);
    app_print_startup();

    /*
     * Timer1 must run before CCP compare is enabled.
     */
    app_timer1_init_for_compare();

    ccp_compare_init();
    ccp_compare_set_value(g_next_compare);
    ccp_compare_set_callback(compare_cb);
    ccp_compare_enable();

    /*
     * Enable peripheral and global interrupts.
     */
    INTCONbits.PEIE = 1u;
    INTCONbits.GIE = 1u;

    while (1)
    {
        if (g_compare_ready != 0u)
        {
            g_compare_ready = 0u;
            app_print_compare_event();
        }
    }
}