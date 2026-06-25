/*
 * File: XC8/examples/wdt_example.c
 */

#include "core/compiler.h"
#include "drivers/system/wdt/wdt.h"

void main(void)
{
    while (1)
    {
        /* Main loop work */
        wdt_clear();
    }
}

