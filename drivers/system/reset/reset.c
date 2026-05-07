#include "drivers/system/reset/reset.h"

#if defined(DRV_COMPILER_C18)
    #include "../../../C18/drivers/system/reset/reset.c"
#elif defined(DRV_COMPILER_XC8)
    #include "../../../XC8/drivers/system/reset/reset.c"
#else

static reset_cause_t g_reset_cause = RESET_CAUSE_UNKNOWN;

void reset_init(void)
{
    g_reset_cause = reset_get_cause();
}

reset_cause_t reset_get_cause(void)
{
#if defined(RCONbits)
    if (RCONbits.POR == 0u)
    {
        return RESET_CAUSE_POWER_ON;
    }
    if (RCONbits.BOR == 0u)
    {
        return RESET_CAUSE_BROWN_OUT;
    }
    if (RCONbits.TO == 0u)
    {
        return RESET_CAUSE_WATCHDOG;
    }
    if (RCONbits.RI == 0u)
    {
        return RESET_CAUSE_EXTERNAL;
    }
#endif
    return RESET_CAUSE_UNKNOWN;
}

void reset_clear_flags(void)
{
#if defined(RCONbits)
    RCONbits.POR = 1u;
    RCONbits.BOR = 1u;
    RCONbits.TO = 1u;
    RCONbits.RI = 1u;
#endif
}

#endif
