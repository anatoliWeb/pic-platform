[🇺🇦 Ukrainian version](./encoder.ua.md)

# Encoder Driver

## Description

Quadrature encoder driver with direction, delta, and absolute position tracking.
The encoder button is integrated through the existing `button_t` driver.
This version is polling-based and does not require interrupts.

## API

- `encoder_init()`
- `encoder_tick()`
- `encoder_update()` - compatibility wrapper
- `encoder_get_delta()`
- `encoder_get_position()`
- `encoder_set_position()`
- `encoder_clicked()`
- `encoder_double_clicked()`
- `encoder_held()`
- `encoder_get_click_count()`

## Example

```c
encoder_t enc;
encoder_init(&enc, &PORTB, &TRISB, 0u, 1u, &PORTB, &TRISB, 2u);

while (1)
{
    encoder_tick(&enc);

    if (encoder_get_delta(&enc) > 0) { /* CW step */ }
    if (encoder_get_delta(&enc) < 0) { /* CCW step */ }
    if (encoder_clicked(&enc)) { /* click action */ }
    if (encoder_double_clicked(&enc)) { /* double click action */ }
    if (encoder_held(&enc)) { /* hold action */ }
}
```

## Notes

- `encoder_get_delta()` keeps the old compatibility behavior and drains one step at a time.
- `encoder_get_position()` returns the accumulated signed position counter.
- The button is handled by the embedded `button_t` state machine.
- Polling every 1-5 ms is recommended.

## Dependencies

- `core/compiler.h`
- `core/types.h`
- `drivers/timers/tick`
- `libraries/input/button`
