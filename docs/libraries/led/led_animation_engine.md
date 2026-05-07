# LED Animation Engine

## Description

`led_animation_engine` is a non-blocking runtime layer for coordinated LED animations.
It manages multiple animations over segments and matrix targets using a fixed-size pool.

## Public API

- `led_animation_engine_init()`
- `led_animation_engine_add()`
- `led_animation_engine_remove()`
- `led_animation_engine_update()`
- `led_animation_engine_pause()`
- `led_animation_engine_resume()`
- `led_animation_engine_is_running()` (optional helper)

## Engine Model

- Fixed animation pool (`LED_ANIM_ENGINE_MAX_ANIMATIONS`).
- Each entry stores:
  - target (`segment` or `matrix`)
  - effect type
  - speed in ticks
  - color
  - enabled/paused flags
  - compact runtime state (index, direction, level, hue).
- No dynamic allocation.

## Update Scheduling

Each `led_animation_engine_update()` call:

1. Iterates all active entries.
2. Increments per-entry tick counters.
3. Runs effect step only when `tick_counter >= speed_ticks`.
4. Writes updated pixels to target buffer.
5. Optionally triggers target show if `auto_show` is enabled.

This works from main loop, scheduler, or RTOS task.

## Implemented Effects

- Blink
- Breathing
- Rainbow
- Color wipe
- Scanner
- Pulse

## Memory Model

- Uses parent strip/matrix framebuffer only.
- No framebuffer duplication.
- Runtime state per animation is small and deterministic.

## Dependencies

- `core/*`
- `libraries/led/ws2812`
- `libraries/led/rgb_utils`
- `libraries/led/led_effects`
- `libraries/led/led_segments`
- `libraries/led/ws2812_matrix`

## PIC18 Notes

- Use reasonable animation count for RAM limits.
- Prefer `auto_show = 0` and one shared `ws2812_show()` per loop for best efficiency.
- No blocking delays are used in the engine.
