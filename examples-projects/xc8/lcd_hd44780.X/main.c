#include "project_config.h"

#include "core/compiler.h"
#include "core/delay.h"

#include "libraries/display/lcd_hd44780/lcd.h"

#define LCD_COLS 16u

static void lcd_write_fixed_16(const char* text)
{
    uint8_t i = 0u;

    /*
     * Write up to 16 characters and fill the rest with spaces.
     * This keeps old text from staying on the LCD.
     */
    while ((text != (const char*)0) && (text[i] != '\0') && (i < LCD_COLS))
    {
        lcd_write_char(text[i]);
        i++;
    }

    while (i < LCD_COLS)
    {
        lcd_write_char(' ');
        i++;
    }
}

static void lcd_show_boot_screen(void)
{
    lcd_clear();

    lcd_set_cursor(0u, 0u);
    lcd_write_fixed_16("PIC18F452 LCD");

    lcd_set_cursor(1u, 0u);
    lcd_write_fixed_16("HD44780 4-bit");

    DRV_DELAY_MS(1500);
}

static void lcd_show_pin_map(void)
{
    lcd_clear();

    lcd_set_cursor(0u, 0u);
    lcd_write_fixed_16("RS RB0 EN RB1");

    lcd_set_cursor(1u, 0u);
    lcd_write_fixed_16("D4-D7 RD4-RD7");

    DRV_DELAY_MS(2000);
}

static void lcd_show_position_test(void)
{
    lcd_clear();

    /*
     * This test checks both rows and cursor positioning.
     */
    lcd_set_cursor(0u, 0u);
    lcd_write_char('0');

    lcd_set_cursor(0u, 15u);
    lcd_write_char('1');

    lcd_set_cursor(1u, 0u);
    lcd_write_char('2');

    lcd_set_cursor(1u, 15u);
    lcd_write_char('3');

    DRV_DELAY_MS(1500);

    lcd_set_cursor(0u, 1u);
    lcd_write_string("Row0");

    lcd_set_cursor(1u, 1u);
    lcd_write_string("Row1");

    DRV_DELAY_MS(1500);
}

static void lcd_show_char_fill_test(void)
{
    uint8_t i;

    /*
     * Fill the display with changing characters.
     * This helps detect wrong D4-D7 wiring.
     */
    lcd_clear();

    lcd_set_cursor(0u, 0u);
    for (i = 0u; i < LCD_COLS; i++)
    {
        lcd_write_char('#');
    }

    lcd_set_cursor(1u, 0u);
    for (i = 0u; i < LCD_COLS; i++)
    {
        lcd_write_char('*');
    }

    DRV_DELAY_MS(1200);

    lcd_clear();

    lcd_set_cursor(0u, 0u);
    for (i = 0u; i < LCD_COLS; i++)
    {
        lcd_write_char((char)('0' + (i % 10u)));
    }

    lcd_set_cursor(1u, 0u);
    for (i = 0u; i < LCD_COLS; i++)
    {
        lcd_write_char((char)('A' + i));
    }

    DRV_DELAY_MS(1800);
}

static void lcd_show_counter_test(void)
{
    uint8_t count;

    /*
     * Simple dynamic update test.
     */
    for (count = 0u; count < 10u; count++)
    {
        lcd_clear();

        lcd_set_cursor(0u, 0u);
        lcd_write_fixed_16("Counter test");

        lcd_set_cursor(1u, 0u);
        lcd_write_string("Count: ");
        lcd_write_char((char)('0' + count));

        DRV_DELAY_MS(500);
    }
}

static void lcd_show_progress_test(void)
{
    uint8_t i;

    /*
     * Progress bar style test.
     */
    lcd_clear();

    lcd_set_cursor(0u, 0u);
    lcd_write_fixed_16("Progress test");

    lcd_set_cursor(1u, 0u);
    lcd_write_fixed_16("[              ]");

    for (i = 1u; i <= 14u; i++)
    {
        lcd_set_cursor(1u, i);
        lcd_write_char('=');
        DRV_DELAY_MS(150);
    }

    DRV_DELAY_MS(800);
}

static void lcd_show_scroll_text_test(void)
{
    uint8_t offset;
    uint8_t i;

    const char message[] = "PIC18F452 HD44780 LCD 4-bit test   ";

    /*
     * Manual scrolling text test.
     * No LCD shift command is needed, so this works with the current API.
     */
    for (offset = 0u; offset < 22u; offset++)
    {
        lcd_clear();

        lcd_set_cursor(0u, 0u);
        lcd_write_fixed_16("Scroll message");

        lcd_set_cursor(1u, 0u);

        for (i = 0u; i < LCD_COLS; i++)
        {
            lcd_write_char(message[offset + i]);
        }

        DRV_DELAY_MS(250);
    }
}

static void lcd_show_alive_screen(uint8_t phase)
{
    lcd_clear();

    lcd_set_cursor(0u, 0u);
    lcd_write_fixed_16("LCD TEST LOOP");

    lcd_set_cursor(1u, 0u);

    if (phase == 0u)
    {
        lcd_write_fixed_16("Status: OK");
    }
    else if (phase == 1u)
    {
        lcd_write_fixed_16("Display alive");
    }
    else
    {
        lcd_write_fixed_16("PIC18F452 ready");
    }

    DRV_DELAY_MS(1000);
}

void main(void)
{
    uint8_t phase = 0u;

    /*
     * PIC18F452 + HD44780 LCD 16x2 test.
     *
     * LCD mode:
     *   4-bit parallel mode
     *
     * LCD data pins:
     *   D4 -> RD4 / pin 27
     *   D5 -> RD5 / pin 28
     *   D6 -> RD6 / pin 29
     *   D7 -> RD7 / pin 30
     *
     * LCD control pins:
     *   RS -> RB0 / pin 33
     *   EN -> RB1 / pin 34
     *
     * LCD fixed pins:
     *   VSS -> GND
     *   VDD -> +5V
     *   V0  -> contrast potentiometer center pin
     *   RW  -> GND
     *   A   -> +5V through resistor, if backlight is used
     *   K   -> GND
     */

    lcd_init_pins(
        &PORTD, &TRISD,
        &PORTB, &TRISB, 0u,
        &PORTB, &TRISB, 1u,
        4u, 5u, 6u, 7u
    );

    lcd_init();

    while (1)
    {
        lcd_show_boot_screen();
        lcd_show_pin_map();
        lcd_show_position_test();
        lcd_show_char_fill_test();
        lcd_show_counter_test();
        lcd_show_progress_test();
        lcd_show_scroll_text_test();

        lcd_show_alive_screen(phase);

        phase++;
        if (phase >= 3u)
        {
            phase = 0u;
        }
    }
}