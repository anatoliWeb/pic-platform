# PIC Drivers Platform (C18 + XC8)

> [English version](README.md)

Бібліотека для PIC18 з чітким поділом на низькорівневі драйвери та високорівневі бібліотеки компонентів.
Це не прикладний застосунок.

## Архітектура
- `/core` - абстракція компілятора та спільні утиліти
- `/drivers` - низькорівневий доступ до периферії та протоколів
- `/libraries` - перевикористовувана логіка пристроїв/компонентів поверх драйверів
- `/C18` - реалізації для MPLAB C18
- `/XC8` - реалізації для MPLAB XC8
- `/docs` - документація
- `/examples-projects` - готові MPLAB-проєкти

## Групи драйверів
- `drivers/gpio`
- `drivers/communication/*` (`uart`, `spi`, `i2c`, `onewire`, `rs485`)
- `drivers/analog/*` (`adc`, `comparator`)
- `drivers/timers/*` (`timer0..3`, `pwm`, `ccp_capture`, `ccp_compare`, `timer`, `tick`)
- `drivers/system/*` (`clock`, `wdt`, `reset`)
- `drivers/interrupts/*` (`ext_interrupt`, `portb_change`)
- `drivers/memory/*` (`eeprom`)
- `drivers/utils/*` (`ring_buffer`, `crc`, `scheduler`)

## Групи бібліотек
- `libraries/input/*` (`button`, `encoder`, `adc_buttons`)
- `libraries/display/*` (`lcd_hd44780`)
- `libraries/sensors/*` (`ds18b20`)
- `libraries/system/*` (`uart_debug`)

## Швидкий старт
1. Тримайте `pic-platform` окремо від застосунку.
2. Додавайте в MPLAB тільки потрібні `.c` у `Source Files`.
3. Додайте include paths: `../pic-platform`, `../pic-platform/core`, `../pic-platform/drivers`, `../pic-platform/libraries`.
4. Підключайте заголовки та викликайте `init()` у коді застосунку.

## Приклад
```c
#include "core/compiler.h"
#include "drivers/communication/uart/uart.h"

void main(void)
{
    uart_init(9600);
    uart_write_string("Hello");
}
```

## Документація
- Інтеграція з MPLAB: [docs/mplab-integration.ua.md](docs/mplab-integration.ua.md) | [EN](docs/mplab-integration.md)
- Документація драйверів: [docs/drivers](docs/drivers)
- Документація бібліотек: [docs/libraries](docs/libraries)
- Приклади проєктів: [examples-projects/README.ua.md](examples-projects/README.ua.md) | [EN](examples-projects/README.md)
