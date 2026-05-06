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

## Drivers
- GPIO: [docs/drivers/gpio.md](docs/drivers/gpio.md)
- UART: [docs/drivers/uart.md](docs/drivers/uart.md)
- UART Debug: [docs/drivers/uart_debug.md](docs/drivers/uart_debug.md)
- ADC: [docs/drivers/adc.md](docs/drivers/adc.md)
- SPI: [docs/drivers/spi.md](docs/drivers/spi.md)
- I2C: [docs/drivers/i2c.md](docs/drivers/i2c.md)
- RS485: [docs/drivers/rs485.md](docs/drivers/rs485.md)
- 1-Wire: [docs/drivers/onewire.md](docs/drivers/onewire.md)
- DS18B20: [docs/drivers/ds18b20.md](docs/drivers/ds18b20.md)
- LCD HD44780: [docs/drivers/lcd_hd44780.md](docs/drivers/lcd_hd44780.md)
- Button: [docs/drivers/button.md](docs/drivers/button.md)
- Encoder: [docs/drivers/encoder.md](docs/drivers/encoder.md)
- CRC: [docs/drivers/crc.md](docs/drivers/crc.md)
- Ring Buffer: [docs/drivers/ring_buffer.md](docs/drivers/ring_buffer.md)
- Timer0: [docs/drivers/timer0.md](docs/drivers/timer0.md)
- Timer1: [docs/drivers/timer1.md](docs/drivers/timer1.md)
- Timer2: [docs/drivers/timer2.md](docs/drivers/timer2.md)
- Timer3: [docs/drivers/timer3.md](docs/drivers/timer3.md)
- PWM: [docs/drivers/pwm.md](docs/drivers/pwm.md)
- Comparator: [docs/drivers/comparator.md](docs/drivers/comparator.md)
- CCP Capture: [docs/drivers/ccp_capture.md](docs/drivers/ccp_capture.md)
- CCP Compare: [docs/drivers/ccp_compare.md](docs/drivers/ccp_compare.md)
- External Interrupt: [docs/drivers/ext_interrupt.md](docs/drivers/ext_interrupt.md)
- PORTB Change: [docs/drivers/portb_change.md](docs/drivers/portb_change.md)
- EEPROM: [docs/drivers/eeprom.md](docs/drivers/eeprom.md)
- WDT: [docs/drivers/wdt.md](docs/drivers/wdt.md)
- Reset: [docs/drivers/reset.md](docs/drivers/reset.md)
- Clock: [docs/drivers/clock.md](docs/drivers/clock.md)


