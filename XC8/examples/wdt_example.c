#include "core/compiler.h"
#include "drivers/wdt/wdt.h"

void main(void)
{
    while (1)
    {
        /* Main loop work */
        wdt_clear();
    }
}

