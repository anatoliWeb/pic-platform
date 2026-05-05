#include "drivers/wdt/wdt.h"

#if defined(DRV_COMPILER_C18)
    #include "../../C18/drivers/wdt/wdt.c"
#elif defined(DRV_COMPILER_XC8)
    #include "../../XC8/drivers/wdt/wdt.c"
#else

void wdt_clear(void)
{
#if defined(__18CXX)
    _asm CLRWDT _endasm
#else
    CLRWDT();
#endif
}

#endif
