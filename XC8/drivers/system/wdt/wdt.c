#include "drivers/system/wdt/wdt.h"

void wdt_clear(void)
{
#if defined(DRV_COMPILER_C18)
    _asm CLRWDT _endasm
#else
    CLRWDT();
#endif
}
