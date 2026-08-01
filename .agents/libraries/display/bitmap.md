# bitmap

## Purpose

Bitmap blitting helper.

## Location

```text
libraries/display/bitmap/bitmap.h
libraries/display/bitmap/bitmap.c
```

## Use when

- you need reusable bitmap drawing helpers.

## Do not use when

- you only need a display backend.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `bitmap_format_t` | enum | bitmap format | `BITMAP_FORMAT_1BPP_ROW`, `BITMAP_FORMAT_UNKNOWN` | format | none |  |
| `bitmap_t` | struct | bitmap descriptor | width, height, data, format | state | none |  |
| `bitmap_draw` | `void bitmap_draw(graphics_t* g, int16_t x, int16_t y, const bitmap_t* bmp, uint8_t color);` | draw bitmap | graphics, position, bitmap, color | none | drawing |  |
| `bitmap_draw_transparent` | `void bitmap_draw_transparent(...);` | draw bitmap | graphics, position, bitmap, color | none | drawing | exact args in header |
| `bitmap_draw_region` | `void bitmap_draw_region(...);` | draw region | graphics, position, bitmap, source rect, color | none | drawing | exact args in header |
| `bitmap_draw_scaled` | `void bitmap_draw_scaled(...);` | draw scaled bitmap | graphics, position, scale, bitmap, color | none | drawing | exact args in header |
| `bitmap_draw_inverted` | `void bitmap_draw_inverted(graphics_t* g, int16_t x, int16_t y, const bitmap_t* bmp, uint8_t color);` | draw inverted bitmap | graphics, position, bitmap, color | none | drawing |  |

## Configuration structures

Public fields are the `bitmap_t` members described in the header.

## Error/status model

Not applicable for this module.

## Source inclusion strategy

### Shared implementation

```text
libraries/display/bitmap/bitmap.c
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
libraries/display/bitmap/bitmap.c

For C18 add:
libraries/display/bitmap/bitmap.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/display/bitmap/bitmap.h
libraries/display/bitmap/bitmap.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
libraries/display/graphics/graphics.h
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

Bitmap helpers operate on the supplied graphics context.

## ISR requirements

Not applicable for this module.

## Resource ownership

Bitmap data stays with the caller.

## Integration recipe

1. Add the shared bitmap source.
2. Provide a graphics context.
3. Call the bitmap draw helpers.

## Minimal usable example

```c
bitmap_draw(&g, 0, 0, &bmp, 1u);
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

Bitmap format support is limited to the declared public formats.

## Extension points

Keep format conversions in reusable code if needed.

## Common mistakes

- Passing a bitmap with a mismatched format.
- Treating this as a display backend.

## AI decision rule

Use this helper for reusable bitmap rendering.
