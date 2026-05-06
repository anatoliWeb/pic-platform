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


