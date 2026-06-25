/*
 * File: C18/drivers/timers/timer1/timer1.c
 */

#include "drivers/timers/timer1/timer1.h"

static void (*timer1_cb)(void) = (void(*)(void))0;

static uint8_t timer1_ps_bits(uint16_t prescaler)
{
    switch (prescaler)
    {
        case 1u: return 0u;
        case 2u: return 1u;
        case 4u: return 2u;
        case 8u: return 3u;
        default: return 3u;
    }
}

void timer1_init(uint16_t prescaler)
{
    T1CONbits.TMR1CS = 0u;
    T1CONbits.T1CKPS0 = (uint8_t)(timer1_ps_bits(prescaler) & 0x01u);
    T1CONbits.T1CKPS1 = (uint8_t)((timer1_ps_bits(prescaler) >> 1u) & 0x01u);
    TMR1H = 0u;
    TMR1L = 0u;
}

void timer1_start(void) { T1CONbits.TMR1ON = 1u; }
void timer1_stop(void) { T1CONbits.TMR1ON = 0u; }

void timer1_set(uint16_t value)
{
    TMR1H = (uint8_t)(value >> 8u);
    TMR1L = (uint8_t)(value & 0xFFu);
}

uint16_t timer1_get(void)
{
    return (uint16_t)(((uint16_t)TMR1H << 8u) | TMR1L);
}

void timer1_enable_interrupt(void)
{
    PIR1bits.TMR1IF = 0u;
    PIE1bits.TMR1IE = 1u;
}

void timer1_disable_interrupt(void) { PIE1bits.TMR1IE = 0u; }
void timer1_set_callback(void (*cb)(void)) { timer1_cb = cb; }
void (*timer1_get_callback(void))(void) { return timer1_cb; }

void timer1_irq_handler(void)
{
    if ((PIE1bits.TMR1IE != 0u) && (PIR1bits.TMR1IF != 0u))
    {
        PIR1bits.TMR1IF = 0u;
        if (timer1_cb != (void(*)(void))0) { timer1_cb(); }
    }
}
