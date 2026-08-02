/*
 * File: examples-projects/xc8/debug_display_i2c.X/main.c
 */

#include "project_config.h"

#include "core/delay.h"
#include "core/debug.h"

void main(void)
{
    uint16_t tick = 0u;

    debug_init();
    debug_clear();
    debug_write_line("BOOT");
    debug_write_line("LCD 2x16 I2C");

    while (1)
    {
        debug_clear();
        debug_write_line("I2C debug");
        debug_write("tick=");
        debug_write_u16(tick);
        debug_write_line("");

        tick++;
        DRV_DELAY_MS(1000u);
    }
}
