/*
 * File: examples-projects/xc8/debug_display_i2c.X/main.c
 */

#include "project_config.h"

#include "core/delay.h"
#include "core/debug.h"

void main(void)
{
    uint16_t counter = 0u;

    debug_init();
    debug_clear();
    debug_write_line("SYSTEM OK");

    while (1)
    {
        debug_clear();
        debug_write("COUNT=");
        debug_write_u16(counter);
        debug_write_line("");

        counter++;
        DRV_DELAY_MS(1000u);
    }
}
