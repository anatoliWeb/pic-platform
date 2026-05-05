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

## SPI / MSSP Driver

SPI driver uses PIC MSSP module and supports:

- Master mode
- Basic slave mode
- SPI modes 0, 1, 2, 3
- Byte transfer and buffer transfer
- Chip Select helper without hardcoded pin

API highlights:

- `spi_init_master(mode, clock)`
- `spi_init_slave(mode)`
- `spi_transfer_byte(data)`
- `spi_transfer_buffer(tx, rx, len)`
- `spi_cs_init(tris, port, pin)`
- `spi_cs_select()` / `spi_cs_deselect()`

Clock options:

- Fosc/4
- Fosc/16
- Fosc/64
- TMR2/2

Transfer buffer behavior:

- if `tx_buffer == NULL` -> sends `0xFF`
- if `rx_buffer != NULL` -> stores received bytes

### SPI Driver Architecture

- `drivers/spi/spi.c` — universal entry point + fallback
- `C18/drivers/spi/spi.c` — C18-specific implementation
- `XC8/drivers/spi/spi.c` — XC8-specific implementation

### SPI Examples

- `drivers/spi/example.c`:
: sensor register read flow
: external EEPROM/Flash command + address + data flow
- `C18/examples/spi_example.c`
- `XC8/examples/spi_example.c`

## EEPROM Driver

EEPROM driver provides safe internal data memory access:

- byte read/write
- block read/write
- wear-aware update write

Safe write sequence uses interrupt-off + EECON2 unlock (0x55, 0xAA).

## Timer Drivers + Tick

- Timer0, Timer1, Timer2, Timer3 unified APIs
- Overflow callback support
- Millisecond tick via `tick_init/tick_get/tick_delay`

## Notes

- No malloc
- No float
- Drivers are device-agnostic on API level
