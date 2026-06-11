[🇬🇧 English version](./README.md)

# examples-projects

## Що таке examples-projects

`examples-projects` містить повні wrapper-проєкти MPLAB X для зовнішнього
репозиторію `pic-platform`.

- Кожна папка є окремим `.X` проєктом.
- Проєкти потрібні для перевірки інтеграції, а не для зберігання коду бібліотек.
- Вони перевіряють той самий код для MPLAB XC8 і MPLAB C18.
- Приклади розділені за компілятором.

## Структура проєкту

```text
examples-projects/
├── xc8/
│   ├── blink.X/
│   ├── uart_debug.X/
│   ├── adc_read.X/
│   ├── ...
│   └── gpio.X/
│
├── c18/
│   ├── blink.X/
│   ├── uart_debug.X/
│   ├── adc_read.X/
│   ├── ...
│   └── gpio.X/
```

## Пояснення структури папок

- `xc8/` містить wrapper-проєкти для MPLAB XC8.
- `c18/` містить wrapper-проєкти для MPLAB C18.
- Кожна папка `.X` є окремим проєктом MPLAB X.
- Згенеровані папки `build/`, `dist/`, `debug/` та `nbproject/private/`
  не зберігаються в репозиторії.

## Правила проєктів

- Не копіюйте драйвери в папки прикладів.
- Використовуйте лише відносні шляхи.
- Тримайте `pic-platform` зовнішнім для прикладних проєктів.
- Додавайте потрібні `.c` файли вручну через метадані проєкту MPLAB X.
- Тримайте налаштування застосунку в `main.c`, `project_config.h` та
  `config_bits.c`.
- CONFIG bits зберігайте лише в `config_bits.c`.

## Нотатки з інтеграції

- Wrapper-проєкти залежать від `../pic-platform`.
- Include paths мають відкривати доступ до:
  - кореня репозиторію
  - `core/`
  - `drivers/`
  - `libraries/`
- Посилання на source files зберігаються в `nbproject/configurations.xml`.
- Достовірне джерело істини - це метадані MPLAB-проєкту, а не згенерований
  `Makefile-default.mk`.

## Підтримувані компілятори

- MPLAB XC8
- MPLAB C18

## Сімейства прикладних проєктів

| Сімейство | Wrapper-проєкти | Призначення |
|---|---|---|
| Базова перевірка | `blink.X`, `uart_debug.X`, `adc_read.X`, `i2c_scan.X`, `spi_loopback.X`, `rs485_echo.X`, `ds18b20_read.X` | Базові проєкти для інтеграції та перевірки |
| Common wrappers | `delay.X`, `template.X` | Простий стартовий набір |
| GPIO / UART | `gpio.X`, `uart.X` | Базова робота з периферією |
| Аналог / input | `adc.X`, `adc_buttons.X`, `button.X`, `encoder.X` | Приклади для входів і датчиків |
| Дисплеї | `lcd_hd44780.X` | Приклад текстового LCD |
| Core utilities | `ring_buffer.X`, `crc.X`, `clock.X` | Легкі утиліти ядра |
| Системи / таймінг | `comparator.X`, `eeprom.X`, `ext_interrupt.X`, `i2c.X`, `onewire.X`, `portb_change.X`, `pwm.X`, `reset.X`, `rs485_basic.X`, `spi.X`, `tick.X`, `timer.X`, `timer0.X`, `timer1.X`, `timer2.X`, `timer3.X`, `wdt.X` | Приклади системи, таймінгу та зв'язку |
| 1-Wire / sensors | `ds18b20.X`, `ds18b20_multi_read.X` | Окремий wrapper для DS18B20 та приклад для кількох датчиків на одній шині |

Wrapper `ds18b20_multi_read.X` використовує спільну 1-Wire шину з підтягувальним резистором 4.7k до VDD. Для реальної роботи з кількома датчиками потрібні ROM search та Match ROM; перевірка в Proteus ще не завершена.

## Інструкція зі збірки

1. Відкрийте потрібний `.X` проєкт у MPLAB X.
2. Виберіть цільовий MCU та програматор/відлагоджувач.
3. Виберіть потрібну папку toolchain:
   - `xc8/` для MPLAB XC8
   - `c18/` для MPLAB C18
4. Перевірте include paths для кореня репозиторію, `core/`, `drivers/` та
   `libraries/`.
5. Перевірте, що потрібні `.c` файли додані до **Source Files**.
6. Зберіть проєкт у MPLAB X.
7. Окремо додавайте Proteus-схеми, якщо потрібна перевірка на рівні схеми.

## Рекомендований workflow

- Тримайте прикладні проєкти окремо від reuse-стеку.
- Тримайте `pic-platform` спільним і зовнішнім.
- Не патчте драйвери під кожен проєкт; змінюйте лише метадані проєкту.
- Тримайте source references у `nbproject/configurations.xml`.
