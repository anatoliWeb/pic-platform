#include "drivers/interrupts/portb_change/portb_change.h"

/*
 * PORTB interrupt-on-change is available on RB4-RB7.
 *
 * The hardware only tells us that something changed.
 * The software compares the current PORTB state with the previous state
 * to determine which pin changed.
 */
static volatile uint8_t g_last_state = 0u;
static volatile uint8_t g_changed_mask = 0u;
static portb_change_callback_t g_callback = 0;

void portb_change_init(void)
{
    uint8_t dummy;

    /*
     * Configure RB4-RB7 as inputs.
     *
     * RB4-RB7 are the only pins used by PORTB interrupt-on-change
     * on PIC18F452.
     */
    TRISB |= 0xF0u;

    /*
     * Read PORTB first to end the mismatch condition.
     * This is important before clearing RBIF.
     */
    dummy = PORTB;
    DRV_UNUSED(dummy);

    /*
     * Store initial state.
     * Only RB4-RB7 are relevant, but keeping the full PORTB value
     * makes debugging easier.
     */
    g_last_state = PORTB;
    g_changed_mask = 0u;

    /*
     * Clear pending PORTB change interrupt flag.
     */
    INTCONbits.RBIF = 0u;
}

void portb_change_enable(void)
{
    uint8_t dummy;

    /*
     * Read PORTB before clearing RBIF.
     * Required to clear the mismatch condition.
     */
    dummy = PORTB;
    DRV_UNUSED(dummy);

    /*
     * Clear stale flag before enabling interrupt.
     */
    INTCONbits.RBIF = 0u;

    /*
     * Enable PORTB change interrupt.
     */
    INTCONbits.RBIE = 1u;

    /*
     * Enable global interrupts.
     * Without GIE, RBIF can be set, but ISR will not run.
     */
    INTCONbits.GIE = 1u;
}

void portb_change_disable(void)
{
    /*
     * Disable PORTB change interrupt.
     */
    INTCONbits.RBIE = 0u;
}

uint8_t portb_change_read_port(void)
{
    return PORTB;
}

uint8_t portb_change_get_last_state(void)
{
    return g_last_state;
}

uint8_t portb_change_get_changed_mask(void)
{
    return g_changed_mask;
}

void portb_change_set_callback(portb_change_callback_t cb)
{
    g_callback = cb;
}

void portb_change_irq_handler(void)
{
    uint8_t current;
    uint8_t changed;

    /*
     * Handle only PORTB change interrupt.
     */
    if (INTCONbits.RBIF == 0u)
    {
        return;
    }

    /*
     * Read PORTB to end mismatch condition.
     */
    current = PORTB;

    /*
     * Detect changes only on RB4-RB7.
     */
    changed = (uint8_t)((current ^ g_last_state) & 0xF0u);

    /*
     * Save state for next comparison.
     */
    g_last_state = current;
    g_changed_mask = changed;

    /*
     * Clear interrupt flag after reading PORTB.
     */
    INTCONbits.RBIF = 0u;

    /*
     * Call user callback only when one of RB4-RB7 actually changed.
     */
    if ((changed != 0u) && (g_callback != 0))
    {
        g_callback(changed, current);
    }
}