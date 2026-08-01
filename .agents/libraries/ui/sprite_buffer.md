# sprite_buffer

## Purpose

Sprite/framebuffer helper.

## Location

```text
libraries/ui/sprite_buffer/sprite_buffer.h
libraries/ui/sprite_buffer/sprite_buffer.c
```

## Use when

- you need reusable sprite operations and blitting.

## Do not use when

- the project does not use sprite buffers.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `sprite_format_t` | enum | sprite format | `SPRITE_FORMAT_1BPP_ROW`, `SPRITE_FORMAT_FUTURE_RGB` | format | none |  |
| `sprite_t` | struct | sprite state | buffer/size/format | state | none |  |
| `sprite_init` | `uint8_t sprite_init(...);` | initialize sprite | state, buffer, size, format | success flag | stores state | exact args in header |
| `sprite_clear` | `void sprite_clear(...);` | clear sprite | sprite | none | clears buffer | exact args in header |
| `sprite_draw_pixel` | `void sprite_draw_pixel(...);` | draw pixel | sprite, x, y, color | none | updates buffer | exact args in header |
| `sprite_fill_rect` | `void sprite_fill_rect(...);` | fill rect | sprite, rectangle, color | none | updates buffer | exact args in header |
| `sprite_draw_bitmap` | `void sprite_draw_bitmap(...);` | draw bitmap | sprite, position, bitmap | none | updates buffer | exact args in header |
| `sprite_draw_sprite` | `void sprite_draw_sprite(...);` | draw sprite | dst, src, position | none | blits buffer | exact args in header |
| `sprite_copy_region` | `void sprite_copy_region(...);` | copy region | dst, src, rect | none | copies buffer | exact args in header |
| `sprite_blit` | `void sprite_blit(...);` | blit | dst, src, position | none | copies buffer | exact args in header |
| `sprite_blit_transparent` | `void sprite_blit_transparent(...);` | transparent blit | dst, src, position | none | copies buffer | exact args in header |

## Configuration structures

Public fields are the `sprite_t` members described in the header.

## Error/status model

Init returns a success flag.

## Source inclusion strategy

### Shared implementation

```text
libraries/ui/sprite_buffer/sprite_buffer.c
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
libraries/ui/sprite_buffer/sprite_buffer.c

For C18 add:
libraries/ui/sprite_buffer/sprite_buffer.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/ui/sprite_buffer/sprite_buffer.h
libraries/ui/sprite_buffer/sprite_buffer.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
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

Sprite operations are synchronous and caller-owned.

## ISR requirements

Not applicable for this module.

## Resource ownership

Sprite buffers are caller-owned.

## Integration recipe

1. Add the shared sprite buffer source.
2. Initialize the buffer.
3. Draw and blit sprites.

## Minimal usable example

```c
sprite_t sprite;
sprite_init(&sprite, buffer, size, SPRITE_FORMAT_1BPP_ROW);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Format support is limited to the declared public formats.

## Common mistakes

- Using a buffer with the wrong format.

## AI decision rule

Use this module for reusable sprite buffer operations.
