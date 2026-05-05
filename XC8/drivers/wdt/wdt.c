#include "drivers/wdt/wdt.h"

void wdt_clear(void)
{
#if defined(__18CXX)
    _asm CLRWDT _endasm
#else
    CLRWDT();
#endif
}
