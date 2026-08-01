# seven_segment

## Purpose

Buffered seven-segment display library for single-digit and multiplexed displays.

## Location

```text
libraries/display/seven_segment/seven_segment.h
libraries/display/seven_segment/seven_segment.c
XC8/libraries/display/seven_segment/seven_segment.c
C18/libraries/display/seven_segment/seven_segment.c
docs/libraries/display/seven_segment.md
docs/libraries/display/seven_segment.ua.md
examples-projects/xc8/seven_segment/basic_manual.X
examples-projects/xc8/seven_segment/multiplex_manual.X
examples-projects/xc8/seven_segment/multiplex_timer.X
```

## Use when

- you need a reusable seven-segment display abstraction;
- you need manual refresh or timer-backed refresh;
- you need number formatting, blink support, and DP control.

## Do not use when

- you only need raw GPIO on segments;
- you need a different display family;
- you already plan to replace the display engine with project-only code.

## Public API

| Function/type | Purpose | What to pass | Returns / output | Notes |
| --- | --- | --- | --- | --- |
| `seven_segment_init()` | initialize display | display instance, config | `uint8_t` success flag | inspect init error on failure |
| `seven_segment_set_digit()` / `set_digit_at()` | set raw digit values | display, digit | none | raw per-digit update |
| `seven_segment_set_hex()` / `set_symbol_at()` / `set_raw()` / `set_raw_at()` | low-level display patterns | display, value/pattern | none | pattern-oriented helpers |
| `seven_segment_set_number()` | show signed integer | display, value | none | compact number rendering |
| `seven_segment_set_number_formatted()` | show formatted integer | display, value, format | `drv_status_t` | use for alignment/width |
| `seven_segment_set_fixed()` | show fixed-point value | display, scaled value, decimals | none | integer math |
| `seven_segment_set_fixed_formatted()` | formatted fixed-point output | display, value, decimals, format | `drv_status_t` | used in examples |
| `seven_segment_set_time_hhmm()` | show HH:MM time | display, start digit, hours, minutes, leading zero flag | `drv_status_t` | time formatting helper |
| `seven_segment_clear()` | clear display buffer | display | none | clears buffer state |
| `seven_segment_set_dot()` / `set_dot_at()` | control decimal point | display, on/off | none | DP support when wired |
| `seven_segment_set_brightness()` | set brightness | display, percent | none | runtime brightness |
| `seven_segment_set_blink_mask()` | select blinking digits | display, mask | none | digit mask |
| `seven_segment_set_blink_period_ms()` | set blink period | display, ms | none | defaults to module constant |
| `seven_segment_set_blink_enabled()` | enable blinking | display, flag | none | toggles blink logic |
| `seven_segment_blink_update()` | advance blink timing | display, now ms | none | manual mode helper |
| `seven_segment_refresh()` | one refresh step | display | none | equivalent to process in manual mode |
| `seven_segment_process()` | one refresh step | display | none | application loop call in manual mode |
| `seven_segment_is_timer_running()` | timer ownership status | display | `uint8_t` | diagnostics |
| `seven_segment_get_refresh_count()` | refresh counter | display | `uint32_t` | diagnostics |
| `seven_segment_get_init_error()` | init error detail | display | `seven_segment_init_error_t` | use after init failure |
| `seven_segment_stop()` | stop refresh | display | none | halts display activity |
| `seven_segment_deinit()` | deinitialize | display | none | cleanup path |
| `seven_segment_irq_handler()` | timer interrupt dispatcher | nothing | none | required in timer mode |

## Required files

### Core/library files

```text
libraries/display/seven_segment/seven_segment.h
libraries/display/seven_segment/seven_segment.c
```

### XC8 wrapper files

```text
XC8/libraries/display/seven_segment/seven_segment.c
```

### C18 wrapper files

```text
C18/libraries/display/seven_segment/seven_segment.c
```

## Dependencies

```text
drivers/gpio/gpio.h
drivers/timers/timer0/timer0.h
drivers/timers/timer1/timer1.h
drivers/timers/timer2/timer2.h
drivers/timers/timer3/timer3.h
core/types.h
core/pic_platform_config.h
```

## Configuration defines

| Define | Default | Where | Effect | Code size impact |
| --- | ---: | --- | --- | --- |
| `SEVEN_SEGMENT_REFRESH_MANUAL` | n/a | header enum | application owns refresh | none |
| `SEVEN_SEGMENT_REFRESH_TIMER` | n/a | header enum | library owns refresh via timer | adds timer backend code |
| `SEVEN_SEGMENT_TIMER0..3` | n/a | header enum | selects timer backend | backend-specific |
| `SEVEN_SEGMENT_ENABLE_TIMER0..3` | `0` | `core/pic_platform_config.h` / build defines | enables backend compilation | backend-specific |

## Runtime model

- Manual mode: application calls `seven_segment_process()` or `seven_segment_refresh()`.
- Timer mode: the selected timer backend owns refresh and the application must forward interrupts to `seven_segment_irq_handler()`.
- `Timer1` cannot be shared with `tick` in the same project.

## Integration recipe

1. Add the core library files and the matching wrapper file for the compiler.
2. Configure `project_config.h` and `config_bits.c`.
3. Bind segment and digit pins in project code.
4. Select manual mode or a timer backend.
5. If timer mode is used, call `seven_segment_irq_handler()` from the global ISR.
6. Build a targeted example first.

## Minimal usage example

```c
seven_segment_t display;

seven_segment_init(&display, &config);
seven_segment_set_number(&display, 1234);

while (1)
{
    seven_segment_process(&display);
}
```

## Resource ownership

- Segment pins are project-bound through the config.
- Digit pins are project-bound through the config.
- Timer0/1/2/3 ownership depends on backend selection.
- DP support is available when the dot pin is configured.

## Examples

```text
examples-projects/xc8/seven_segment/basic_manual.X
examples-projects/xc8/seven_segment/multiplex_manual.X
examples-projects/xc8/seven_segment/multiplex_timer.X
```

## Human documentation

```text
docs/libraries/display/seven_segment.md
docs/libraries/display/seven_segment.ua.md
```

## Known limitations

- One timer backend can own one display instance at a time.

## Extension points

- Extend formatting helpers or diagnostics in the library, not in a project.

## AI decision rule

When a task needs a seven-segment display, use or extend this library instead of creating a project-local display engine.
