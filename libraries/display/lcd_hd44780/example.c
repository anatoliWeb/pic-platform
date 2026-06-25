/*
 * File: libraries/display/lcd_hd44780/example.c
 */

#include "core/compiler.h"
#include "libraries/display/lcd_hd44780/lcd.h"

void main(void)
{
    lcd_init_pins(
        &PORTD, &TRISD,
        &PORTB, &TRISB, 0u,
        &PORTB, &TRISB, 1u,
        4u, 5u, 6u, 7u
    );

    lcd_init();
    lcd_clear();

    lcd_set_cursor(0u, 0u);
    lcd_write_string("Temp: 23.5C");

    lcd_set_cursor(1u, 0u);
    lcd_write_string("System OK");

    while (1)
    {
    }
}
