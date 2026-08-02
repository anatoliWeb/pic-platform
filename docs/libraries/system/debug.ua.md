# Debug-фасад (українська)

[English version](./debug.md)

## Призначення

Бібліотека `libraries/system/debug` — це розширюваний, compile-time
debug-фасад для PIC18F452. Прикладна програма викликає один невеликий API, а
вивід спрямовується кожному backend-у, вибраному define-ами під час компіляції:

```text
UART     - послідовний вивід через спільний UART-драйвер
DISPLAY  - текстовий вивід на символьний LCD
PINS     - діагностичні сигнальні на фізичні виводи (GPIO)
```

У firmware потрапляє лише код вибраних backend-ів та їх залежностей. Точка
входу — `core/debug.h`.

## Глобальне ввімкнення

Головний перемикач — `DRV_DEBUG_ENABLE` (за замовчуванням `1`, визначений у
`core/config.h`). Якщо `0` — код усіх backend-ів і залежностей не компілюється,
а всі виклики `debug_*` стають no-op макросами; прикладний код не потребує
додаткових `#if`.

## Вибір backend-ів

```c
#define DRV_DEBUG_BACKEND_UART     1   /* стандарт */
#define DRV_DEBUG_BACKEND_DISPLAY  0   /* стандарт */
#define DRV_DEBUG_BACKEND_PINS     0   /* стандарт */
```

Без додаткових налаштувань використовується UART на стандартній швидкості.
Можна вмикати кілька backend-ів одночасно — текстове повідомлення піде в усі
увімкнені текстові backend-и.

## Налаштування

| Define | Стандарт | Значення |
| --- | ---: | --- |
| `DRV_DEBUG_ENABLE` | `1` | головний перемикач |
| `DRV_DEBUG_BACKEND_UART` | `1` | UART-backend |
| `DRV_DEBUG_BACKEND_DISPLAY` | `0` | DISPLAY-backend |
| `DRV_DEBUG_BACKEND_PINS` | `0` | PINS-backend |
| `DRV_DEBUG_UART_BAUD` | `9600` | швидкість UART |
| `DRV_DEBUG_DISPLAY_TYPE_LCD_2X16` | `0` | адаптер LCD 2x16 |
| `DRV_DEBUG_DISPLAY_INTERFACE_I2C` | `0` | I2C PCF8574 |
| `DRV_DEBUG_DISPLAY_INTERFACE_PARALLEL` | `0` | прямий 4-bit LCD |
| `DRV_DEBUG_DISPLAY_I2C_ADDR` | - | I2C-адреса (I2C-режим) |
| `DRV_DEBUG_DISPLAY_I2C_FREQ` | `100000` | частота I2C |
| `DRV_DEBUG_DISPLAY_AUTO_INIT` | `1` | debug сам ініціалізує дисплей |
| `DRV_DEBUG_PINS_INTERFACE_GPIO` | `0` | GPIO-виходи |
| `DRV_DEBUG_PINS_INTERFACE_I2C` | `0` | не підтримується |
| `DRV_DEBUG_PINS_PORT` / `DRV_DEBUG_PINS_TRIS` | - | регістри GPIO |
| `DRV_DEBUG_PINS_START_BIT` | - | перший біт каналів |
| `DRV_DEBUG_PINS_CHANNEL_COUNT` | `4` | кількість каналів |
| `DRV_DEBUG_PINS_PULSE_US` | `1000` | тривалість імпульсу |
| `DRV_DEBUG_LEVEL` | `INFO` | фільтр severity |

## Мінімальна інтеграція (UART)

```c
#include "core/debug.h"

int main(void)
{
    debug_init();
    debug_write_line("BOOT");
    for (;;) { }
}
```

## UART

Дефолтна швидкість — `9600`, перевизначається через `DRV_DEBUG_UART_BAUD`.
PIC18F452 має один EUSART, тому вибір порту не потрібен.

## DISPLAY

Cursor влаштований на матрицю 2 рядки x 16 colонок. `\n` переміз на наступний
рядок, при досяганні colумента 16 — автоматичний перенос. За допомогою
`debug_clear()` екран очищається. `DRV_DEBUG_DISPLAY_AUTO_INIT=1` — дисплей
ініціалізується в `debug_init()`. У режимі `_PARALLEL` проєкт має викликати
`lcd_init_pins()` перед `debug_init()`.

## PINS

Не текстовень backend. Підтримує `debug_pin_pulse/set/code/error`.
Прямий GPIO-режим використовує драйвер GPIO на блоці бекст `START_BIT..+N`
портare `DRV_DEBUG_PINS_PORT`. I2C expander-режим не підтримується.

## Backward compatibility

Старі `DBG_*` макроси та `uart_debug`, а також `core/debug.h`-алласи
залишаються та працюють. Див. таблицю сумісності в англійській версії.