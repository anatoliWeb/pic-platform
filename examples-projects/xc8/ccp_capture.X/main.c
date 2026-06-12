#include "project_config.h"
#include "core/compiler.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"
#include "drivers/timers/ccp_capture/ccp_capture.h"

static volatile uint16_t g_last_capture = 0u;
static volatile uint8_t g_capture_ready = 0u;

static uint16_t g_last_printed_capture = 0u;

static void capture_cb(uint16_t value)
{
    g_last_capture = value;
    g_capture_ready = 1u;
}

/*
 * CCP capture interrupt handler.
 * The driver reads CCPR1H:CCPR1L and calls capture_cb().
 */
void __interrupt() isr(void)
{
    ccp_capture_irq_handler();
}

static void app_timer1_init_for_capture(void)
{
    /*
     * CCP Capture uses Timer1 as the time base.
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

static void app_print_startup(void)
{
    DBG_PRINTLN("XC8 CCP capture driver example started");
    DBG_PRINTLN("Input: RC2/CCP1");
    DBG_PRINTLN("Signal: pulse generator 0..5V square wave");
}

static void app_print_capture(uint16_t value)
{
    DBG_PRINT("CCP capture=");
    DBG_PRINT_INT((int)value);

    DBG_PRINT(" TMR1=");
    DBG_PRINT_INT((int)(((uint16_t)TMR1H << 8) | TMR1L));

    DBG_PRINTLN("");
}

void main(void)
{
    /*
     * RC2 / CCP1 input.
     */
    TRISCbits.TRISC2 = 1u;

    uart_init(9600u);
    app_print_startup();

    /*
     * Timer1 must be running before/while CCP capture is enabled.
     */
    app_timer1_init_for_capture();

    ccp_capture_init(CCP_CAPTURE_RISING);
    ccp_capture_set_callback(capture_cb);
    ccp_capture_enable();

    /*
     * Enable peripheral and global interrupts.
     */
    INTCONbits.PEIE = 1u;
    INTCONbits.GIE = 1u;

    while (1)
    {
        if (g_capture_ready != 0u)
        {
            g_capture_ready = 0u;

            if (g_last_capture != g_last_printed_capture)
            {
                g_last_printed_capture = g_last_capture;
                app_print_capture(g_last_capture);
            }
        }
    }
}