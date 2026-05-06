[Ukrainian version](./driver-convention.ua.md)

# Driver Convention

## Purpose
This document defines a common standard for all platform drivers.

## Required Driver Structure
- `drivers/<name>/<name>.h`
- `drivers/<name>/<name>.c`
- `drivers/<name>/example.c`

## API Rules
- Keep API minimal and clear.
- Use naming format: `driver_action()`.
- Put only public declarations in `.h`.

## Restrictions
- No project-specific business logic in drivers.
- No hardcoded pins unless passed through init/config.
- No dynamic memory (`malloc`/`free`).

## Compiler Split
- Shared logic: `drivers/<name>/`
- C18-specific: `C18/drivers/<name>/`
- XC8-specific: `XC8/drivers/<name>/`
- Common abstraction: `core/compiler.h`


