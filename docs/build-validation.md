[Ukrainian version](./build-validation.ua.md)

# Build Validation

Toolchains are not available in this environment, so validation must be done in MPLAB manually.

## C18
1. Open project in MPLAB IDE with C18
2. Build basic examples first (`gpio`, `uart`, `adc`)
3. Build protocol drivers after core validation

## XC8
1. Open project in MPLAB X with XC8
2. Verify include paths and MCU target
3. Build `XC8/examples/*_example.c` incrementally

## Typical Issues
- Wrong MCU header
- Wrong oscillator frequency define
- Config bits mismatch
- Interrupt syntax differences


