[English version](pic18f452-config-bits.md)

# Конфігураційні біти PIC18F452

## Огляд

`pic-platform` зберігає проєктну конфігурацію на рівні застосунку.
Бібліотеки не повинні емітувати `#pragma config`.

Проєкт керує двома пов'язаними речами:

- CONFIG bits через `config_bits.c`
- частотою такту через `project_config.h`

## Файли

- `core/config/project_config_template.h`
- `core/config/platform_config_check.h`
- `core/config/library_requirements.md`
- `core/config/example.c`
- `templates/config/pic18f452/xc8/config_bits.c`
- `templates/config/pic18f452/c18/config_bits.c`
- `examples-projects/xc8/blink.X/project_config.h`
- `examples-projects/xc8/blink.X/config_bits.c`
- `examples-projects/c18/blink.X/project_config.h`
- `examples-projects/c18/blink.X/config_bits.c`

## Макроси частоти

- `PIC_PLATFORM_CLOCK_HZ` - частота проєкту в Hz.
- `DRV_XTAL_FREQ` - значення частоти платформи, яке використовують драйвери.
- `_XTAL_FREQ` - значення частоти, яке використовують виклики delay/timing.
- `PIC_PLATFORM_WDT_ENABLED`, `PIC_PLATFORM_LVP_ENABLED` і
  `PIC_PLATFORM_BOR_ENABLED` - прості допоміжні прапорці для перевірок і нотаток.

Для PIC18 ці значення мають бути узгоджені.

## Рекомендовані значення для PIC18F452

Використовуйте буквальні значення CONFIG у `config_bits.c`:

- `OSC = HS`
- `OSCS = OFF`
- `PWRT = OFF`
- `BOR = ON`
- `BORV = 20`
- `WDT = OFF`
- `WDTPS = 128`
- `CCP2MUX = ON`
- `STVR = ON`
- `LVP = OFF`
- захист коду та запису - OFF

## Потік CONFIG-бітів

1. `project_config.h` зберігає значення на рівні проєкту.
2. `platform_config_check.h` перевіряє наявність потрібних значень.
3. `config_bits.c` емiтує реальні рядки `#pragma config` з буквальними значеннями.
4. Застосунок має фінальний пріоритет над рекомендаціями бібліотек.

## Конвенція для вимог бібліотек

Бібліотеки можуть оголошувати вимоги або рекомендації, наприклад:

- `PIC_PLATFORM_REQUIRES_CLOCK_HZ`
- `PIC_PLATFORM_REQUIRES_WDT_ENABLED`
- `PIC_PLATFORM_RECOMMENDS_WDT_ON`
- `PIC_PLATFORM_REQUIRES_CCP2MUX_ENABLED`

Бібліотеки не повинні емітувати `#pragma config`.

## Нотатки для компіляторів

- Для XC8 і C18 використовуються окремі шаблони `config_bits.c`.
- Макроси в `#pragma config` для XC8 виявилися ненадійними для PIC18F452.
- Для XC8 і C18 слід використовувати буквальні значення CONFIG у `config_bits.c`.
- `project_config.h` залишається для частоти, таймінгів і допоміжних прапорців.

## Зв'язок з `TODO.md`

Цей шар вважається реалізованим, коли присутні шаблони, checker, документація
та інтеграція з example-проєктами.