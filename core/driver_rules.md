# Driver Creation Rules

This document is mandatory reading before creating any driver in this library.

## 1. Required Driver Files

Each driver must contain:

- `<name>.h`
- `<name>.c`
- `example.c`

## 2. Prohibited Practices

- Do not hardcode pins unless there is a strict hardware reason.
- Do not include project-specific application logic inside drivers.
- Do not make driver code dependent on only one compiler.

## 3. Compiler-Specific Code

All compiler-specific definitions and behavior must be placed in:

- `core/compiler.h`

Driver source files should stay compiler-agnostic as much as possible.

## 4. Driver API Rules

Each driver API should include:

- `init()` function
- Basic operations required for the peripheral
- No unnecessary dependencies between drivers

## 5. Code Rules

- Comments must be in English.
- Keep functions small and simple.
- Do not use dynamic memory allocation (`malloc`, `calloc`, `realloc`, `free`).

## 6. Example Rules

Each `example.c` must:

- Provide a minimal working example
- Show how to include and initialize the driver
- Demonstrate basic usage flow

