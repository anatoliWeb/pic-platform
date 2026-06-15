[English version](clock-configuration.md)

# Налаштування тактування для PIC18 проєктів

## Чому `main.c` недостатньо

`#define PIC_PLATFORM_CLOCK_HZ ...` всередині `main.c` видно лише цьому
translation unit.

Кожен `.c` файл компілюється окремо, тому драйвери `uart.c`, `delay.c`,
`onewire.c` не бачать define, який існує тільки в `main.c`.

## Що таке translation unit

Translation unit - це окремий вихідний файл після preprocessing.

Наприклад:

- `main.c`
- `core/delay.c`
- `XC8/drivers/communication/uart/uart.c`

Кожен такий файл проходить власний preprocessor і власну стадію компіляції.

## Правильна модель частоти

Використовуйте один головний macro на рівні проєкту:

- `PIC_PLATFORM_CLOCK_HZ`

Потім спільний device layer нехай автоматично створює:

- `DRV_XTAL_FREQ`
- `_XTAL_FREQ`

Так application code, drivers і delay helper будуть узгоджені між собою.

## Налаштування в MPLAB X

Macro треба задати тут:

`Project Properties -> XC8 Global Options -> xc8-cc -> Preprocessing and messages -> Preprocessor macros`

Приклад:

```text
PIC_PLATFORM_CLOCK_HZ=10000000UL
```

Таке значення стане видимим для всіх translation units у проєкті.

Якщо ви редагуєте metadata вручну, потрібно заново відкрити проєкт у MPLAB X
або regenerate project files, щоб згенерований makefile підхопив новий macro.

## Рекомендована модель для example-проєктів

Структура має бути такою:

- `project_config.h`
  - містить `PIC_PLATFORM_CLOCK_HZ`
- `nbproject/configurations.xml`
  - містить той самий macro як project-wide define
- `core/device.h`
  - створює `DRV_XTAL_FREQ` і `_XTAL_FREQ`
- driver `.c` файли
  - використовують `DRV_XTAL_FREQ` для таймінгів і baud rate

## Чек для Proteus

Перевірте, що збігаються такі значення:

- PIC Clock Frequency
- частота зовнішнього кварца
- `PIC_PLATFORM_CLOCK_HZ`
- `DRV_XTAL_FREQ`
- `_XTAL_FREQ`

Якщо хоч одне з них лишилось на 8 MHz, а решта вже 10 MHz, UART і delay
почнуть працювати з похибкою.

## Перевірка UART

Для PIC18F452 при 10 MHz:

- у Virtual Terminal ставте `9600 baud`
- формат: `8N1`
- очікуваний baud generator приблизно:
  - `SPBRG = 64` у 8-bit high-speed mode
  - `SPBRGH:SPBRG = 259` у 16-bit BRG mode

Якщо baud все ще рахується від 8 MHz, на екрані будуть "крякозябри".

## Чек-лист для "крякозябр"

Якщо UART показує сміття, перевірте:

1. `PIC_PLATFORM_CLOCK_HZ` у `project_config.h`
2. `PIC_PLATFORM_CLOCK_HZ` у MPLAB project-wide macros
3. PIC Clock Frequency у Proteus
4. Baud rate у Virtual Terminal
5. Чи не залишився активним `PIC_PLATFORM_ALLOW_DEFAULT_CLOCK`

## Legacy fallback

Старі проєкти можуть використовувати default 8 MHz тільки якщо явно увімкнути
`PIC_PLATFORM_ALLOW_DEFAULT_CLOCK`.

Нові проєкти мають задавати `PIC_PLATFORM_CLOCK_HZ` безпосередньо.
