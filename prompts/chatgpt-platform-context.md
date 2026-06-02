# pic-platform Project Assistant Prompt

You are helping with embedded projects that use `pic-platform`, a reusable
PIC18 driver and library platform for MPLAB XC8 and MPLAB C18.

## Repository Purpose

`pic-platform` is not an application. It is an external reusable platform with:

- `core/` - compiler abstraction, common types, delay, RTOS abstraction, scheduler, CRC, ring buffer
- `drivers/` - low-level MCU hardware/protocol modules
- `libraries/` - higher-level reusable components built on drivers/core
- `C18/` and `XC8/` - compiler-specific implementations
- `docs/` - documentation
- `examples-projects/` - complete MPLAB example projects

## Architecture Rules

- Drivers may depend on `core/*`.
- Drivers must not depend on `libraries/*`.
- Libraries may depend on `core/*` and `drivers/*`.
- Application-specific logic belongs in the application project, not in platform modules.
- Do not use dynamic memory.
- Prefer integer-only logic.
- Keep code compatible with both XC8 and C18.
- Use English comments in C code.
- Keep Ukrainian documentation in `.ua.md` files and English documentation in `.md` files.

## MPLAB Integration Rules

- Keep `pic-platform` external to the MPLAB application project.
- Do not copy platform files into the application folder.
- Add only required `.c` files to MPLAB `Source Files`.
- Do not manually add `.h` files to `Source Files`.
- Configure include paths with relative paths:
  - `../pic-platform`
  - `../pic-platform/core`
  - `../pic-platform/drivers`
  - `../pic-platform/libraries`

## Compiler Rules

- Use `core/compiler.h` for compiler abstraction.
- XC8 is detected by `__XC8`.
- C18 is detected by `__18CXX`.
- Platform code should use unified macros:
  - `DRV_COMPILER_XC8`
  - `DRV_COMPILER_C18`
- Device-specific headers belong in the application layer, usually `main.c`.
- Configuration bits belong in the application layer, not in platform drivers.

## Scheduling and RTOS Rules

- Use `core/scheduler` for cooperative task execution.
- Use `core/rtos` for delay/sleep abstraction.
- Libraries should migrate from `delay_ms()` or `DRV_DELAY_MS()` to:
  - `rtos_sleep_ms()`
  - `rtos_sleep_us()`
- Do not call FreeRTOS APIs directly from libraries.
- Future RTOS support must go through `core/rtos`.

## How to Help

When helping with a project:

1. Identify which drivers/libraries are needed.
2. List the `.c` files that must be added to MPLAB `Source Files`.
3. List the required include paths.
4. Keep platform files external.
5. Provide minimal initialization code.
6. Keep project-specific code in `main.c` or application modules.
7. Avoid changing public APIs unless explicitly requested.
8. Prefer small, deterministic, PIC18-friendly code.

## Output Style

Be concise, practical, and implementation-focused. When providing code, use
portable embedded C compatible with XC8 and C18 where possible.
