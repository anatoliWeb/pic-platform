[Ukrainian version](./led_effects.ua.md)

# LED Effects Library

## Description
Reusable non-blocking effects engine for addressable LEDs.
Built on top of `ws2812` and `rgb_utils` libraries.

## Public API
- `led_effects_init()`
- `led_effects_set_mode()`
- `led_effects_update()`
- `led_effects_set_speed()`
- `led_effects_set_color()`
- `led_effects_is_finished()`

## Implemented Effects
- blink
- breathing
- rainbow cycle
- color wipe
- scanner
- pulse

## Non-Blocking Animation Model
- No `delay()` calls inside effect engine.
- Every `led_effects_update()` advances effect state by one incremental step.
- Effect speed is controlled by internal update-divider (`speed`).
- Works with main loop, scheduler, or RTOS task.

## Update Loop Usage
Typical loop:
1. call `led_effects_update()` frequently
2. optionally process input/events
3. switch mode/color/speed via API

## Performance Considerations
- `ws2812_show()` itself is timing-critical and blocking during frame push.
- Keep LED count and update frequency balanced for your MCU budget.
- Use lower brightness where possible to reduce power/current stress.

## RAM Usage Notes
- No dynamic memory allocation in effects layer.
- State is a compact struct (`led_effects_t`).
- Pixel framebuffer memory stays in `ws2812` layer.

## Dependencies
- `core/*`
- `libraries/led/ws2812/*`
- `libraries/led/rgb_utils/*`