#include "drivers/system/clock/clock.h"

#if defined(DRV_COMPILER_C18)
    #include "../../../C18/drivers/system/clock/clock.c"
#elif defined(DRV_COMPILER_XC8)
    #include "../../../XC8/drivers/system/clock/clock.c"
#else

uint32_t clock_get_frequency(void)
{
    return DRV_XTAL_FREQ;
}

#endif
