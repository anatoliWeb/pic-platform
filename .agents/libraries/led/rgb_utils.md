# rgb_utils

## Purpose

RGB and HSV color helpers.

## Location

```text
libraries/led/rgb_utils/rgb_utils.h
libraries/led/rgb_utils/rgb_utils.c
```

## Use when

- you need reusable color math helpers.

## Do not use when

- the project does not need color utilities.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `rgb_color_t` | struct | RGB color | r/g/b | state | none |  |
| `hsv_color_t` | struct | HSV color | h/s/v | state | none |  |
| `rgb_color` | `rgb_color_t rgb_color(uint8_t r, uint8_t g, uint8_t b);` | construct RGB | components | color | none |  |
| `rgb_scale` | `rgb_color_t rgb_scale(...);` | scale color | color, factor | color | none | exact args in header |
| `rgb_blend` | `rgb_color_t rgb_blend(...);` | blend colors | a, b, factor | color | none | exact args in header |
| `rgb_add` | `rgb_color_t rgb_add(...);` | add colors | a, b | color | none | exact args in header |
| `rgb_subtract` | `rgb_color_t rgb_subtract(...);` | subtract colors | a, b | color | none | exact args in header |
| `hsv_to_rgb` | `rgb_color_t hsv_to_rgb(hsv_color_t hsv);` | convert HSV to RGB | hsv | color | none |  |

## Configuration structures

Public fields are the color structs described in the header.

## Error/status model

Not applicable for this module.

## Source inclusion strategy

### Shared implementation

```text
libraries/led/rgb_utils/rgb_utils.c
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
libraries/led/rgb_utils/rgb_utils.c

For C18 add:
libraries/led/rgb_utils/rgb_utils.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/led/rgb_utils/rgb_utils.h
libraries/led/rgb_utils/rgb_utils.c
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

Color helpers are pure functions.

## ISR requirements

Not applicable for this module.

## Resource ownership

Color values are caller-owned data.

## Integration recipe

1. Add the shared color helper source.
2. Use color constructors and transforms where needed.

## Minimal usable example

```c
rgb_color_t c = rgb_color(255u, 0u, 0u);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Color math is limited to the public helpers.

## Common mistakes

- Expecting side effects.
- Treating the helpers as hardware drivers.

## AI decision rule

Use this helper for reusable RGB/HSV operations.
