/*
 * File: examples-projects/xc8/debug_disabled.X/main.c
 */

#include "project_config.h"

#include "core/delay.h"
#include "core/debug.h"

void main(void)
{
    debug_init();
    debug_write_line("BOOT");

    while (1)
    {
        debug_write_line("disabled");
        DRV_DELAY_MS(1000u);
    }
}
