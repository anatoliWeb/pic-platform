/*
 * File: examples-projects/xc8/debug_default_uart.X/main.c
 */

#include "project_config.h"

#include "core/delay.h"
#include "core/debug.h"

void main(void)
{
    uint16_t tick = 0u;

    debug_init();
    debug_write_line("BOOT");

    while (1)
    {
        debug_write("tick=");
        debug_write_u16(tick);
        debug_write_line("");

        tick++;
        DRV_DELAY_MS(1000u);
    }
}
