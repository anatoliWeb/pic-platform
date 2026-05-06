# PIC Drivers Platform (C18 + XC8)

> [Ukrainian version](README.ua.md)

## Description
PIC Drivers Platform is a reusable embedded driver library for PIC microcontrollers, focused on PIC18.
It provides a portable base for low-level drivers and interfaces.
This repository is a driver platform, not an application.

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
1. Copy required drivers from `drivers/`.
2. Copy `core/` and add include paths.
3. Use C18/XC8-specific sources when needed.
4. Include headers and call `init()`.

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

## Supported Compilers
- MPLAB C18
- MPLAB XC8

## Documentation
- Driver Convention: [docs/driver-convention.md](docs/driver-convention.md) | [UA](docs/driver-convention.ua.md)
- Compiler Notes: [docs/compiler.md](docs/compiler.md) | [UA](docs/compiler.ua.md)
- Naming Rules: [docs/naming.md](docs/naming.md) | [UA](docs/naming.ua.md)
- Examples Guide: [docs/examples.md](docs/examples.md) | [UA](docs/examples.ua.md)
- Porting Guide: [docs/porting.md](docs/porting.md) | [UA](docs/porting.ua.md)
- Build Validation: [docs/build-validation.md](docs/build-validation.md) | [UA](docs/build-validation.ua.md)


