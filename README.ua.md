[English version](./README.md)

# PIC Drivers Platform (C18 + XC8)

Повторно використовувана платформа для PIC18 з чітким розділенням на `core`, `drivers` і `libraries`.

## Архітектура

- `core/` - абстракція компілятора, затримки, типи, CRC, ring buffer, scheduler, RTOS wrapper
- `drivers/` - низькорівневі драйвери периферії та протоколів
- `libraries/` - повторно використовувані компоненти вищого рівня поверх drivers і core
- `C18/` - compiler-specific файли для MPLAB C18
- `XC8/` - compiler-specific файли для MPLAB XC8
- `docs/` - документація
- `examples-projects/` - wrapper-проєкти MPLAB
- `prompts/` - готові prompt-файли для AI-чатів

## Швидкий старт

1. Тримайте `pic-platform` окремо від прикладного проєкту.
2. Додавайте в MPLAB лише потрібні `.c` файли у `Source Files`.
3. Налаштуйте include paths:
   - `../pic-platform`
   - `../pic-platform/core`
   - `../pic-platform/drivers`
   - `../pic-platform/libraries`
4. Залишайте configuration bits у `config_bits.c`.
5. Використовуйте wrapper-приклади з `examples-projects/` як інтеграційні еталони.

## Основна документація

- Архітектура: [docs/architecture.ua.md](docs/architecture.ua.md)
- Графи залежностей: [docs/architecture/dependency-graph.ua.md](docs/architecture/dependency-graph.ua.md)
- Матриця прикладів: [docs/architecture/example-matrix.ua.md](docs/architecture/example-matrix.ua.md)
- Inventory модулів: [docs/architecture/module-inventory.ua.md](docs/architecture/module-inventory.ua.md)
- Workflow генерації: [docs/architecture/generation-workflow.ua.md](docs/architecture/generation-workflow.ua.md)
- MPLAB інтеграція: [docs/mplab-integration.ua.md](docs/mplab-integration.ua.md)
- Консольна збірка: [docs/build/console-build.ua.md](docs/build/console-build.ua.md)
- Приклади проєктів: [examples-projects/README.ua.md](examples-projects/README.ua.md)
- Бібліотека seven_segment: [docs/libraries/display/seven_segment.ua.md](docs/libraries/display/seven_segment.ua.md)
- Бібліотека segment_keys: [docs/libraries/input/segment_keys.ua.md](docs/libraries/input/segment_keys.ua.md)
- Бібліотека position_drive: [docs/libraries/actuator/position_drive.ua.md](docs/libraries/actuator/position_drive.ua.md)
- Набір prompt-файлів: [docs/prompts.ua.md](docs/prompts.ua.md)

## Короткий статус

- OneWire і DS18B20 перевірені для PIC18F452 + XC8 + Proteus + 10 MHz.
- Input-приклади `button.X`, `encoder.X` і `adc_buttons.X` перевірені в Proteus.
- `software_pwm.X` перевірено в Proteus.
- Grouped seven-segment приклади в `examples-projects/xc8/seven_segment/` реалізовані та готові до перевірки в Proteus, включно з buffered fixed-point formatting у `multiplex_manual.X` і library-owned Timer2 refresh у `multiplex_timer.X`: `basic_manual.X`, `multiplex_manual.X`, `multiplex_timer.X`, `keys_single_line.X` і `keys_diode_coded.X`.
- Приклад position drive `position_drive_adc.X` у `examples-projects/xc8/actuator/` реалізований і готовий до перевірки в Proteus, використовуючи замкнену бібліотеку `position_drive` з ADC-бекендом; encoder-бекенд лишається заглушкою.
