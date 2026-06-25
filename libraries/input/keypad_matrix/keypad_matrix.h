/*
 * File: libraries/input/keypad_matrix/keypad_matrix.h
 */

#ifndef LIBRARIES_INPUT_KEYPAD_MATRIX_H
#define LIBRARIES_INPUT_KEYPAD_MATRIX_H

#include "core/compiler.h"
#include "core/types.h"

#define KEYPAD_MATRIX_MAX_ROWS 8u
#define KEYPAD_MATRIX_MAX_COLS 8u

typedef struct
{
    volatile uint8_t* port;
    volatile uint8_t* tris;
    uint8_t bit;
} keypad_matrix_pin_t;

typedef struct
{
    uint8_t rows;
    uint8_t cols;

    const keypad_matrix_pin_t* row_pins;
    const keypad_matrix_pin_t* col_pins;

    /* Flattened key map in row-major order (rows * cols entries). */
    const char* keymap;

    /* Debounce threshold in equal scan results. */
    uint8_t debounce_scans;

    /* Small settle delay after row activation. */
    uint8_t scan_settle_cycles;

    /* Logic level treated as pressed on column input (0 or 1). */
    uint8_t pressed_level;
} keypad_matrix_config_t;

typedef struct
{
    keypad_matrix_config_t config;

    uint8_t initialized;

    uint8_t candidate_valid;
    uint8_t candidate_row;
    uint8_t candidate_col;
    uint8_t stable_count;

    uint8_t pressed;
    uint8_t active_row;
    uint8_t active_col;

    char active_key;
} keypad_matrix_t;

uint8_t keypad_matrix_init(keypad_matrix_t* keypad, const keypad_matrix_config_t* config);

void keypad_matrix_scan(keypad_matrix_t* keypad);

char keypad_matrix_get_key(const keypad_matrix_t* keypad);

uint8_t keypad_matrix_is_pressed(const keypad_matrix_t* keypad, char key);

#endif /* LIBRARIES_INPUT_KEYPAD_MATRIX_H */