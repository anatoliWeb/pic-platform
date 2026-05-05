#include "core/delay.h"

#if defined(DRV_COMPILER_C18)
    #define DRV_NOP() do { _asm nop _endasm } while (0)
#else
    #define DRV_NOP() do { NOP(); } while (0)
#endif

void delay_us(uint16_t us)
{
#if defined(DRV_COMPILER_XC8)
    while (us > 0u)
    {
        __delay_us(1u);
        us--;
    }
#elif defined(DRV_COMPILER_C18)
    volatile uint16_t ticks_per_us;

    ticks_per_us = (uint16_t)(DRV_XTAL_FREQ / 4000000UL);
    if (ticks_per_us == 0u)
    {
        ticks_per_us = 1u;
    }

    while (us > 0u)
    {
        volatile uint16_t i = ticks_per_us;
        while (i > 0u)
        {
            DRV_NOP();
            i--;
        }
        us--;
    }
#else
    DRV_UNUSED(us);
#endif
}

void delay_ms(uint16_t ms)
{
#if defined(DRV_COMPILER_XC8)
    while (ms > 0u)
    {
        __delay_ms(1u);
        ms--;
    }
#else
    while (ms > 0u)
    {
        delay_us(1000u);
        ms--;
    }
#endif
}
