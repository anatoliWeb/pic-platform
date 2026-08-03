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
 */

#ifndef LIBRARIES_DISPLAY_LCD_HD44780_LCD_I2C_H
#define LIBRARIES_DISPLAY_LCD_HD44780_LCD_I2C_H

#include "core/compiler.h"
#include "core/types.h"

/*
 * Compile-time PCF8574 pin mapping. Override any of these before including
 * this header (for example through a compiler -D define).
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

/*
 * Full init: configures the shared I2C master, runs the HD44780 init sequence
 * and enables the backlight. The LCD is ready on success.
 *
 * Attach: binds the LCD to an I2C bus that is already initialized elsewhere.
 * It only probes the address; it never calls i2c_init(), never runs the
 * HD44780 init sequence and never clears the display. The bind is committed
 * only after a successful probe. Run lcd_i2c_controller_init() afterwards to
 * make the display ready.
 *
 * Controller init: runs only the HD44780 init sequence on an already
 * initialized I2C bus, using the address bound by init() or attach().
 * It returns LCD_I2C_NOT_INITIALIZED if no address is bound.
 *
 * Probe: diagnostic only. It checks address presence without changing the
 * currently bound address or ready flag.
 */
lcd_i2c_status_t lcd_i2c_init(uint8_t i2c_addr, uint32_t i2c_clock_hz);
lcd_i2c_status_t lcd_i2c_attach(uint8_t i2c_addr);
lcd_i2c_status_t lcd_i2c_controller_init(void);

lcd_i2c_status_t lcd_i2c_probe(uint8_t i2c_addr);
lcd_i2c_status_t lcd_i2c_last_status(void);
uint8_t lcd_i2c_is_ready(void);

void lcd_i2c_clear(void);
void lcd_i2c_home(void);
void lcd_i2c_set_cursor(uint8_t row, uint8_t col);
void lcd_i2c_write_char(char c);
void lcd_i2c_write_string(const char* str);
void lcd_i2c_backlight(uint8_t on);

#endif /* LIBRARIES_DISPLAY_LCD_HD44780_LCD_I2C_H */
