[Ukrainian version](./keypad_matrix.ua.md)

# Keypad Matrix Library

## Description
Reusable matrix keypad library for embedded projects.
Supports configurable matrix dimensions and key mapping.

## Public API
- `keypad_matrix_init()`
- `keypad_matrix_scan()`
- `keypad_matrix_get_key()`
- `keypad_matrix_is_pressed()`

## Supported Matrix Sizes
- 2x2
- 3x4
- 4x4
- custom size up to 8x8 (`KEYPAD_MATRIX_MAX_ROWS/COLS`)

## Configuration Model
Use `keypad_matrix_config_t`:
- row pin list (`row_pins`)
- column pin list (`col_pins`)
- matrix dimensions (`rows`, `cols`)
- key map (`keymap` as flattened row-major array)
- debounce scans (`debounce_scans`)
- settle cycles (`scan_settle_cycles`)
- column pressed logic level (`pressed_level`)

## Key Map Example (3x4)
```c
static const char keymap[12] =
{
    '1','2','3',
    '4','5','6',
    '7','8','9',
    '*','0','#'
};
```

## Wiring Overview
- Rows: MCU outputs (driven one by one)
- Columns: MCU inputs (read every scan)
- Typical hardware: column pull-up resistors, active-low key press

Example signal flow:
- set all rows inactive (HIGH)
- drive one row active (LOW)
- read all columns
- repeat for next row

## Debounce
Debounce is scan-based.
A key is accepted only after `debounce_scans` consecutive equal detections.

## Dependencies
- `core/*`
- `drivers/gpio/*`

## Notes
- No dynamic memory
- Blocking/simple scan model
- Suitable for periodic calls from main loop or scheduler