#include "drivers/interrupts/ext_interrupt/ext_interrupt.h"

/*
 * Callback table for INT0, INT1, INT2.
 *
 * Index mapping:
 *   EXT_INT0 -> 0
 *   EXT_INT1 -> 1
 *   EXT_INT2 -> 2
 */
static ext_interrupt_callback_t ext_int_callbacks[3] = { 0, 0, 0 };

void ext_interrupt_init(void)
{
    /*
     * Clear pending interrupt flags before configuration.
     * This prevents an old pending edge from firing immediately
     * after the interrupt is enabled.
     */
    INTCONbits.INT0IF = 0u;
    INTCON3bits.INT1IF = 0u;
    INTCON3bits.INT2IF = 0u;

    /*
     * Disable external interrupts during initialization.
     */
    INTCONbits.INT0IE = 0u;
    INTCON3bits.INT1IE = 0u;
    INTCON3bits.INT2IE = 0u;

    /*
     * Default edge: falling edge.
     *
     * For a button connected as:
     *   +5V -> 10k pull-up -> RBx/INTx -> button -> GND
     *
     * the pin goes from HIGH to LOW when pressed,
     * so falling edge is the correct default.
     */
    INTCON2bits.INTEDG0 = 0u;
    INTCON2bits.INTEDG1 = 0u;
    INTCON2bits.INTEDG2 = 0u;

    /*
     * Keep callbacks cleared by default.
     */
    ext_int_callbacks[EXT_INT0] = 0;
    ext_int_callbacks[EXT_INT1] = 0;
    ext_int_callbacks[EXT_INT2] = 0;
}

void ext_interrupt_enable(ext_interrupt_channel_t channel)
{
    /*
     * Clear the selected interrupt flag before enabling it.
     * This avoids entering the ISR because of a stale flag.
     */
    if (channel == EXT_INT0)
    {
        INTCONbits.INT0IF = 0u;
        INTCONbits.INT0IE = 1u;
    }
    else if (channel == EXT_INT1)
    {
        INTCON3bits.INT1IF = 0u;
        INTCON3bits.INT1IE = 1u;
    }
    else if (channel == EXT_INT2)
    {
        INTCON3bits.INT2IF = 0u;
        INTCON3bits.INT2IE = 1u;
    }
    else
    {
        return;
    }

    /*
     * Enable global interrupts.
     *
     * INT0/INT1/INT2 will not enter the ISR while GIE is cleared,
     * even if their local enable bits are set.
     */
    INTCONbits.GIE = 1u;
}

void ext_interrupt_disable(ext_interrupt_channel_t channel)
{
    if (channel == EXT_INT0)
    {
        INTCONbits.INT0IE = 0u;
    }
    else if (channel == EXT_INT1)
    {
        INTCON3bits.INT1IE = 0u;
    }
    else if (channel == EXT_INT2)
    {
        INTCON3bits.INT2IE = 0u;
    }
    else
    {
        /* Invalid channel, nothing to disable. */
    }
}

void ext_interrupt_set_edge(ext_interrupt_channel_t channel, ext_interrupt_edge_t edge)
{
    uint8_t rising;

    /*
     * INTEDGx = 1: rising edge
     * INTEDGx = 0: falling edge
     */
    rising = (edge == EXT_INT_EDGE_RISING) ? 1u : 0u;

    if (channel == EXT_INT0)
    {
        INTCON2bits.INTEDG0 = rising;
    }
    else if (channel == EXT_INT1)
    {
        INTCON2bits.INTEDG1 = rising;
    }
    else if (channel == EXT_INT2)
    {
        INTCON2bits.INTEDG2 = rising;
    }
    else
    {
        /* Invalid channel, nothing to configure. */
    }
}

void ext_interrupt_set_callback(ext_interrupt_channel_t channel, ext_interrupt_callback_t callback)
{
    if (channel <= EXT_INT2)
    {
        ext_int_callbacks[(uint8_t)channel] = callback;
    }
}

void ext_interrupt_clear_flag(ext_interrupt_channel_t channel)
{
    if (channel == EXT_INT0)
    {
        INTCONbits.INT0IF = 0u;
    }
    else if (channel == EXT_INT1)
    {
        INTCON3bits.INT1IF = 0u;
    }
    else if (channel == EXT_INT2)
    {
        INTCON3bits.INT2IF = 0u;
    }
    else
    {
        /* Invalid channel, nothing to clear. */
    }
}

uint8_t ext_interrupt_get_flag(ext_interrupt_channel_t channel)
{
    if (channel == EXT_INT0)
    {
        return (uint8_t)INTCONbits.INT0IF;
    }
    else if (channel == EXT_INT1)
    {
        return (uint8_t)INTCON3bits.INT1IF;
    }
    else if (channel == EXT_INT2)
    {
        return (uint8_t)INTCON3bits.INT2IF;
    }
    else
    {
        return 0u;
    }
}

void ext_interrupt_irq_handler(void)
{
    /*
     * INT0 handler.
     */
    if ((INTCONbits.INT0IF != 0u) && (INTCONbits.INT0IE != 0u))
    {
        INTCONbits.INT0IF = 0u;

        if (ext_int_callbacks[EXT_INT0] != 0)
        {
            ext_int_callbacks[EXT_INT0]();
        }
    }

    /*
     * INT1 handler.
     */
    if ((INTCON3bits.INT1IF != 0u) && (INTCON3bits.INT1IE != 0u))
    {
        INTCON3bits.INT1IF = 0u;

        if (ext_int_callbacks[EXT_INT1] != 0)
        {
            ext_int_callbacks[EXT_INT1]();
        }
    }

    /*
     * INT2 handler.
     */
    if ((INTCON3bits.INT2IF != 0u) && (INTCON3bits.INT2IE != 0u))
    {
        INTCON3bits.INT2IF = 0u;

        if (ext_int_callbacks[EXT_INT2] != 0)
        {
            ext_int_callbacks[EXT_INT2]();
        }
    }
}