/*
 * File: drivers/system/wdt/example.c
 */

#include "core/compiler.h"
#include "drivers/system/wdt/wdt.h"

void main(void)
{
    while (1)
    {
        /* Application logic */

        wdt_clear();
    }
}
