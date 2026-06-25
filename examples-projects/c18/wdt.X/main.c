/*
 * File: examples-projects/c18/wdt.X/main.c
 */

#include "project_config.h"
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

