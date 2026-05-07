#include "drivers/interrupts/ext_interrupt/ext_interrupt.h"

static ext_interrupt_callback_t ext_int_callbacks[3] = { 0, 0, 0 };

void ext_interrupt_init(void)
{
    INTCONbits.INT0IF = 0u;
    INTCON3bits.INT1IF = 0u;
    INTCON3bits.INT2IF = 0u;

    INTCONbits.INT0IE = 0u;
    INTCON3bits.INT1IE = 0u;
    INTCON3bits.INT2IE = 0u;

    INTCON2bits.INTEDG0 = 0u;
    INTCON2bits.INTEDG1 = 0u;
    INTCON2bits.INTEDG2 = 0u;
}

void ext_interrupt_enable(ext_interrupt_channel_t channel)
{
    if (channel == EXT_INT0) { INTCONbits.INT0IE = 1u; }
    else if (channel == EXT_INT1) { INTCON3bits.INT1IE = 1u; }
    else { INTCON3bits.INT2IE = 1u; }
}

void ext_interrupt_disable(ext_interrupt_channel_t channel)
{
    if (channel == EXT_INT0) { INTCONbits.INT0IE = 0u; }
    else if (channel == EXT_INT1) { INTCON3bits.INT1IE = 0u; }
    else { INTCON3bits.INT2IE = 0u; }
}

void ext_interrupt_set_edge(ext_interrupt_channel_t channel, ext_interrupt_edge_t edge)
{
    uint8_t rising = (edge == EXT_INT_EDGE_RISING) ? 1u : 0u;

    if (channel == EXT_INT0) { INTCON2bits.INTEDG0 = rising; }
    else if (channel == EXT_INT1) { INTCON2bits.INTEDG1 = rising; }
    else { INTCON2bits.INTEDG2 = rising; }
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
    if (channel == EXT_INT0) { INTCONbits.INT0IF = 0u; }
    else if (channel == EXT_INT1) { INTCON3bits.INT1IF = 0u; }
    else { INTCON3bits.INT2IF = 0u; }
}

uint8_t ext_interrupt_get_flag(ext_interrupt_channel_t channel)
{
    if (channel == EXT_INT0) { return INTCONbits.INT0IF; }
    else if (channel == EXT_INT1) { return INTCON3bits.INT1IF; }
    else { return INTCON3bits.INT2IF; }
}

void ext_interrupt_irq_handler(void)
{
    if (INTCONbits.INT0IF != 0u)
    {
        INTCONbits.INT0IF = 0u;
        if (ext_int_callbacks[EXT_INT0] != 0) { ext_int_callbacks[EXT_INT0](); }
    }

    if (INTCON3bits.INT1IF != 0u)
    {
        INTCON3bits.INT1IF = 0u;
        if (ext_int_callbacks[EXT_INT1] != 0) { ext_int_callbacks[EXT_INT1](); }
    }

    if (INTCON3bits.INT2IF != 0u)
    {
        INTCON3bits.INT2IF = 0u;
        if (ext_int_callbacks[EXT_INT2] != 0) { ext_int_callbacks[EXT_INT2](); }
    }
}
