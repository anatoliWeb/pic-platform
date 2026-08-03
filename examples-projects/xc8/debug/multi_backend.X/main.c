/*
 * File: examples-projects/xc8/debug/debug_multi_backend.X/main.c
 */

#include "project_config.h"

#include "core/delay.h"
#include "core/debug.h"

void main(void)
{
    uint16_t tick = 0u;

    debug_init();
    debug_write_line("BOOT");
    debug_pin_code(0x03u);

    while (1)
    {
        debug_info("MULTI");
        debug_write("tick=");
        debug_write_u16(tick);
        debug_write_line("");
        debug_pin_pulse(0u);

        tick++;
        DRV_DELAY_MS(1000u);
    }
}
