# graphics

## Purpose

Portable 2D graphics helper layer.

## Location

```text
libraries/display/graphics/graphics.h
libraries/display/graphics/graphics.c
```

## Use when

- you need reusable drawing primitives over a framebuffer or callbacks.

## Do not use when

- the module is not needed for drawing primitives.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `graphics_set_pixel_cb_t` | function pointer typedef | set pixel callback | context, x, y, color | none | callback |  |
| `graphics_get_pixel_cb_t` | function pointer typedef | get pixel callback | context, x, y | color | callback |  |
| `graphics_flush_cb_t` | function pointer typedef | flush callback | context | none | callback |  |
| `graphics_format_t` | enum | framebuffer format | `GRAPHICS_FMT_1BPP_VPAGE`, `GRAPHICS_FMT_CUSTOM` | format | none |  |
| `graphics_t` | struct | graphics context | size, framebuffer, callbacks, context | state | none |  |
| `graphics_init` | `uint8_t graphics_init(graphics_t* g, uint16_t width, uint16_t height, uint8_t* framebuffer, uint16_t framebuffer_size, graphics_format_t format, graphics_set_pixel_cb_t set_cb, graphics_get_pixel_cb_t get_cb, graphics_flush_cb_t flush_cb, void* user_ctx);` | initialize graphics | context and callbacks | success flag | stores context |  |
| `graphics_draw_pixel` | `void graphics_draw_pixel(...);` | draw pixel | graphics, x, y, color | none | updates framebuffer or callback target | exact args in header |
| `graphics_draw_line` | `void graphics_draw_line(...);` | draw line | graphics, endpoints, color | none | drawing |  |
| `graphics_draw_rect` | `void graphics_draw_rect(...);` | draw rect | graphics, rectangle, color | none | drawing |  |
| `graphics_fill_rect` | `void graphics_fill_rect(...);` | fill rect | graphics, rectangle, color | none | drawing |  |
| `graphics_draw_circle` | `void graphics_draw_circle(...);` | draw circle | graphics, center, radius, color | none | drawing |  |
| `graphics_draw_char` | `void graphics_draw_char(...);` | draw char | graphics, x, y, char, color | none | drawing |  |
| `graphics_draw_string` | `void graphics_draw_string(...);` | draw string | graphics, x, y, string, color | none | drawing |  |
| `graphics_draw_triangle` | `void graphics_draw_triangle(...);` | draw triangle | graphics, points, color | none | drawing |  |
| `graphics_fill_circle` | `void graphics_fill_circle(...);` | fill circle | graphics, center, radius, color | none | drawing |  |
| `graphics_flush` | `void graphics_flush(graphics_t* g);` | flush drawing | graphics | none | flushes target |  |

## Configuration structures

Public fields are the `graphics_t` members described in the header.

## Error/status model

`graphics_init()` returns a success flag.

## Source inclusion strategy

### Shared implementation

```text
libraries/display/graphics/graphics.c
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
libraries/display/graphics/graphics.c

For C18 add:
libraries/display/graphics/graphics.c

Do not add together:
not applicable

Reason:
this module is independent and has no compiler wrapper split.
```

## Required files

```text
libraries/display/graphics/graphics.h
libraries/display/graphics/graphics.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
```

### Callback-provided dependencies

```text
graphics_set_pixel_cb_t, graphics_get_pixel_cb_t, graphics_flush_cb_t
```

### Optional feature dependencies

Not applicable for this module.

### Example-only dependencies

Not applicable for this module.

### Transitive dependencies

Not applicable for this module.

## Compile-time defines

Not applicable for this module.

## Runtime model

Graphics primitives operate on the initialized context and optional callbacks.

## ISR requirements

Not applicable for this module.

## Resource ownership

Framebuffer and callback target ownership stay with the caller.

## Integration recipe

1. Add the shared graphics source.
2. Initialize with the desired framebuffer or callbacks.
3. Draw primitives.
4. Flush if using a buffered backend.

## Minimal usable example

```c
graphics_t g;
graphics_init(&g, 128u, 64u, framebuffer, sizeof(framebuffer), GRAPHICS_FMT_1BPP_VPAGE, 0, 0, 0, 0);
graphics_draw_pixel(&g, 0u, 0u, 1u);
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

Performance depends on framebuffer size and callbacks.

## Extension points

Keep device-specific rendering in display backends.

## Common mistakes

- Treating the helper as a backend driver.
- Omitting callback dependencies when needed.

## AI decision rule

Use this helper for reusable graphics primitives.
