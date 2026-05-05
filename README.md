# PIC PLATFORM - Universal Drivers (C18 + XC8)

Це база універсальних драйверів для PIC (фокус: PIC18).
Репозиторій не є application-проєктом.

## Цілі

- Універсальні перевикористовувані драйвери
- Підтримка MPLAB C18 та MPLAB XC8
- Мінімальна залежність драйверів від компілятора
- Усі compiler-specific відмінності винесені в `core/compiler.h`

## Структура

- `core/`
- `drivers/`
- `examples/`
- `docs/`
- `C18/`
- `XC8/`

## Як створити новий драйвер

1. Прочитати `DRIVER_GUIDELINES.md`
2. Скопіювати `/drivers/_template`
3. Перейменувати під назву драйвера
4. Реалізувати API та внутрішню логіку драйвера
5. Додати/адаптувати `example.c`

## Приклади для компіляторів

- `examples/common` - універсальні приклади
- `C18/examples` - приклади під MPLAB C18
- `XC8/examples` - приклади під MPLAB XC8

## Core Layer

`core/` — це базовий шар, від якого залежать усі драйвери.
Він надає спільні типи, макроси, конфігурацію та компіляторну абстракцію.

Файли Core Layer:

- `core/compiler.h` - абстракція C18/XC8 і базові compiler wrappers
- `core/types.h` - спільні типи статусів та булевих значень
- `core/bit_utils.h` - універсальні бітові макроси
- `core/delay.h` - єдині wrappers для затримок
- `core/device.h` - базові device/clock налаштування
- `core/interrupts.h` - helper-макроси для global interrupts
- `core/config.h` - compile-time feature flags

Як використовуються драйверами:

- драйвери підключають `core/compiler.h` для compiler portability
- за потреби підключають `core/types.h`, `core/bit_utils.h`, `core/delay.h`
- project-level налаштування виконуються через `core/config.h` та `core/device.h`

## Підтримувані компілятори

- MPLAB C18
- MPLAB XC8
