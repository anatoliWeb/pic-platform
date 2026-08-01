# ws2812_matrix

## Purpose

WS2812 matrix helper.

## Location

```text
libraries/led/ws2812_matrix/ws2812_matrix.h
libraries/led/ws2812_matrix/ws2812_matrix.c
```

## Use when

- you need reusable WS2812 matrix addressing.

## Do not use when

- a simple strip mapping is enough.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `WS2812_MATRIX_INVALID_INDEX` | macro | invalid index | none | value | none |  |
| `ws2812_matrix_layout_t` | enum | layout | layout values | value | none |  |
| `ws2812_matrix_origin_t` | enum | origin | origin values | value | none |  |
| `ws2812_matrix_config_t` | struct | config | layout/origin/size | state | none |  |
| `ws2812_matrix_t` | struct | state | runtime state | state | none |  |
| `ws2812_matrix_init` | `uint8_t ws2812_matrix_init(...);` | initialize matrix | config | success flag | configures mapping | exact args in header |
| `ws2812_matrix_get_index` | `uint16_t ws2812_matrix_get_index(...);` | map coordinates | matrix, x, y | index | none | exact args in header |
| `ws2812_matrix_set_pixel` | `uint8_t ws2812_matrix_set_pixel(...);` | set pixel | matrix, x, y, color | status | buffer update | exact args in header |
| `ws2812_matrix_clear` | `void ws2812_matrix_clear(...);` | clear matrix | matrix | none | clears buffer | exact args in header |
| `ws2812_matrix_fill` | `void ws2812_matrix_fill(...);` | fill matrix | matrix, color | none | fills buffer | exact args in header |
| `ws2812_matrix_show` | `void ws2812_matrix_show(...);` | flush matrix | matrix | none | sends frame | exact args in header |
| `ws2812_matrix_draw_line` | `uint8_t ws2812_matrix_draw_line(...);` | draw line | matrix, endpoints, color | status | drawing | exact args in header |
| `ws2812_matrix_draw_rect` | `uint8_t ws2812_matrix_draw_rect(...);` | draw rect | matrix, rectangle, color | status | drawing | exact args in header |

## Configuration structures

Public fields are the config and state structs described in the header.

## Error/status model

Init returns a success flag; draw helpers return status where applicable.

## Source inclusion strategy

### Shared implementation

```text
libraries/led/ws2812_matrix/ws2812_matrix.c
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
libraries/led/ws2812_matrix/ws2812_matrix.c

For C18 add:
libraries/led/ws2812_matrix/ws2812_matrix.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/led/ws2812_matrix/ws2812_matrix.h
libraries/led/ws2812_matrix/ws2812_matrix.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
libraries/led/ws2812/ws2812.h
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

Not applicable for this module.

## Runtime model

Coordinate mapping is converted into strip indices before flushing.

## ISR requirements

Not applicable for this module.

## Resource ownership

Matrix pixels are backed by a WS2812 strip.

## Integration recipe

1. Add the shared matrix source.
2. Configure layout and origin.
3. Draw pixels or shapes.
4. Flush with `ws2812_matrix_show()`.

## Minimal usable example

```c
ws2812_matrix_t matrix;
ws2812_matrix_init(&matrix, &cfg);
ws2812_matrix_show(&matrix);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Layout mapping is fixed by configuration.

## Common mistakes

- Forgetting to flush.
- Using the wrong matrix origin.

## AI decision rule

Use this helper for WS2812 matrix addressing.
