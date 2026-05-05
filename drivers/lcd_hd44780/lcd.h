#ifndef DRIVERS_LCD_HD44780_LCD_H
#define DRIVERS_LCD_HD44780_LCD_H

#include "core/compiler.h"
#include "core/types.h"

void lcd_init_pins(
    volatile uint8_t* data_port,
    volatile uint8_t* data_tris,
    volatile uint8_t* rs_port,
    volatile uint8_t* rs_tris,
    uint8_t rs_pin,
    volatile uint8_t* en_port,
    volatile uint8_t* en_tris,
    uint8_t en_pin,
    uint8_t d4_pin,
    uint8_t d5_pin,
    uint8_t d6_pin,
    uint8_t d7_pin
);

void lcd_init(void);
void lcd_clear(void);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_write_char(char c);
void lcd_write_string(const char* str);

#endif /* DRIVERS_LCD_HD44780_LCD_H */
