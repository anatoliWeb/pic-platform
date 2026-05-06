# PIC Drivers Platform (C18 + XC8)

> [English version](README.md)

## Опис
PIC Drivers Platform — це бібліотека повторно використовуваних embedded-драйверів для PIC мікроконтролерів (фокус на PIC18).
Вона надає портативну базу для низькорівневих драйверів та інтерфейсів.
Це драйверна платформа, а не application.

## Можливості
- GPIO
- UART / RS485
- ADC
- SPI / I2C
- 1-Wire / DS18B20
- LCD (GPIO + I2C)
- Button / Encoder
- Scheduler
- CRC / Ring Buffer

## Структура Проєкту
```text
/core
/drivers
/C18
/XC8
/examples
/docs
```

## Швидкий Старт
1. Скопіювати потрібні драйвери з `drivers/`.
2. Додати `core/` та include paths у проєкт.
3. Підключити C18/XC8-реалізації за потреби.
4. Підключити заголовки та викликати `init()`.

## Приклад
```c
#include "core/compiler.h"
#include "drivers/uart/uart.h"

void main(void)
{
    uart_init(9600);
    uart_write_string("Hello");
}
```

## Підтримувані Компілятори
- MPLAB C18
- MPLAB XC8

## Документація
- Конвенція драйверів: [docs/driver-convention.md](docs/driver-convention.md) | [UA](docs/driver-convention.ua.md)
- Нотатки про компілятори: [docs/compiler.md](docs/compiler.md) | [UA](docs/compiler.ua.md)
- Правила іменування: [docs/naming.md](docs/naming.md) | [UA](docs/naming.ua.md)
- Гайд прикладів: [docs/examples.md](docs/examples.md) | [UA](docs/examples.ua.md)
- Гайд портування: [docs/porting.md](docs/porting.md) | [UA](docs/porting.ua.md)
- Валідація збірки: [docs/build-validation.md](docs/build-validation.md) | [UA](docs/build-validation.ua.md)

## Драйвери
- GPIO: [docs/drivers/gpio.md](docs/drivers/gpio.ua.md)
- UART: [docs/drivers/uart.md](docs/drivers/uart.ua.md)
- UART Debug: [docs/drivers/uart_debug.md](docs/drivers/uart_debug.ua.md)
- ADC: [docs/drivers/adc.md](docs/drivers/adc.ua.md)
- SPI: [docs/drivers/spi.md](docs/drivers/spi.ua.md)
- I2C: [docs/drivers/i2c.md](docs/drivers/i2c.ua.md)
- RS485: [docs/drivers/rs485.md](docs/drivers/rs485.ua.md)
- 1-Wire: [docs/drivers/onewire.md](docs/drivers/onewire.ua.md)
- DS18B20: [docs/drivers/ds18b20.md](docs/drivers/ds18b20.ua.md)
- LCD HD44780: [docs/drivers/lcd_hd44780.md](docs/drivers/lcd_hd44780.ua.md)
- Button: [docs/drivers/button.md](docs/drivers/button.ua.md)
- Encoder: [docs/drivers/encoder.md](docs/drivers/encoder.ua.md)
- CRC: [docs/drivers/crc.md](docs/drivers/crc.ua.md)
- Ring Buffer: [docs/drivers/ring_buffer.md](docs/drivers/ring_buffer.ua.md)
- Timer0: [docs/drivers/timer0.md](docs/drivers/timer0.ua.md)
- Timer1: [docs/drivers/timer1.md](docs/drivers/timer1.ua.md)
- Timer2: [docs/drivers/timer2.md](docs/drivers/timer2.ua.md)
- Timer3: [docs/drivers/timer3.md](docs/drivers/timer3.ua.md)
- PWM: [docs/drivers/pwm.md](docs/drivers/pwm.ua.md)
- Comparator: [docs/drivers/comparator.md](docs/drivers/comparator.ua.md)
- CCP Capture: [docs/drivers/ccp_capture.md](docs/drivers/ccp_capture.ua.md)
- CCP Compare: [docs/drivers/ccp_compare.md](docs/drivers/ccp_compare.ua.md)
- External Interrupt: [docs/drivers/ext_interrupt.md](docs/drivers/ext_interrupt.ua.md)
- PORTB Change: [docs/drivers/portb_change.md](docs/drivers/portb_change.ua.md)
- EEPROM: [docs/drivers/eeprom.md](docs/drivers/eeprom.ua.md)
- WDT: [docs/drivers/wdt.md](docs/drivers/wdt.ua.md)
- Reset: [docs/drivers/reset.md](docs/drivers/reset.ua.md)
- Clock: [docs/drivers/clock.md](docs/drivers/clock.ua.md)

## Приклади Проєктів MPLAB
- Українська: [examples-projects/README.ua.md](examples-projects/README.ua.md)
- English: [examples-projects/README.md](examples-projects/README.md)
