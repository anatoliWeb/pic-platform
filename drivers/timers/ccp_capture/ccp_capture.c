#include "drivers/timers/ccp_capture/ccp_capture.h"

#if defined(DRV_COMPILER_C18)
    #include "../../../C18/drivers/timers/ccp_capture/ccp_capture.c"
#elif defined(DRV_COMPILER_XC8)
    #include "../../../XC8/drivers/timers/ccp_capture/ccp_capture.c"
#else

static ccp_capture_callback_t g_capture_cb = 0;

static uint8_t ccp_capture_mode_bits(ccp_capture_mode_t mode)
{
    switch (mode)
    {
        case CCP_CAPTURE_RISING:  return 0x05u;
        case CCP_CAPTURE_FALLING: return 0x04u;
        case CCP_CAPTURE_EVERY_4: return 0x06u;
        case CCP_CAPTURE_EVERY_16:return 0x07u;
        default:                  return 0x05u;
    }
}

void ccp_capture_init(ccp_capture_mode_t mode)
{
    CCP1CON = (uint8_t)((CCP1CON & 0xF0u) | ccp_capture_mode_bits(mode));
    PIR1bits.CCP1IF = 0u;
}

void ccp_capture_enable(void)
{
    PIE1bits.CCP1IE = 1u;
}

void ccp_capture_disable(void)
{
    PIE1bits.CCP1IE = 0u;
}

uint16_t ccp_capture_get_value(void)
{
    return (uint16_t)(((uint16_t)CCPR1H << 8u) | CCPR1L);
}

void ccp_capture_set_callback(ccp_capture_callback_t cb)
{
    g_capture_cb = cb;
}

void ccp_capture_irq_handler(void)
{
    if (PIR1bits.CCP1IF != 0u)
    {
        uint16_t value = ccp_capture_get_value();
        PIR1bits.CCP1IF = 0u;
        if (g_capture_cb != 0)
        {
            g_capture_cb(value);
        }
    }
}

#endif
