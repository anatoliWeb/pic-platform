# PIC Drivers Platform (C18 + XC8)

> Українська версія: [README.ua.md](README.ua.md)

## Description
PIC Drivers Platform is a reusable embedded driver library for PIC microcontrollers, focused on PIC18.
It provides a consistent driver architecture for low-level peripherals and communication interfaces.
The codebase supports both MPLAB C18 and MPLAB XC8 through a shared abstraction approach.
This repository is a driver platform, not an end-user application.

## Features
- GPIO
- UART / RS485
- ADC
- SPI / I2C
- 1-Wire / DS18B20
- LCD (GPIO + I2C)
- Button / Encoder
- Scheduler
- CRC / Ring Buffer

## Project Structure
```text
/core
/drivers
/C18
/XC8
/examples
/docs
```

## Quick Start
1. Copy required driver folder(s) from `/drivers`.
2. Copy required common files from `/core`.
3. Add C18 or XC8 implementation files from `/C18` or `/XC8`.
4. Include headers and call driver init functions.

## Example
```c
#include "core/compiler.h"
#include "drivers/uart/uart.h"

void main(void)
{
    uart_init(9600);
    uart_write_string("Hello");
}
```

## Documentation
- Driver Convention -> [docs/driver-convention.md](docs/driver-convention.md)
- Compiler Notes -> [docs/compiler.md](docs/compiler.md)
- Naming Rules -> [docs/naming.md](docs/naming.md)
- Examples Guide -> [docs/examples.md](docs/examples.md)
- Porting Guide -> [docs/porting.md](docs/porting.md)
- Build Validation -> [docs/build-validation.md](docs/build-validation.md)

## Design Principles
- no malloc
- non-blocking
- reusable
- portable

## Status
- actively developed
- stable drivers available

## Future
- builder system
- config headers
