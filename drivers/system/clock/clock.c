/*
 * File: drivers/system/clock/clock.c
 */

#include "drivers/system/clock/clock.h"

/*
 * Backward-compatible function.
 */
uint32_t clock_get_frequency(void)
{
    return clock_get_frequency_hz();
}

uint32_t clock_get_frequency_hz(void)
{
    return (uint32_t)CLOCK_FREQUENCY;
}

uint32_t clock_get_frequency_khz(void)
{
    return (uint32_t)(CLOCK_FREQUENCY / 1000UL);
}

uint32_t clock_get_frequency_mhz(void)
{
    return (uint32_t)(CLOCK_FREQUENCY / 1000000UL);
}

uint32_t clock_get_instruction_frequency_hz(void)
{
    return (uint32_t)CLOCK_INSTRUCTION_FREQUENCY;
}

uint32_t clock_get_instruction_frequency_khz(void)
{
    return (uint32_t)(CLOCK_INSTRUCTION_FREQUENCY / 1000UL);
}

uint32_t clock_get_instruction_frequency_mhz(void)
{
    return (uint32_t)(CLOCK_INSTRUCTION_FREQUENCY / 1000000UL);
}