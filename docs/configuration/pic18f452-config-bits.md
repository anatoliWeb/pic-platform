[Ukrainian version](pic18f452-config-bits.ua.md)

# PIC18F452 Configuration Bits

## Overview

`pic-platform` keeps project configuration at the application layer.
Libraries must not emit `#pragma config`.

The project controls two related things:

- CONFIG bits, through `config_bits.c`
- clock frequency, through `project_config.h`

## Files

- `core/config/project_config_template.h`
- `core/config/platform_config_check.h`
- `core/config/library_requirements.md`
- `core/config/example.c`
- `templates/config/pic18f452/xc8/config_bits.c`
- `templates/config/pic18f452/c18/config_bits.c`
- `examples-projects/xc8/blink.X/project_config.h`
- `examples-projects/xc8/blink.X/config_bits.c`
- `examples-projects/c18/blink.X/project_config.h`
- `examples-projects/c18/blink.X/config_bits.c`

## Frequency Macros

- `PIC_PLATFORM_CLOCK_HZ` is the project clock in Hz.
- `DRV_XTAL_FREQ` is the platform clock value used by drivers.
- `_XTAL_FREQ` is the compiler-facing clock value used by delay and timing code.
- `PIC_PLATFORM_WDT_ENABLED`, `PIC_PLATFORM_LVP_ENABLED`, and
  `PIC_PLATFORM_BOR_ENABLED` are simple helper flags for checks and notes.

For PIC18 projects, these values should stay aligned.

## Recommended PIC18F452 Defaults

Use literal CONFIG values in `config_bits.c`:

- `OSC = HS`
- `OSCS = OFF`
- `PWRT = OFF`
- `BOR = ON`
- `BORV = 20`
- `WDT = OFF`
- `WDTPS = 128`
- `CCP2MUX = ON`
- `STVR = ON`
- `LVP = OFF`
- code protection and write protection bits OFF

## CONFIG Bit Flow

1. `project_config.h` stores project-level values.
2. `platform_config_check.h` checks that required values exist.
3. `config_bits.c` emits the real `#pragma config` lines with literal values.
4. The application has final priority over library recommendations.

## Library Requirements Convention

Libraries may declare requirements or recommendations such as:

- `PIC_PLATFORM_REQUIRES_CLOCK_HZ`
- `PIC_PLATFORM_REQUIRES_WDT_ENABLED`
- `PIC_PLATFORM_RECOMMENDS_WDT_ON`
- `PIC_PLATFORM_REQUIRES_CCP2MUX_ENABLED`

Libraries must not emit `#pragma config`.

## Compiler Notes

- XC8 and C18 use separate `config_bits.c` templates.
- XC8 macro-based `#pragma config` values were not reliable on PIC18F452.
- Use literal CONFIG values in `config_bits.c` for both compilers.
- `project_config.h` remains for clock, timing, and helper flags.

## Relation to `TODO.md`

This layer is considered implemented when the project templates, checker, docs,
and example-project integration are present.
