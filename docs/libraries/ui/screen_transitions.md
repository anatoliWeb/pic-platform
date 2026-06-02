[Ukrainian version](./screen_transitions.ua.md)

# Screen Transitions Library

## Description

`screen_transitions` provides a lightweight non-blocking transition engine for
screen changes in embedded UI systems.

It is display-agnostic and hardware-independent.

## Public API

- `screen_transition_init()`
- `screen_transition_start()`
- `screen_transition_update()`
- `screen_transition_draw()`
- `screen_transition_is_active()`
- `screen_transition_cancel()` (optional)
- `screen_transition_set_speed()` (optional)

## Transition / Render Flow Notes

Transition model (`transition_t`) fields:

- `type`
- `duration_ms`
- `progress` (0..1000)
- `active`
- `source_screen`
- `target_screen`
- `user_data`

Supported transition types:

- fade
- slide_left
- slide_right
- slide_up
- slide_down

Update flow:

- computes elapsed time using `now_ms`
- updates integer progress
- auto-finishes when duration expires
- switches current screen to target on completion

Draw flow:

- if inactive: delegates to overlay/screen drawing
- if active: draws source or target by phase and overlays a lightweight marker
- avoids full-screen framebuffer blending

## Animation / Performance Notes

- fully non-blocking
- no delay calls
- integer math only
- no dynamic memory
- PIC-friendly low-RAM approach

Low-memory strategy:

- no full-screen offscreen buffer required
- transition visualized with lightweight markers/progress bars
- deterministic execution time

## Dependencies

- `core/*`
- `libraries/ui/screen_manager`
- `libraries/ui/overlay_system`
- `libraries/display/graphics`
- `libraries/ui/sprite_buffer`

## Example

`example.c` demonstrates:

- fade transition
- slide transition
- screen switch animation loop
