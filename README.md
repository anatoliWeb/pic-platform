# PIC Drivers Platform (C18 + XC8)

> [Ukrainian version](README.ua.md)

Reusable PIC18 embedded platform with strict layer separation and portable architecture.
This repository is a driver/library platform, not an application.

## Architecture
- `/core` - compiler abstraction and software utilities
- `/drivers` - low-level hardware/protocol modules
- `/libraries` - high-level reusable components
- `/C18` - compiler-specific sources for MPLAB C18
- `/XC8` - compiler-specific sources for MPLAB XC8
- `/docs` - documentation
- `/examples-projects` - complete MPLAB example projects
- `/prompts` - reusable AI prompt context files

## Core Layer
- `core/compiler.h`, `core/types.h`, `core/bit_utils.h`, `core/delay.*`
- `core/crc/*`, `core/ring_buffer/*`, `core/scheduler/*`
- `core/rtos/*` for optional RTOS abstraction

## RTOS Abstraction (Initial)
- No FreeRTOS dependency is added.
- Build switch: `DRV_USE_FREERTOS`
- `rtos_delay_ms()` works in bare-metal mode via `delay_ms()`.
- Mutex/queue APIs exist as placeholders for future backend mapping.
- Libraries should use `core/rtos` APIs only (never RTOS-native calls directly).

## Quick Start
1. Keep `pic-platform` external to your app project.
2. Add required `.c` files into MPLAB `Source Files`.
3. Configure include paths:
   - `../pic-platform`
   - `../pic-platform/core`
   - `../pic-platform/drivers`
   - `../pic-platform/libraries`
4. Include headers and call init APIs from application code.

## Documentation
- Architecture: [docs/architecture.md](docs/architecture.md) | [UA](docs/architecture.ua.md)
- MPLAB integration: [docs/mplab-integration.md](docs/mplab-integration.md) | [UA](docs/mplab-integration.ua.md)
- PIC18F452 configuration: [docs/configuration/pic18f452-config-bits.md](docs/configuration/pic18f452-config-bits.md) | [UA](docs/configuration/pic18f452-config-bits.ua.md)
- Examples projects: [examples-projects/README.md](examples-projects/README.md) | [UA](examples-projects/README.ua.md)
- Prompt kit: [docs/prompts.md](docs/prompts.md) | [UA](docs/prompts.ua.md)
- Phase 1 examples audit: [docs/audits/phase-1-examples-validation.md](docs/audits/phase-1-examples-validation.md) | [UA](docs/audits/phase-1-examples-validation.ua.md)

## Validated Example Status
- OneWire and DS18B20 are working for PIC18F452 + XC8 + Proteus + 10 MHz.
- Validated flows: SEARCH_ROM, MATCH_ROM temperature read, SKIP_ROM temperature read, CRC validation.
- Validated Proteus timing preset uses real microseconds for reset/read and raw tuning ticks for write slots.
- Reference example: `examples-projects/xc8/ds18b20.X` (lean temperature example)
- Diagnostic examples: `examples-projects/xc8/onewire_bus_test.X` and `examples-projects/xc8/ds18b20_search_rom.X`
- Reference docs: `docs/libraries/sensors/ds18b20.md` and `docs/libraries/sensors/ds18b20.ua.md`
