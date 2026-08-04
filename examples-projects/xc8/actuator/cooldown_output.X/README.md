# cooldown_output.X

[Ukrainian version](./README.ua.md)

## Purpose

Simulation/debug demo for `libraries/actuator/cooldown_output`. It drives the helper with synthetic
request changes and timestamps.

## Required Source Files

- `main.c`
- `config_bits.c`
- `project_config.h`
- `../../../../libraries/actuator/cooldown_output/cooldown_output.c`

## What It Demonstrates

- request on and immediate active state
- request off starting cooldown
- re-request canceling shutdown
- remaining time and force-off

## Notes

- This is a simulation/debug-only demo.
- No fan, heater, or tachometer policy is part of the example.

## How to Build

```cmd
python scripts\build_xc8_project.py examples-projects\xc8\actuator\cooldown_output.X
```

## Status

Ready for XC8 build validation.
