#include "drivers/timers/ccp_compare/ccp_compare.h"

static ccp_compare_callback_t g_compare_cb = 0;

void ccp_compare_init(void)
{
    CCP1CON = (uint8_t)((CCP1CON & 0xF0u) | 0x0Au);
    PIR1bits.CCP1IF = 0u;
}

void ccp_compare_enable(void)
{
    PIE1bits.CCP1IE = 1u;
}

void ccp_compare_disable(void)
{
    PIE1bits.CCP1IE = 0u;
}

void ccp_compare_set_value(uint16_t value)
{
    CCPR1H = (uint8_t)(value >> 8u);
    CCPR1L = (uint8_t)(value & 0xFFu);
}

void ccp_compare_set_callback(ccp_compare_callback_t cb)
{
    g_compare_cb = cb;
}

void ccp_compare_irq_handler(void)
{
    if (PIR1bits.CCP1IF != 0u)
    {
        PIR1bits.CCP1IF = 0u;
        if (g_compare_cb != 0)
        {
            g_compare_cb();
        }
    }
}
