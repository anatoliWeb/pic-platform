# Debug-фасад

[English version](./debug.md)

## Призначення

`libraries/system/debug` - це compile-time debug-фасад для PIC18F452.
`core/debug.h` перекидає на нього виклики і зберігає старі макроси `DBG_*` та
`DRV_DEBUG_*` без залежності від `uart_debug.h`.

Backend-и:

```text
UART     - текст через спільний UART-драйвер
DISPLAY  - текст на LCD 2x16
PINS     - діагностичні сигнали через GPIO
```

## Збірка

Windows route:

```cmd
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .clean-conf
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .build-conf
```

Кожен приклад збирає власний hex, наприклад
`dist/default/production/debug_default_uart.X.production.hex`. Точна назва
виводу визначається MPLAB X з імені папки проєкту (`*.X`) та активної
конфігурації (`default`, `production`).

## Стандарти

| Define | Стандарт | Примітка |
| --- | ---: | --- |
| `DRV_DEBUG_ENABLE` | `1` | головний перемикач |
| `DRV_DEBUG_BACKEND_UART` | `1` | стандартний backend |
| `DRV_DEBUG_BACKEND_DISPLAY` | `0` | LCD backend |
| `DRV_DEBUG_BACKEND_PINS` | `0` | GPIO backend |
| `DRV_DEBUG_UART_BAUD` | `9600` | швидкість UART |
| `DRV_DEBUG_DISPLAY_AUTO_INIT` | `1` | init transport/LCD + clear/reset |
| `DRV_DEBUG_PINS_CHANNEL_COUNT` | `4` | 1..8 |
| `DRV_DEBUG_PINS_PULSE_US` | `10000` | має вміститись у `uint16_t` |
| `DRV_DEBUG_LEVEL` | `INFO` | `ERROR`..`TRACE` |

Boolean define-и мають бути тільки `0` або `1`.

## Поведінка

- `DRV_DEBUG_ENABLE=0` робить усі `debug_*` макроси no-op без обчислення аргументів.
- Фільтр severity теж не обчислює аргументи, коли виклик відфільтровано.
- `debug_write_i16()` коректно друкує `-32768..32767`.
- `DRV_DEBUG_DISPLAY_AUTO_INIT=1` ініціалізує транспорт і LCD, потім очищає екран.
- `DRV_DEBUG_DISPLAY_AUTO_INIT=0` не ініціалізує транспорт, не ініціалізує LCD і не очищає зовнішній дисплей.
- `DRV_DEBUG_PINS_START_BIT + DRV_DEBUG_PINS_CHANNEL_COUNT` не може перевищувати `8`.

## Source matrix

| Приклад | Збирається |
| --- | --- |
| `debug_default_uart.X` | `main.c`, `config_bits.c`, `core/delay.c`, `drivers/communication/uart/uart.c`, `libraries/system/debug/debug.c` |
| `debug_display_i2c.X` | `main.c`, `config_bits.c`, `core/delay.c`, `drivers/communication/i2c/i2c.c`, `libraries/system/debug/debug.c` |
| `debug_display_parallel.X` | `main.c`, `config_bits.c`, `core/delay.c`, `drivers/gpio/gpio.c`, `libraries/display/lcd_hd44780/lcd.c`, `libraries/system/debug/debug.c` |
| `debug_pins_gpio.X` | `main.c`, `config_bits.c`, `core/delay.c`, `drivers/gpio/gpio.c`, `libraries/system/debug/debug.c` |
| `debug_multi_backend.X` | `main.c`, `config_bits.c`, `core/delay.c`, `drivers/communication/uart/uart.c`, `drivers/communication/i2c/i2c.c`, `drivers/gpio/gpio.c`, `libraries/system/debug/debug.c` |
| `debug_disabled.X` | `main.c`, `config_bits.c`, `core/delay.c` |

## Quick start приклади

Три практичні проєкти демонструють головні backend-и. Кожен проєкт компілює
лише джерела свого backend-у.

### 1. UART

| Позиція | Значення |
| --- | --- |
| Проєкт | `examples-projects/xc8/debug_default_uart.X` |
| Defines | `DRV_DEBUG_BACKEND_UART=1` (backend за замовчуванням) |
| Апаратна частина | PIC18F452, RC6/TX вивід 25, RC7/RX вивід 26, 9600 8N1 |
| Очікується | `BOOT`, далі `counter=` що секунди |

### 2. LCD 2x16 I2C

| Позиція | Значення |
| --- | --- |
| Проєкт | `examples-projects/xc8/debug_display_i2c.X` |
| Defines | `DRV_DEBUG_BACKEND_DISPLAY=1`, `DRV_DEBUG_DISPLAY_INTERFACE_I2C=1`, `DRV_DEBUG_DISPLAY_I2C_ADDR=0x27` |
| Апаратна частина | LCD 2x16 + PCF8574, SCL RC3/вивід 18, SDA RC4/вивід 23, pull-ups |
| Очікується | `SYSTEM OK` на рядку 1, `COUNT=<значення>` на рядку 2 |

### 3. GPIO blink / сигнали

| Позиція | Значення |
| --- | --- |
| Проєкт | `examples-projects/xc8/debug_pins_gpio.X` |
| Defines | `DRV_DEBUG_BACKEND_PINS=1`, `DRV_DEBUG_PINS_INTERFACE_GPIO=1`, `DRV_DEBUG_PINS_PORT=PORTC`, `DRV_DEBUG_PINS_START_BIT=0`, `DRV_DEBUG_PINS_CHANNEL_COUNT=4` |
| Апаратна частина | LED + резистор 330–1000 Ом на кожен канал PORTC0..3 |
| Очікується | pulse, set/clear, code та error патерни на PORTC0..3 |

Додаткові технічні приклади: `debug_display_parallel.X`, `debug_multi_backend.X`
(текст у кілька текстових backend-ів), `debug_disabled.X`
(`DRV_DEBUG_ENABLE=0`, все компілюється в no-op).

## Сумісність

`core/debug.h` підтримує старі імена `DBG_PRINT`, `DBG_PRINTLN`,
`DBG_WRITE_BYTE`, `DBG_PRINT_INT`, `DBG_PRINT_HEX`, `DRV_DEBUG_INIT`,
`DRV_DEBUG_LOG`, `DRV_DEBUG_LOG_HEX`.

`libraries/system/uart_debug/uart_debug.h` лишається окремим шляхом для старих
проєктів.

## ROM/RAM

| Конфіг | ROM | RAM |
| --- | ---: | ---: |
| `debug_default_uart.X` | `1246` | `93` |
| `debug_display_i2c.X` | `2132` | `32` |
| `debug_display_parallel.X` | `2234` | `50` |
| `debug_pins_gpio.X` | `976` | `23` |
| `debug_multi_backend.X` | `3028` | `98` |
| `debug_disabled.X` | `102` | `3` |
