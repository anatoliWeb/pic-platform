#include "core/compiler.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/input/keypad_matrix/keypad_matrix.h"

static const keypad_matrix_pin_t g_rows[4] =
{
    { &PORTB, &TRISB, 0u },
    { &PORTB, &TRISB, 1u },
    { &PORTB, &TRISB, 2u },
    { &PORTB, &TRISB, 3u }
};

static const keypad_matrix_pin_t g_cols[3] =
{
    { &PORTB, &TRISB, 4u },
    { &PORTB, &TRISB, 5u },
    { &PORTB, &TRISB, 6u }
};

static const char g_keymap[12] =
{
    '1', '2', '3',
    '4', '5', '6',
    '7', '8', '9',
    '*', '0', '#'
};

void main(void)
{
    keypad_matrix_t keypad;
    keypad_matrix_config_t cfg;
    char key;

    uart_init(9600u);

    cfg.rows = 4u;
    cfg.cols = 3u;
    cfg.row_pins = g_rows;
    cfg.col_pins = g_cols;
    cfg.keymap = g_keymap;
    cfg.debounce_scans = 3u;
    cfg.scan_settle_cycles = 8u;
    cfg.pressed_level = 0u;

    (void)keypad_matrix_init(&keypad, &cfg);

    while (1)
    {
        keypad_matrix_scan(&keypad);
        key = keypad_matrix_get_key(&keypad);

        if (key != '\0')
        {
            uart_write_string("KEY: ");
            uart_write_byte((uint8_t)key);
            uart_write_string("\r\n");
        }
    }
}