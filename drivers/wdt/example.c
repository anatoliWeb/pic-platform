#include "core/compiler.h"
#include "drivers/wdt/wdt.h"

void main(void)
{
    while (1)
    {
        /* Application logic */

        wdt_clear();
    }
}
