#include "libraries/input/keypad_matrix/keypad_matrix.h"

#if defined(DRV_COMPILER_C18)
    #include "../../../C18/libraries/input/keypad_matrix/keypad_matrix.c"
#elif defined(DRV_COMPILER_XC8)
    #include "../../../XC8/libraries/input/keypad_matrix/keypad_matrix.c"
#else

#include "drivers/gpio/gpio.h"

/* Drive all rows to inactive level before scanning next row. */
static void keypad_matrix_set_all_rows_inactive(const keypad_matrix_t* keypad)
{
    uint8_t i;

    for (i = 0u; i < keypad->config.rows; i++)
    {
        gpio_write_high(keypad->config.row_pins[i].port, keypad->config.row_pins[i].bit);
    }
}

/* Activate one row and keep others inactive. */
static void keypad_matrix_set_row_active(const keypad_matrix_t* keypad, uint8_t row)
{
    gpio_write_low(keypad->config.row_pins[row].port, keypad->config.row_pins[row].bit);
}

/* Minimal settle loop for matrix line stabilization. */
static void keypad_matrix_wait_settle(uint8_t cycles)
{
    volatile uint8_t i;

    for (i = 0u; i < cycles; i++)
    {
    }
}

/* Scan matrix once and return first detected key position. */
static uint8_t keypad_matrix_scan_once(keypad_matrix_t* keypad, uint8_t* out_row, uint8_t* out_col)
{
    uint8_t row;
    uint8_t col;

    for (row = 0u; row < keypad->config.rows; row++)
    {
        keypad_matrix_set_all_rows_inactive(keypad);
        keypad_matrix_set_row_active(keypad, row);
        keypad_matrix_wait_settle(keypad->config.scan_settle_cycles);

        for (col = 0u; col < keypad->config.cols; col++)
        {
            uint8_t level = gpio_read(keypad->config.col_pins[col].port, keypad->config.col_pins[col].bit);

            if (level == keypad->config.pressed_level)
            {
                *out_row = row;
                *out_col = col;
                keypad_matrix_set_all_rows_inactive(keypad);
                return 1u;
            }
        }
    }

    keypad_matrix_set_all_rows_inactive(keypad);
    return 0u;
}

uint8_t keypad_matrix_init(keypad_matrix_t* keypad, const keypad_matrix_config_t* config)
{
    uint8_t i;

    if ((keypad == (keypad_matrix_t*)0) || (config == (const keypad_matrix_config_t*)0))
    {
        return 0u;
    }

    if ((config->rows == 0u) || (config->cols == 0u))
    {
        return 0u;
    }

    if ((config->rows > KEYPAD_MATRIX_MAX_ROWS) || (config->cols > KEYPAD_MATRIX_MAX_COLS))
    {
        return 0u;
    }

    if ((config->row_pins == (const keypad_matrix_pin_t*)0) ||
        (config->col_pins == (const keypad_matrix_pin_t*)0) ||
        (config->keymap == (const char*)0))
    {
        return 0u;
    }

    /* Copy runtime configuration to instance state. */
    keypad->config = *config;

    if (keypad->config.debounce_scans == 0u)
    {
        keypad->config.debounce_scans = 2u;
    }

    keypad->candidate_valid = 0u;
    keypad->candidate_row = 0u;
    keypad->candidate_col = 0u;
    keypad->stable_count = 0u;
    keypad->pressed = 0u;
    keypad->active_row = 0u;
    keypad->active_col = 0u;
    keypad->active_key = '\0';

    for (i = 0u; i < keypad->config.rows; i++)
    {
        gpio_set_output(keypad->config.row_pins[i].tris, keypad->config.row_pins[i].bit);
        gpio_write_high(keypad->config.row_pins[i].port, keypad->config.row_pins[i].bit);
    }

    for (i = 0u; i < keypad->config.cols; i++)
    {
        gpio_set_input(keypad->config.col_pins[i].tris, keypad->config.col_pins[i].bit);
    }

    keypad->initialized = 1u;
    return 1u;
}

void keypad_matrix_scan(keypad_matrix_t* keypad)
{
    uint8_t found;
    uint8_t row = 0u;
    uint8_t col = 0u;

    if ((keypad == (keypad_matrix_t*)0) || (keypad->initialized == 0u))
    {
        return;
    }

    /* Perform one matrix scan cycle. */
    found = keypad_matrix_scan_once(keypad, &row, &col);

    if (found != 0u)
    {
        if ((keypad->candidate_valid != 0u) &&
            (keypad->candidate_row == row) &&
            (keypad->candidate_col == col))
        {
            if (keypad->stable_count < 255u)
            {
                keypad->stable_count++;
            }
        }
        else
        {
            keypad->candidate_valid = 1u;
            keypad->candidate_row = row;
            keypad->candidate_col = col;
            keypad->stable_count = 1u;
        }

        /* Accept key only after debounce threshold. */
        if (keypad->stable_count >= keypad->config.debounce_scans)
        {
            keypad->pressed = 1u;
            keypad->active_row = row;
            keypad->active_col = col;
            keypad->active_key = keypad->config.keymap[(uint16_t)row * keypad->config.cols + col];
        }
    }
    else
    {
        keypad->candidate_valid = 0u;
        keypad->stable_count = 0u;
        keypad->pressed = 0u;
        keypad->active_key = '\0';
    }
}

char keypad_matrix_get_key(const keypad_matrix_t* keypad)
{
    if ((keypad == (const keypad_matrix_t*)0) || (keypad->pressed == 0u))
    {
        return '\0';
    }

    return keypad->active_key;
}

uint8_t keypad_matrix_is_pressed(const keypad_matrix_t* keypad, char key)
{
    if ((keypad == (const keypad_matrix_t*)0) || (keypad->pressed == 0u))
    {
        return 0u;
    }

    return (uint8_t)(keypad->active_key == key ? 1u : 0u);
}

#endif