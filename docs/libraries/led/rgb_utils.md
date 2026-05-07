[Ukrainian version](./rgb_utils.ua.md)

# RGB Utilities Library

## Description
Reusable color utility library for LED systems.
Provides lightweight integer RGB/HSV operations for embedded targets.

## Public API
- `rgb_color()`
- `rgb_scale()`
- `rgb_blend()`
- `rgb_add()`
- `rgb_subtract()`
- `hsv_to_rgb()`

## Supported Color Models
- RGB (`rgb_color_t`)
- HSV (`hsv_color_t`)

## Integer Math Notes
- All calculations use integer arithmetic only.
- No floating-point dependency.
- Intermediate values use 16-bit math to avoid overflow during scaling/blending.

## Brightness Scaling
- `rgb_scale(color, scale)` applies global/per-color brightness in `0..255` domain.
- `0` = off, `255` = unchanged.

## Blending
- `rgb_blend(a, b, alpha)` uses linear interpolation:
  - `alpha=0` -> `a`
  - `alpha=255` -> `b`

## Extra Helpers
- `rgb_add()` with saturation at 255.
- `rgb_subtract()` with floor at 0.

## Embedded Optimization Notes
- No dynamic memory allocation.
- Minimal stack usage.
- Suitable foundation for effects engines and matrix pipelines.

## Dependencies
- `core/*` only