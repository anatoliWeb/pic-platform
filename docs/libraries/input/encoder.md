[🇺🇦 Ukrainian version](./encoder.ua.md)

# Encoder Driver

## Description

Quadrature encoder driver with direction/delta detection and integrated button handling.
Supports CW/CCW tracking via periodic `encoder_update()` calls.

## API

- `encoder_init()`
- `encoder_update()`
- `encoder_get_delta()`

## Example

```c
encoder_t enc;
encoder_init(&enc, &PORTB, &TRISB, 4u, 5u, &PORTB, &TRISB, 0u);
encoder_update(&enc);

delta = encoder_get_delta(&enc); // + / - for CW / CCW
button_update(&enc.button);
```

## Notes

- `encoder_get_delta()` reports signed movement steps.
- Button is integrated through existing `button_t` driver state.

## Dependencies

- `core/compiler.h`
- `core/types.h`
- `drivers/button`

