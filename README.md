# PIC PLATFORM - Universal Drivers (C18 + XC8)

Universal PIC drivers base (focus: PIC18).
This repository is a reusable drivers library, not an application project.

## Supported Compilers

- MPLAB C18
- MPLAB XC8

## Core Idea

- One shared API per driver
- Universal entry file in `drivers/`
- Compiler-specific implementations in `C18/` and `XC8/`
- Fallback implementation when no specific override is selected

## Timer Drivers

Timer support includes Timer0, Timer1, Timer2, Timer3 with unified API:

- `init/start/stop/set/get`
- interrupt enable/disable
- overflow callback
- `timerX_irq_handler()` for ISR integration

### Tick System

- `tick_init()`
- `tick_get()`
- `tick_delay(ms)`

Tick is millisecond-based and integer-only.

## EEPROM Driver

EEPROM driver provides safe internal data memory access:

- byte read/write
- block read/write
- wear-aware update write

API:

- `eeprom_init()`
- `eeprom_read_byte(address)`
- `eeprom_write_byte(address, value)`
- `eeprom_update_byte(address, value)`
- `eeprom_read_block(address, buffer, length)`
- `eeprom_write_block(address, data, length)`

### Safe Write Sequence

Write flow follows required unlock sequence:

1. Disable global interrupts
2. `EECON2 = 0x55`
3. `EECON2 = 0xAA`
4. Set `WR` bit
5. Wait until write completes
6. Restore global interrupts

### Wear Recommendations

- Prefer `eeprom_update_byte()` over direct write
- Do not write unchanged values
- Minimize frequent writes in fast loops

## Examples

- `drivers/eeprom/example.c` - save/restore setting + block operations
- `C18/examples/eeprom_example.c` - byte write/read with UART debug output
- `XC8/examples/eeprom_example.c` - byte write/read with UART debug output
