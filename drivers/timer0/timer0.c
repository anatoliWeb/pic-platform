#include "drivers/timer0/timer0.h"

#if defined(DRV_COMPILER_C18)
    #include "../../C18/drivers/timer0/timer0.c"
#elif defined(DRV_COMPILER_XC8)
    #include "../../XC8/drivers/timer0/timer0.c"
#else

static void (*timer0_cb)(void) = (void(*)(void))0;

static uint8_t timer0_ps_bits(uint16_t prescaler)
{
    switch (prescaler)
    {
        case 2u: return 0u;
        case 4u: return 1u;
        case 8u: return 2u;
        case 16u: return 3u;
        case 32u: return 4u;
        case 64u: return 5u;
        case 128u: return 6u;
        case 256u: return 7u;
        default: return 7u;
    }
}

void timer0_init(uint16_t prescaler)
{
    T0CONbits.T08BIT = 0u;
    T0CONbits.T0CS = 0u;
    T0CONbits.PSA = 0u;
    T0CONbits.T0PS = timer0_ps_bits(prescaler);
    TMR0H = 0u;
    TMR0L = 0u;
}

void timer0_start(void) { T0CONbits.TMR0ON = 1u; }
void timer0_stop(void) { T0CONbits.TMR0ON = 0u; }

void timer0_set(uint16_t value)
{
    TMR0H = (uint8_t)(value >> 8u);
    TMR0L = (uint8_t)(value & 0xFFu);
}

uint16_t timer0_get(void)
{
    return (uint16_t)(((uint16_t)TMR0H << 8u) | TMR0L);
}

void timer0_enable_interrupt(void)
{
    INTCONbits.TMR0IF = 0u;
    INTCONbits.TMR0IE = 1u;
}

void timer0_disable_interrupt(void) { INTCONbits.TMR0IE = 0u; }
void timer0_set_callback(void (*cb)(void)) { timer0_cb = cb; }

void timer0_irq_handler(void)
{
    if ((INTCONbits.TMR0IE != 0u) && (INTCONbits.TMR0IF != 0u))
    {
        INTCONbits.TMR0IF = 0u;
        if (timer0_cb != (void(*)(void))0) { timer0_cb(); }
    }
}

#endif
