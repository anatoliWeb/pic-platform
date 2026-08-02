/*
 * File: examples-projects/xc8/debug_display_parallel.X/main.c
 */

#include "project_config.h"

#include "core/delay.h"
#include "core/debug.h"
#include "libraries/display/lcd_hd44780/lcd.h"

static void lcd_prepare(void)
{
    lcd_init_pins(
        &PORTD, &TRISD,
        &PORTB, &TRISB, 0u,
        &PORTB, &TRISB, 1u,
        4u, 5u, 6u, 7u
    );
}

void main(void)
{
    uint16_t tick = 0u;

    lcd_prepare();
    debug_init();
    debug_clear();
    debug_write_line("BOOT");
    debug_write_line("LCD 2x16 PAR");

    while (1)
    {
        debug_clear();
        debug_write_line("Parallel debug");
        debug_write("tick=");
        debug_write_u16(tick);
        debug_write_line("");

        tick++;
        DRV_DELAY_MS(1000u);
    }
}
