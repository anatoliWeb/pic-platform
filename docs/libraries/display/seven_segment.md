# seven_segment

[Ukrainian version](./seven_segment.ua.md)

## Description

Buffered seven-segment display library for single-digit and multiplexed displays.
The library supports two refresh ownership models:

- `SEVEN_SEGMENT_REFRESH_MANUAL`
- `SEVEN_SEGMENT_REFRESH_TIMER`

Manual mode keeps refresh in the application loop.
Timer mode moves refresh ownership into the library and binds one display instance to one hardware timer backend.

## API

- `seven_segment_init()`
- `seven_segment_refresh()`
- `seven_segment_process()`
- `seven_segment_stop()`
- `seven_segment_deinit()`
- `seven_segment_irq_handler()`
- `seven_segment_is_timer_running()`
- `seven_segment_get_refresh_count()`
- `seven_segment_set_number()`
- `seven_segment_set_number_formatted()`
- `seven_segment_set_fixed()`
- `seven_segment_set_fixed_formatted()`
- `seven_segment_set_time_hhmm()`
- `seven_segment_set_brightness()`
- `seven_segment_set_blink_mask()`
- `seven_segment_set_blink_period_ms()`
- `seven_segment_set_blink_enabled()`

## Refresh Ownership

### Manual Mode

Use manual mode when the application owns the multiplex step:

```c
config.refresh_mode = SEVEN_SEGMENT_REFRESH_MANUAL;
config.timer = SEVEN_SEGMENT_TIMER_NONE;
config.refresh_period_us = 0U;

seven_segment_init(&display, &config);

while (1)
{
    seven_segment_process(&display);
    seven_segment_blink_update(&display, now_ms);
}
```

Behavior:

- `seven_segment_process()` performs one real refresh step.
- `seven_segment_refresh()` is equivalent to `seven_segment_process()`.
- blink timing advances only from `seven_segment_blink_update()` in main context.

### Timer Mode

Use timer mode when the library must own refresh:

```c
config.refresh_mode = SEVEN_SEGMENT_REFRESH_TIMER;
config.timer = SEVEN_SEGMENT_TIMER2;
config.refresh_period_us = 1000U;

seven_segment_init(&display, &config);

INTCONbits.PEIE = 1U;
INTCONbits.GIE = 1U;

while (1)
{
    /* No manual display refresh call is required here. */
}
```

Behavior:

- the library configures the selected timer backend;
- the library registers its internal timer callback;
- the timer callback calls the private refresh core directly;
- `seven_segment_process()` and `seven_segment_refresh()` become safe no-ops;
- blink timing advances automatically inside the timer-owned path.

## Timer Backend Selection

Supported runtime selections:

- `SEVEN_SEGMENT_TIMER0`
- `SEVEN_SEGMENT_TIMER1`
- `SEVEN_SEGMENT_TIMER2`
- `SEVEN_SEGMENT_TIMER3`

Current limitation:

- each timer driver exposes only one callback slot;
- one timer backend can own only one `seven_segment_t` instance at a time;
- `seven_segment_init()` fails if the selected timer is already occupied by another callback owner.

## Project Configuration Visibility

The timer backend selection macros (`SEVEN_SEGMENT_ENABLE_TIMER0..3`) are consumed
by `seven_segment.c`, which is compiled separately. Set them project-wide through
compiler `-D` or MPLAB `define-macros`, identical in every translation unit.

For MPLAB example projects, keep the project root, `core`, `drivers`, and
`libraries` on the include path, for example:

```text
.;../../../../;../../../../core;../../../../drivers;../../../../libraries
```

## Interrupt Requirements

The library does not install the global MCU ISR for you.
The application must still forward interrupts to the library dispatcher:

```c
void __interrupt() isr(void)
{
    seven_segment_irq_handler();
}
```

Application responsibilities in timer mode:

- provide the global ISR;
- enable `PEIE` and `GIE` when initialization is complete.

## Diagnostics

For timer-owned validation:

- `seven_segment_is_timer_running()` reports whether the selected timer slot is currently owned by the display instance;
- `seven_segment_get_refresh_count()` returns a read-only refresh counter updated from the timer callback after a real refresh step.

These helpers are intended for diagnostics and Proteus bring-up, not for the main display logic.

## Notes

- buffer updates are still protected against mid-refresh tearing;
- timer-owned shared-line key queueing is still a separate pending integration step;
- timer mode should be validated with a minimal constant-content example before returning to more complex scenes.
