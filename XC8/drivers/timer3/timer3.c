#include "drivers/timer3/timer3.h"

static void (*timer3_cb)(void) = (void(*)(void))0;

static uint8_t timer3_ps_bits(uint16_t prescaler)
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

void timer3_init(uint16_t prescaler)
{
    T3CONbits.TMR3CS = 0u;
    T3CONbits.T3CKPS0 = (uint8_t)(timer3_ps_bits(prescaler) & 0x01u);
    T3CONbits.T3CKPS1 = (uint8_t)((timer3_ps_bits(prescaler) >> 1u) & 0x01u);
    TMR3H = 0u;
    TMR3L = 0u;
}

void timer3_start(void) { T3CONbits.TMR3ON = 1u; }
void timer3_stop(void) { T3CONbits.TMR3ON = 0u; }

void timer3_set(uint16_t value)
{
    TMR3H = (uint8_t)(value >> 8u);
    TMR3L = (uint8_t)(value & 0xFFu);
}

uint16_t timer3_get(void)
{
    return (uint16_t)(((uint16_t)TMR3H << 8u) | TMR3L);
}

void timer3_enable_interrupt(void)
{
    PIR2bits.TMR3IF = 0u;
    PIE2bits.TMR3IE = 1u;
}

void timer3_disable_interrupt(void) { PIE2bits.TMR3IE = 0u; }
void timer3_set_callback(void (*cb)(void)) { timer3_cb = cb; }

void timer3_irq_handler(void)
{
    if ((PIE2bits.TMR3IE != 0u) && (PIR2bits.TMR3IF != 0u))
    {
        PIR2bits.TMR3IF = 0u;
        if (timer3_cb != (void(*)(void))0) { timer3_cb(); }
    }
}
