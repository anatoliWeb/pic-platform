# seven_segment example family

## Purpose

Example family for manual and timer-backed seven-segment display integration.

## Where to look

```text
examples-projects/xc8/seven_segment/basic_manual.X
examples-projects/xc8/seven_segment/multiplex_manual.X
examples-projects/xc8/seven_segment/multiplex_timer.X
examples-projects/xc8/seven_segment/keys_single_line.X
examples-projects/xc8/seven_segment/keys_diode_coded.X
```

## Integration notes

- `basic_manual.X` shows single-digit/manual refresh.
- `multiplex_manual.X` shows manual multiplexing.
- `multiplex_timer.X` shows timer-owned refresh.
- `keys_single_line.X` and `keys_diode_coded.X` show shared-line key decoding.

## Files usually added

```text
main.c
config_bits.c
project_config.h
core/delay.c
drivers/gpio/gpio.c
libraries/display/seven_segment/seven_segment.c
```

## Important conflicts

- manual mode: application owns refresh;
- timer mode: selected timer backend owns refresh;
- Timer1 is already owned by `tick` in key examples.
