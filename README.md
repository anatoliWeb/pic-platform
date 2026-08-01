# PIC Drivers Platform (C18 + XC8)

[Ukrainian version](./README.ua.md)

Reusable PIC18 platform with clear separation between `core`, `drivers`, and `libraries`.

## Architecture

- `core/` - compiler abstraction, delays, types, CRC, ring buffer, scheduler, RTOS wrapper
- `drivers/` - low-level MCU peripherals and protocol drivers
- `libraries/` - reusable higher-level components built on top of drivers and core
- `C18/` - compiler-specific MPLAB C18 sources
- `XC8/` - compiler-specific MPLAB XC8 sources
- `docs/` - documentation
- `examples-projects/` - MPLAB example wrapper projects
- `prompts/` - reusable AI prompt context files

## Quick Start

1. Keep `pic-platform` external to the application project.
2. Add only required `.c` files to MPLAB `Source Files`.
3. Configure include paths:
   - `../pic-platform`
   - `../pic-platform/core`
   - `../pic-platform/drivers`
   - `../pic-platform/libraries`
4. Keep configuration bits in `config_bits.c`.
5. Use wrapper examples from `examples-projects/` as integration references.

## Key Documentation

- Architecture: [docs/architecture.md](docs/architecture.md)
- Dependency graphs: [docs/architecture/dependency-graph.md](docs/architecture/dependency-graph.md)
- Example matrix: [docs/architecture/example-matrix.md](docs/architecture/example-matrix.md)
- Module inventory: [docs/architecture/module-inventory.md](docs/architecture/module-inventory.md)
- Generation workflow: [docs/architecture/generation-workflow.md](docs/architecture/generation-workflow.md)
- MPLAB integration: [docs/mplab-integration.md](docs/mplab-integration.md)
- Console build: [docs/build/console-build.md](docs/build/console-build.md)
- Examples projects: [examples-projects/README.md](examples-projects/README.md)
- Seven segment library: [docs/libraries/display/seven_segment.md](docs/libraries/display/seven_segment.md)
- Segment keys library: [docs/libraries/input/segment_keys.md](docs/libraries/input/segment_keys.md)
- Prompt kit: [docs/prompts.md](docs/prompts.md)

## Validation Snapshot

- OneWire and DS18B20 are validated for PIC18F452 + XC8 + Proteus + 10 MHz.
- Input examples `button.X`, `encoder.X`, and `adc_buttons.X` are verified in Proteus.
- `software_pwm.X` is verified in Proteus.
- Grouped seven-segment examples under `examples-projects/xc8/seven_segment/` are implemented and ready for Proteus validation, including buffered fixed-point formatting in `multiplex_manual.X` and library-owned Timer2 refresh in `multiplex_timer.X`: `basic_manual.X`, `multiplex_manual.X`, `multiplex_timer.X`, `keys_single_line.X`, and `keys_diode_coded.X`.
