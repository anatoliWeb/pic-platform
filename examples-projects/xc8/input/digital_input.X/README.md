# digital_input.X

[Ukrainian version](./README.ua.md)

## Purpose

Simulation/debug demo for `libraries/input/digital_input`. It feeds synthetic raw levels and
timestamps to show debounce, edges, and latch behavior.

## Required Source Files

- `main.c`
- `config_bits.c`
- `project_config.h`
- `../../../../libraries/input/digital_input/digital_input.c`

## What It Demonstrates

- active-high and active-low mapping
- debounce and stable state transitions
- single rise / fall events
- optional latch mode

## Notes

- This is a simulation/debug-only demo.
- `digital_input_rose()` and `digital_input_fell()` consume their event flags.

## How to Build

```cmd
python scripts\build_xc8_project.py examples-projects\xc8\input\digital_input.X
```

## Status

Ready for XC8 build validation.
