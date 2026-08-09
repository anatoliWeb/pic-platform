# tachometer.X

[Ukrainian version](./README.ua.md)

## Purpose

Simulation/debug demo for `libraries/sensors/tachometer`. It feeds synthetic pulse timestamps and
tracks RPM/status without any board-specific tachometer wiring.

## Required Source Files

- `main.c`
- `config_bits.c`
- `project_config.h`
- `../../../../libraries/sensors/tachometer/tachometer.c`

## What It Demonstrates

- expected-running control
- first pulse, second pulse, and RPM calculation
- startup grace and timeout handling
- noise rejection by minimum pulse interval

## Notes

- This is a simulation/debug-only demo.
- No fan, relay, PWM, or heater policy is part of the example.

## How to Build

```cmd
python scripts\build_xc8_project.py examples-projects\xc8\sensors\tachometer.X
```

To build the LIGHTWEIGHT profile, pass `TACHOMETER_LIGHTWEIGHT=1` as a
project-wide compiler define (identical for the example and `tachometer.c`):
```cmd
xc8 -mcpu=18F452 -DTACHOMETER_LIGHTWEIGHT=1 ...\main.c ...\config_bits.c ...\tachometer.c
```

## Status

Ready for XC8 build validation.
