# Аудит валідації прикладів Phase 1

## 1. Підсумок

Приклади Phase 1 переважно перевірені для PIC18F452 + XC8 + Proteus на частоті 10 MHz.
Цей audit фіксує, що вже підтверджено, що відкладено, і що переходить у Phase 2.

## 2. Оточення

- MCU: PIC18F452 DIP-40
- Package: DIP-40
- Clock: 10 MHz
- Compiler: XC8
- Simulator: Proteus
- UART terminal: Virtual Terminal, 9600 baud, 8N1

## 3. Перевірені приклади

| Приклад | Область | Що перевірено | Примітки Proteus | Статус |
|---|---|---|---|---|
| `blink` | GPIO | Базове миготіння LED | Простий GPIO-ланцюжок світлодіода | Перевірено в Proteus |
| `uart` | UART | Тест echo | RC6/TX до Virtual Terminal RXD, RC7/RX необов’язково | Перевірено в Proteus |
| `uart_debug` | UART debug | Допоміжні повідомлення Text, HEX, INT, OK/FAIL | Перевірено вивід debug і alive counter | Перевірено в Proteus |
| `adc_read` | ADC | Базове читання ADC | Джерело аналогового сигналу на AN0/AN1 | Перевірено в Proteus |
| `button` | Input | Active-low debounce кнопки | Підтяжка, стабільні події press/release | Перевірено в Proteus |
| `encoder` | Input | Обертання, delta, position, події кнопки | Підключення A/B/SW з pull-up | Перевірено в Proteus |
| `adc_buttons` | Input | Кнопки через resistor ladder на ADC | Один ADC input і кілька кнопок | Перевірено в Proteus |
| `ring_buffer` | Core utility | Push/pop/full/empty/wrap-around | UART output підтверджує FIFO-поведінку | Перевірено в Proteus |
| `rs485_basic` | RS485 | Передача frame | MAX487 і керування DE/RE | Перевірено в Proteus |
| `rs485_echo` | RS485 | Half-duplex echo | Два вузли MAX487 повертають чистий echo | Перевірено в Proteus |
| `spi` | SPI | Базовий периферійний вихід | 74HC595 і LED pattern | Перевірено в Proteus |
| `spi_loopback` | SPI | Loopback self-test | RC5/SDO до RC4/SDI | Перевірено в Proteus |
| `tick` | Timing | Джерело мілісекундного tick | Потрібен forwarding через Timer1 ISR | Перевірено в Proteus |
| `timer0` | Timer | Лічильник переривань | Binary output на RB0..RB3 | Перевірено в Proteus |
| `timer1` | Timer | Лічильник переривань | Binary output на RB0..RB3 | Перевірено в Proteus |
| `timer2` | Timer | Лічильник переривань | Binary output на RB0..RB3 | Перевірено в Proteus |
| `timer3` | Timer | Лічильник переривань | Binary output на RB0..RB3 | Перевірено в Proteus |
| `wdt` | System safety | Поведінка watchdog reset | У normal mode WDT очищується, hang викликає reset | Перевірено в Proteus |
| `ds18b20_read` | Sensor | Читання температури DS18B20 | Перевірено на легкому температурному прикладі | Перевірено в Proteus |

## 4. Відкладені приклади

| Приклад | Причина | Поточний стан | Наступна дія |
|---|---|---|---|
| `ds18b20_search_rom` | XC8 HEX export свідомо відкладено | Відкладено | Пізніше експортувати HEX за потреби |
| `onewire_bus_test` | XC8 HEX export свідомо відкладено | Відкладено | Пізніше експортувати HEX за потреби |

## 5. Потребує перевірки

| Приклад | Причина | Наступна дія |
|---|---|---|
| `ds18b20_multi_read` | Приклад для multi-drop DS18B20 ще потребує перевірки в Proteus і C18 | Окремо перевірити перед позначенням verified |

## 6. Межа Phase 2

Phase 2 зарезервовано для menu-бібліотек і навігації для конкретних дисплеїв.
Такі пункти свідомо не входять до scope Phase 1:

- encoder menu demo
- button/encoder navigation layer
- LCD menu library
- OLED menu library
- display-specific menu examples
- screen-specific UI abstractions

## 7. Примітки щодо репозиторію

- Код цим audit не змінювався.
- Generated-файли не слід комітити.
- `.X` project metadata лишається source of truth для списків source files.
- `git add .` не використовувався.

## 8. Рекомендовані наступні кроки

1. Тримати відкладені HEX-export задачі у postponed, якщо вони не потрібні зараз.
2. Окремо перевірити `ds18b20_multi_read`.
3. Розпочати планування Phase 2 для menu-бібліотек.
4. Пізніше виконати C18 validation там, де це практично.