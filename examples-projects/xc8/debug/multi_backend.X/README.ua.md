# debug_multi_backend приклад для XC8

[English version](./README.md)

## Призначення

Технічний приклад, який одночасно використовує `UART + DISPLAY + PINS` з
одного debug-фасаду.

## Обрані налаштування

У MPLAB `define-macros`:

| Define | Значення |
| --- | ---: |
| `DRV_DEBUG_BACKEND_UART` | `1` (за замовчуванням) |
| `DRV_DEBUG_BACKEND_DISPLAY` | `1` |
| `DRV_DEBUG_BACKEND_PINS` | `1` |
| `DRV_DEBUG_DISPLAY_TYPE_LCD_2X16` | `1` |
| `DRV_DEBUG_DISPLAY_INTERFACE_I2C` | `1` |
| `DRV_DEBUG_DISPLAY_I2C_ADDR` | `0x27` |
| `DRV_DEBUG_PINS_INTERFACE_GPIO` | `1` |
| `DRV_DEBUG_PINS_PORT` | `PORTC` |
| `DRV_DEBUG_PINS_TRIS` | `TRISC` |
| `DRV_DEBUG_PINS_START_BIT` | `0` |
| `DRV_DEBUG_PINS_CHANNEL_COUNT` | `4` |

## Text API vs Signal API

- **Text API** (`debug_write`, `debug_write_line`, `debug_write_u16`, ...)
  надсилається у кожен увімкнений текстовий backend: тут UART **і** LCD.
  Один текстовий виклик виводиться в обидва backend-и.
- **Signal API** (`debug_pin_*`) маршрутизується лише в PINS backend. Сигнальні
  виклики не потрапляють ні в UART, ні на LCD.

Це ключова думка multi-backend прикладу: текст йде в усі текстові backend-и, а
сигнали — тільки в pins.

## Апаратна частина

- PIC18F452
- UART TX на RC6 / вивід 25
- LCD 2x16 I2C backpack на адресі `0x27`
- GPIO канали на бітах PORTC 0..3

## Source isolation

Цей приклад збирає `uart.c`, `i2c.c` і `gpio.c` разом. Точний список джерел у
`nbproject/configurations.xml`.

## Збірка

```cmd
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .clean-conf
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .build-conf
```

## Очікуваний результат

Текст йде в UART і на LCD одночасно, а канал 0 дає heartbeat-імпульси.
