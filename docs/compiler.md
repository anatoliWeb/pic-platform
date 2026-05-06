[Ukrainian version](./compiler.ua.md)

# Compiler Notes

## Supported Toolchains
- MPLAB C18
- MPLAB XC8

## Main Principle
Keep one public API and isolate compiler-specific implementation.

## Locations
- Abstraction macros: `core/compiler.h`
- C18 sources: `C18/drivers/*`
- XC8 sources: `XC8/drivers/*`

## Recommendation
Use universal entrypoint in `drivers/<name>/<name>.c` and route by compiler macros.

## Detection And Override

- Central detection is implemented in `core/compiler.h`.
- Unified macros used by all drivers:
- `DRV_COMPILER_XC8`
- `DRV_COMPILER_C18`

### Detection Priority

1. Built-in toolchain macros:
- `__XC8` (MPLAB XC8)
- `__18CXX` (MPLAB C18)
2. Manual project defines:
- `DRV_COMPILER_XC8`
- `DRV_COMPILER_C18`
3. If no match: compile-time error.

### Manual Override (MPLAB Project Defines)

- Add one define only when automatic detection is unavailable or in special build environments.
- Example (XC8): `DRV_COMPILER_XC8`
- Example (C18): `DRV_COMPILER_C18`
- Never define both at the same time.

### C18 Type Compatibility

- XC8 branch uses:
- `<stdint.h>`
- `<stdbool.h>`
- C18 branch does not use these headers because C18 does not provide full C99 stdint/stdbool support.
- `core/compiler.h` provides manual typedefs for:
- `uint8_t`, `uint16_t`, `uint32_t`
- `int8_t`, `int16_t`, `int32_t`
- `bool`, `true`, `false`

### CONFIG_DECL Guard

- `CONFIG_DECL` is guarded in `core/compiler.h`:
- defined only if it is not already defined.
- This avoids macro redefinition conflicts between toolchains/environments.

### Interrupt Abstraction

- `DRV_INT_DISABLE()` and `DRV_INT_ENABLE()` are defined in `core/compiler.h`.
- `core/interrupts.h` remains a thin compatibility include.

### Important Rules

- Do not switch compiler mode in source code.
- Do not use `__XC8` / `__18CXX` directly in drivers.
- Use only `DRV_COMPILER_*` macros in driver code.
