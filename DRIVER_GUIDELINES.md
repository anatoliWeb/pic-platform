# DRIVER_GUIDELINES

> [Ukrainian version](DRIVER_GUIDELINES.ua.md)

This document defines the common driver development standard for `pic-platform`.

## 1. Core Principles

- Drivers must be reusable and portable.
- Both compilers are supported: MPLAB XC8 and MPLAB C18.
- All compiler-specific differences must be isolated in `core/compiler.h`.
- Drivers must not contain project-specific business logic.
- Do not hardcode pins unless passed through explicit configuration/init.
- Do not use dynamic memory (`malloc` / `free`).
- Code comments must be short and in English.

## 2. Required Driver Structure

Each driver in `/drivers/<name>/` must include:

- `<name>.h`
- `<name>.c`
- `example.c`

## 3. Forbidden Patterns

- Changing public API without strong reason.
- Mixing application/business logic into drivers.
- Tight binding to one board unless explicitly parameterized.

## 4. Compiler-Specific Code

All XC8/C18 differences must be centralized in:

- `core/compiler.h`

Driver code must use only unified abstraction macros.

## 5. API Style

Recommended base API:

- `<driver>_init()`
- `<driver>_read()`
- `<driver>_write()`

Add extra functions only when required by the peripheral behavior.

## 6. Naming Rules

- Files: `lower_snake_case` (`uart.h`, `ring_buffer.c`).
- Types: `<driver>_t`.
- Macros/constants: `UPPER_SNAKE_CASE`.
- Public functions: `<driver>_<action>` (`uart_init`, `gpio_toggle`).
- Internal functions in `.c`: `static`.

## 7. example.c Rules

`example.c` must show a minimal working flow:

- proper `#include` lines;
- `init()` call;
- basic API usage in a loop or simple sequence.

Examples are for demonstration only and must not contain product-specific logic.
