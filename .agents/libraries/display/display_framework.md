# display_framework

## Purpose

Thin display abstraction that routes drawing calls to LCD, seven-segment, or WS2812 matrix backends.

## Location

```text
libraries/display/display_framework/display_framework.h
libraries/display/display_framework/display_framework.c
docs/libraries/display/display_framework.md
```

## Use when

- you need one display API across multiple backends;
- you want backend selection without rewriting application drawing code.

## Do not use when

- you need a full GUI system;
- you need a framebuffer-owned rendering stack.

## Public API

| Function/type | Purpose | What to pass | Returns / output | Notes |
| --- | --- | --- | --- | --- |
| `display_driver_t` | backend callback table | clear/update/pixel/char callbacks | struct | backend contract |
| `display_t` | runtime display object | backend, ctx, size | struct | no dynamic memory |
| `display_init()` | initialize display | display, driver, ctx, width, height | `uint8_t` | returns success flag |
| `display_clear()` | clear output | display | none | forwards to backend |
| `display_update()` | flush output | display | none | forwards to backend |
| `display_draw_pixel()` | draw one pixel | display, x, y, color | none | backend may ignore |
| `display_draw_char()` | draw one char | display, x, y, char | none | backend may ignore color depth |
| `display_draw_string()` | draw string | display, x, y, string | none | newline advances line |
| `display_draw_number()` | draw signed number | display, x, y, value | none | decimal text formatting |
| `display_draw_line()` | draw line | display, endpoints, color | none | Bresenham-style |
| `display_draw_rect()` | draw rectangle | display, x, y, w, h, color, filled | none | filled or outline |
| `display_backend_lcd_hd44780()` | install LCD backend | driver table | none | backend adapter |
| `display_backend_seven_segment()` | install seven-segment backend | driver table | none | backend adapter |
| `display_backend_ws2812_matrix()` | install matrix backend | driver table | none | backend adapter |

## Configuration structures

| Field | Type | Required | Meaning | Valid range | Notes |
| --- | --- | --- | --- | --- | --- |
| `display_driver_t.clear` | function pointer | yes | clear callback | valid fn ptr | backend owns action |
| `display_driver_t.update` | function pointer | yes | refresh callback | valid fn ptr | backend owns action |
| `display_driver_t.draw_pixel` | function pointer | yes | pixel callback | valid fn ptr | may be no-op |
| `display_driver_t.draw_char` | function pointer | yes | char callback | valid fn ptr | may be no-op |
| `display_t.ctx` | `void*` | yes | backend context | backend object | points to LCD/display object |
| `display_t.width` | `uint16_t` | yes | width | backend-specific | stored for app use |
| `display_t.height` | `uint16_t` | yes | height | backend-specific | stored for app use |

## Error/status model

| Status/error | Meaning | Typical cause | Required handling |
| --- | --- | --- | --- |
| `uint8_t` from `display_init()` | success flag | null driver/display | abort init path |

## Source inclusion strategy

### Shared implementation

```text
libraries/display/display_framework/display_framework.c
```

### XC8 implementation/wrapper

```text
none
```

### C18 implementation/wrapper

```text
none
```

### Select exactly one route

```text
Add the shared source once. Do not pair it with compiler wrappers because none exist.
```

## Required files

```text
libraries/display/display_framework/display_framework.h
libraries/display/display_framework/display_framework.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
libraries/display/lcd_hd44780/lcd.h
libraries/display/seven_segment/seven_segment.h
libraries/led/ws2812_matrix/ws2812_matrix.h
libraries/led/rgb_utils/rgb_utils.h
```

### Callback-provided dependencies

```text
backend function pointers from the selected display implementation
```

### Optional feature dependencies

```text
LCD backend, seven-segment backend, WS2812 matrix backend
```

### Example-only dependencies

```text
display backend objects used by application examples
```

### Transitive dependencies

```text
none beyond selected backend APIs
```

## Compile-time defines

| Define | Default | Where defined | Effect | Required files | Resource impact | Code-size impact |
| --- | ---: | --- | --- | --- | --- | --- |
| none module-specific | n/a | n/a | backend selected at runtime | shared source | backend-owned | small |

## Runtime model

- `display_t` wraps backend callbacks and display size.
- Some operations are no-op on limited backends.
- No framebuffer ownership is taken by the framework.

## Resource ownership

- Backend object ownership stays with the caller.
- Display pin ownership belongs to the selected backend.

## Integration recipe

1. Add the shared display framework source.
2. Add the chosen backend library.
3. Initialize `display_driver_t` with one backend adapter.
4. Call `display_init()` with backend context and dimensions.
5. Draw through the abstract API.

## Minimal usable example

```c
display_t display;
display_driver_t driver;

display_backend_lcd_hd44780(&driver);
display_init(&display, &driver, &lcd, 16u, 2u);
display_draw_string(&display, 0u, 0u, "OK");
```

## Working examples

```text
docs/libraries/display/display_framework.md
```

## Human documentation

```text
docs/libraries/display/display_framework.md
```

## Known limitations

- Not a GUI stack.
- Some backends ignore unsupported primitives.

## AI decision rule

Use this abstraction when the application must switch display backends without changing drawing code.
