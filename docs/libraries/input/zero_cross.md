[Ukrainian version](./zero_cross.ua.md)

# Zero-Cross Detector

## Description

Reusable AC zero-cross edge detector with glitch rejection, half-cycle measurement, frequency detection (50/60 Hz), timeout, and recovery. It owns no pin, no timer, and no output. The caller feeds edges with a monotonic microsecond time and dispatches the produced event to any number of consumers.

## API

- `zero_cross_init(zc, config)`
- `zero_cross_on_edge(zc, now_us, event)`
- `zero_cross_process(zc, now_us)`
- `zero_cross_is_alive(zc)`
- `zero_cross_get_status(zc)`
- `zero_cross_get_frequency(zc)`
- `zero_cross_get_half_cycle_us(zc)`
- `zero_cross_get_sequence(zc)`
- `zero_cross_reset(zc)`

## Example

```c
zero_cross_config_t cfg = {
    .timeout_ms = 300u,
    .min_half_cycle_us = 8000u,
    .max_half_cycle_us = 12000u,
    .glitch_reject_us = 500u,
    .recovery_event_count = 2u,
};
zero_cross_t zc;
zero_cross_init(&zc, &cfg);

/* In the zero-cross ISR: */
zero_cross_event_t event;
if (zero_cross_on_edge(&zc, now_us(), &event))
{
    dispatch_to_consumers(&event);
}
```

## Notes

- The library never owns a clock; the caller passes monotonic microseconds in `now_us`.
- The first edge only arms the detector; a valid half-cycle needs two edges.
- A valid edge produces one event and marks the detector `ALIVE`.
- Timeout is advanced from the main loop via `zero_cross_process()`.
- `ac_phase_control` uses `zero_cross` as its shared sync domain.
- Feature blocks (timeout, frequency detection, glitch filter) can be cut with `ZERO_CROSS_DISABLE_*` defines.

## Dependencies

- `core/types.h`
- `core/compiler.h`

## Related Examples

- `libraries/input/zero_cross/example.c`
- `libraries/output/ac_phase_control/example.c`
