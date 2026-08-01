# keypad_matrix

## Purpose

Keypad matrix scanner.

## Location

```text
libraries/input/keypad_matrix/keypad_matrix.h
libraries/input/keypad_matrix/keypad_matrix.c
```

## Use when

- you need reusable row/column keypad scanning.

## Do not use when

- the hardware is not a matrix keypad.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `KEYPAD_MATRIX_MAX_ROWS` | macro | max rows | none | value | none |  |
| `KEYPAD_MATRIX_MAX_COLS` | macro | max cols | none | value | none |  |
| `keypad_matrix_pin_t` | struct | pin descriptor | port/tris/pin | state | none |  |
| `keypad_matrix_config_t` | struct | scanner config | rows, cols, map | state | none |  |
| `keypad_matrix_t` | struct | scanner state | runtime state | state | none |  |
| `keypad_matrix_init` | `uint8_t keypad_matrix_init(...);` | initialize scanner | config | success flag | stores config | exact args in header |
| `keypad_matrix_scan` | `void keypad_matrix_scan(...);` | scan keypad | scanner | none | reads row/col pins | exact args in header |
| `keypad_matrix_get_key` | `char keypad_matrix_get_key(...);` | decoded key | scanner | key | none | exact args in header |
| `keypad_matrix_is_pressed` | `uint8_t keypad_matrix_is_pressed(...);` | pressed state | scanner, key | flag | none | exact args in header |

## Configuration structures

Public fields are the config and state structs described in the header.

## Error/status model

`keypad_matrix_init()` returns a success flag.

## Source inclusion strategy

### Shared implementation

```text
libraries/input/keypad_matrix/keypad_matrix.c
```

### XC8 implementation/wrapper

```text
Not applicable for this module.
```

### C18 implementation/wrapper

```text
Not applicable for this module.
```

### Select exactly one route

```text
For XC8 add:
libraries/input/keypad_matrix/keypad_matrix.c

For C18 add:
libraries/input/keypad_matrix/keypad_matrix.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/input/keypad_matrix/keypad_matrix.h
libraries/input/keypad_matrix/keypad_matrix.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
drivers/gpio/gpio.h
```

### Callback-provided dependencies

Not applicable for this module.

### Optional feature dependencies

Not applicable for this module.

### Example-only dependencies

Not applicable for this module.

### Transitive dependencies

Not applicable for this module.

## Compile-time defines

| Define | Default | Where | Effect | Required files | Resource impact | Code-size impact |
| --- | ---: | --- | --- | --- | --- | --- |
| `KEYPAD_MATRIX_MAX_ROWS` | project-specific | header | max rows | shared source | table size | small |
| `KEYPAD_MATRIX_MAX_COLS` | project-specific | header | max cols | shared source | table size | small |

## Runtime model

The scanner updates key state from the current matrix scan.

## ISR requirements

Not applicable for this module.

## Resource ownership

Matrix row and column pins are project-owned.

## Integration recipe

1. Add the shared keypad matrix source.
2. Configure row and column pins.
3. Initialize the scanner.
4. Call scan periodically.

## Minimal usable example

```c
keypad_matrix_t keypad;
keypad_matrix_init(&keypad, &cfg);
keypad_matrix_scan(&keypad);
```

## Working examples

Not applicable for this module.

## Proteus integration

Not applicable for this module.

## HEX artifacts

Not applicable for this module.

## Human documentation

Not applicable for this module.

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Matrix size is bounded by the compile-time limits.

## Extension points

Keep input event mapping in reusable layers.

## Common mistakes

- Exceeding row/column limits.
- Forgetting to debounce at a higher layer if needed.

## AI decision rule

Use this module for reusable keypad matrix scanning.
