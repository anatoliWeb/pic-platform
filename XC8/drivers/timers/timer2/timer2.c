#include "drivers/timers/timer2/timer2.h"

static void (*timer2_cb)(void) = (void(*)(void))0;

static uint8_t timer2_ps_bits(uint16_t prescaler)
{
    switch (prescaler)
    {
        case 1u: return 0u;
        case 4u: return 1u;
        case 16u: return 2u;
        default: return 2u;
    }
}

void timer2_init(uint16_t prescaler)
{
    T2CONbits.T2CKPS0 = (uint8_t)(timer2_ps_bits(prescaler) & 0x01u);
    T2CONbits.T2CKPS1 = (uint8_t)((timer2_ps_bits(prescaler) >> 1u) & 0x01u);
    PR2 = 0xFFu;
    TMR2 = 0u;
}

void timer2_start(void) { T2CONbits.TMR2ON = 1u; }
void timer2_stop(void) { T2CONbits.TMR2ON = 0u; }

void timer2_set(uint16_t value)
{
    TMR2 = (uint8_t)(value & 0xFFu);
}

uint16_t timer2_get(void)
{
    return (uint16_t)TMR2;
}

void timer2_enable_interrupt(void)
{
    PIR1bits.TMR2IF = 0u;
    PIE1bits.TMR2IE = 1u;
}

void timer2_disable_interrupt(void) { PIE1bits.TMR2IE = 0u; }
void timer2_set_callback(void (*cb)(void)) { timer2_cb = cb; }

void timer2_irq_handler(void)
{
    if ((PIE1bits.TMR2IE != 0u) && (PIR1bits.TMR2IF != 0u))
    {
        PIR1bits.TMR2IF = 0u;
        if (timer2_cb != (void(*)(void))0) { timer2_cb(); }
    }
}
