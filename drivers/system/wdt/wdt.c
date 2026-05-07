#include "drivers/system/wdt/wdt.h"

#if defined(DRV_COMPILER_C18)
    #include "../../../C18/drivers/system/wdt/wdt.c"
#elif defined(DRV_COMPILER_XC8)
    #include "../../../XC8/drivers/system/wdt/wdt.c"
#else

void wdt_clear(void)
{
#if defined(DRV_COMPILER_C18)
    _asm CLRWDT _endasm
#else
    CLRWDT();
#endif
}

#endif
