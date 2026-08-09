/*
 * File: libraries/display/lcd_hd44780/lcd_i2c.h
 *
 * HD44780 character LCD behind a PCF8574 I2C backpack.
 *
 * The backpack pin mapping is configurable at compile time through the
 * LCD_I2C_PIN_* and LCD_I2C_DATA_SHIFT defines, so backpacks with different
 * wiring can be used without editing the library. Defaults match the common
 * PCF8574 backpack: P0=RS, P1=RW, P2=EN, P3=Backlight, P4..P7=D4..D7.
 *
 * The address is a 7-bit I2C address (0x27 for the common backpack). It is
 * used as-is; no masking is applied. Address 0x00 and addresses above 0x7F
 * are rejected as invalid.
 *
 * Compile-time profiles:
 *
 *   FULL (default) - all APIs available.
 *   MINIMAL        - pass -DLCD_I2C_MINIMAL=1 project-wide (compiler macro,
 *                    identical in every translation unit). Removes runtime
 *                    backlight control, lcd_i2c_home(), lcd_i2c_write_string(),
 *                    lcd_i2c_attach(), lcd_i2c_controller_init(), lcd_i2c_probe(),
 *                    lcd_i2c_last_status(), lcd_i2c_is_ready().
 *                    Backlight is always ON (LCD_I2C_PIN_BL included
 *                    in every nibble). All other display operations
 *                    remain fully functional.
 *
 * The library .c is compiled separately, so the profile and the pin mapping
 * defines must be set project-wide through compiler -D or MPLAB define-macros,
 * not through a local #define in one translation unit.
 */

#ifndef LIBRARIES_DISPLAY_LCD_HD44780_LCD_I2C_H
#define LIBRARIES_DISPLAY_LCD_HD44780_LCD_I2C_H

#include "core/compiler.h"
#include "core/types.h"

#ifndef LCD_I2C_MINIMAL
#define LCD_I2C_MINIMAL 0
#endif

/*
 * Compile-time PCF8574 pin mapping. Override any of these project-wide
 * through compiler -D defines or MPLAB define-macros. A local override
 * before including this header is visible to one translation unit only.
 */
#ifndef LCD_I2C_PIN_RS
    #define LCD_I2C_PIN_RS 0x01u
#endif

#ifndef LCD_I2C_PIN_RW
    #define LCD_I2C_PIN_RW 0x02u
#endif

#ifndef LCD_I2C_PIN_EN
    #define LCD_I2C_PIN_EN 0x04u
#endif

#ifndef LCD_I2C_PIN_BL
    #define LCD_I2C_PIN_BL 0x08u
#endif

#ifndef LCD_I2C_DATA_SHIFT
    #define LCD_I2C_DATA_SHIFT 4u
#endif

typedef enum lcd_i2c_status
{
    LCD_I2C_OK = 0,
    LCD_I2C_INVALID_ARGUMENT,
    LCD_I2C_NOT_INITIALIZED,
    LCD_I2C_NO_ACK
} lcd_i2c_status_t;

lcd_i2c_status_t lcd_i2c_init(uint8_t i2c_addr, uint32_t i2c_clock_hz);
#if !LCD_I2C_MINIMAL
lcd_i2c_status_t lcd_i2c_attach(uint8_t i2c_addr);
lcd_i2c_status_t lcd_i2c_controller_init(void);
lcd_i2c_status_t lcd_i2c_probe(uint8_t i2c_addr);
lcd_i2c_status_t lcd_i2c_last_status(void);
uint8_t lcd_i2c_is_ready(void);
#endif

void lcd_i2c_clear(void);
#if LCD_I2C_MINIMAL
#define lcd_i2c_home() lcd_i2c_set_cursor(0u, 0u)
#else
void lcd_i2c_home(void);
#endif
void lcd_i2c_set_cursor(uint8_t row, uint8_t col);
void lcd_i2c_write_char(char c);
#if !LCD_I2C_MINIMAL
void lcd_i2c_write_string(const char* str);
void lcd_i2c_backlight(uint8_t on);
#endif

#endif /* LIBRARIES_DISPLAY_LCD_HD44780_LCD_I2C_H */
