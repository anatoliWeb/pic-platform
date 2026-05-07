# PIC Drivers Platform (C18 + XC8)

> [Ukrainian version](README.ua.md)

Reusable PIC18 embedded platform with clear separation between low-level drivers and high-level libraries.
This repository is not an application.

## Architecture
- `/core` - compiler abstraction and common utilities
- `/drivers` - low-level hardware/protocol modules
- `/libraries` - reusable high-level components built on drivers
- `/C18` - C18 compiler-specific implementations
- `/XC8` - XC8 compiler-specific implementations
- `/docs` - documentation
- `/examples-projects` - complete MPLAB projects

## Driver Groups
- `drivers/gpio`
- `drivers/communication/*` (`uart`, `spi`, `i2c`, `onewire`, `rs485`)
- `drivers/analog/*` (`adc`, `comparator`)
- `drivers/timers/*` (`timer0..3`, `pwm`, `ccp_capture`, `ccp_compare`, `timer`, `tick`)
- `drivers/system/*` (`clock`, `wdt`, `reset`)
- `drivers/interrupts/*` (`ext_interrupt`, `portb_change`)
- `drivers/memory/*` (`eeprom`)
- `core/*` software utilities (`ring_buffer`, `crc`, `scheduler`)

## Library Groups
- `libraries/input/*` (`button`, `encoder`, `adc_buttons`)
- `libraries/display/*` (`lcd_hd44780`)
- `libraries/sensors/*` (`ds18b20`)
- `libraries/system/*` (`uart_debug`)

## Quick Start
1. Keep `pic-platform` external to your application project.
2. Add only required `.c` files into MPLAB `Source Files`.
3. Configure include paths: `../pic-platform`, `../pic-platform/core`, `../pic-platform/drivers`, `../pic-platform/libraries`.
4. Include headers and call init APIs from application code.

## Example
```c
#include "core/compiler.h"
#include "drivers/communication/uart/uart.h"

void main(void)
{
    uart_init(9600);
    uart_write_string("Hello");
}
```

## Documentation
- MPLAB integration: [docs/mplab-integration.md](docs/mplab-integration.md) | [UA](docs/mplab-integration.ua.md)
- Drivers docs: [docs/drivers](docs/drivers)
- Libraries docs: [docs/libraries](docs/libraries)
- Examples projects: [examples-projects/README.md](examples-projects/README.md) | [UA](examples-projects/README.ua.md)

## Architecture Separation
- `core/` for portable software utilities and abstraction
- `drivers/` for low-level hardware/protocol access
- `libraries/` for high-level reusable component logic

## RTOS Readiness
- Optional abstraction in `core/rtos/` for future FreeRTOS support
- Target API surface includes wrappers like `rtos_delay_ms()`, `rtos_mutex_lock()`, `rtos_queue_send()`
- Current code remains bare-metal compatible

## Architecture Doc
- [docs/architecture.md](docs/architecture.md)