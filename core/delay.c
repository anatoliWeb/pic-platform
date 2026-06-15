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

    /*
     * PIC18 instruction clock is Fosc / 4.
     * ticks_per_us = Fosc / 4 / 1000000
     */
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

void micro_delay_us(uint16_t us)
{
#if defined(DRV_COMPILER_XC8)
    /*
     * XC8 __delay_us() is based on _XTAL_FREQ.
     * Calling it with 1 us inside a loop allows runtime delay values.
     */
    while (us > 0u)
    {
        __delay_us(1u);
        us--;
    }
#elif defined(DRV_COMPILER_C18)
    /*
     * Approximate fallback for C18.
     * For timing-critical protocols, verify with an oscilloscope.
     */
    while (us > 0u)
    {
        micro_delay_cycles((uint16_t)(DRV_XTAL_FREQ / 4000000UL));
        us--;
    }
#else
    DRV_UNUSED(us);
#endif
}

void micro_delay_cycles(uint16_t cycles)
{
#if defined(DRV_COMPILER_XC8) || defined(DRV_COMPILER_C18)
    /*
     * One loop iteration is not exactly one instruction cycle because
     * the loop itself has overhead. This function is still useful for
     * sub-microsecond tuning steps and should be calibrated by scope.
     */
    while (cycles > 0u)
    {
        DRV_NOP();
        cycles--;
    }
#else
    DRV_UNUSED(cycles);
#endif
}