[Ukrainian version](clock-configuration.ua.md)

# Clock Configuration for PIC18 Projects

## Why `main.c` is not enough

`#define PIC_PLATFORM_CLOCK_HZ ...` inside `main.c` is visible only to that
translation unit.

Each `.c` file is compiled separately, so drivers such as `uart.c`,
`delay.c`, and `onewire.c` do not see defines that live only in `main.c`.

## What is a translation unit

A translation unit is one source file after preprocessing.

For example:

- `main.c`
- `core/delay.c`
- `XC8/drivers/communication/uart/uart.c`

Each file gets its own preprocessor pass and its own compile step.

## Correct clock model

Use one project-wide clock macro:

- `PIC_PLATFORM_CLOCK_HZ`

Then let the shared device layer derive:

- `DRV_XTAL_FREQ`
- `_XTAL_FREQ`

This keeps application code, drivers, and delay helpers aligned.

## MPLAB X setup

Set the macro in:

`Project Properties -> XC8 Global Options -> xc8-cc -> Preprocessing and messages -> Preprocessor macros`

Example:

```text
PIC_PLATFORM_CLOCK_HZ=10000000UL
```

That makes the value visible to all translation units in the project.

If you edit the project metadata manually, reopen the project in MPLAB X or
regenerate the project files so the generated makefiles pick up the new macro.

## Example project model

Recommended structure:

- `project_config.h`
  - contains `PIC_PLATFORM_CLOCK_HZ`
- `nbproject/configurations.xml`
  - contains the same macro as a project-wide define
- `core/device.h`
  - derives `DRV_XTAL_FREQ` and `_XTAL_FREQ`
- driver `.c` files
  - use `DRV_XTAL_FREQ` for timing and baud calculations

## Proteus checklist

Make sure these values match:

- PIC Clock Frequency
- external crystal value
- `PIC_PLATFORM_CLOCK_HZ`
- `DRV_XTAL_FREQ`
- `_XTAL_FREQ`

If one of them still points to 8 MHz while the others use 10 MHz, UART and
delay timing will drift.

## UART check

For PIC18F452 at 10 MHz:

- use Virtual Terminal at `9600 baud`
- format: `8N1`
- expected baud generator is about:
  - `SPBRG = 64` in 8-bit high-speed mode
  - `SPBRGH:SPBRG = 259` in 16-bit BRG mode

If the baud generator is still calculated from 8 MHz, the output will look
like garbled characters.

## Bad output checklist

If UART shows random characters, check:

1. `PIC_PLATFORM_CLOCK_HZ` in `project_config.h`
2. `PIC_PLATFORM_CLOCK_HZ` in MPLAB project-wide macros
3. PIC Clock Frequency in Proteus
4. Virtual Terminal baud rate
5. Whether `PIC_PLATFORM_ALLOW_DEFAULT_CLOCK` is still active

## Legacy fallback

Legacy projects may still allow the default 8 MHz clock only when
`PIC_PLATFORM_ALLOW_DEFAULT_CLOCK` is explicitly enabled.

New projects should define `PIC_PLATFORM_CLOCK_HZ` directly.
