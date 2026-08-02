/*
 * File: examples-projects/xc8/debug_default_uart.X/main.c
 */

#include "project_config.h"

#include "core/delay.h"
#include "core/debug.h"

void main(void)
{
    uint16_t counter = 0u;

    debug_init();
    debug_write_line("BOOT");

    while (1)
    {
        debug_write("counter=");
        debug_write_u16(counter);
        debug_write_line("");

        counter++;
        DRV_DELAY_MS(1000u);
    }
}
