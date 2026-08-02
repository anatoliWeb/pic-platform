# debug_display_i2c приклад для XC8

[English version](./README.md)

## Призначення

Демонструє backend `DISPLAY` debug-фасаду (`core/debug.h`) на LCD 2x16 з
PCF8574 I2C backpack-платою.

## Обрані налаштування

У `project_config.h`:

| Define | Значення |
| --- | ---: |
| `DRV_DEBUG_BACKEND_UART` | `0` |
| `DRV_DEBUG_BACKEND_DISPLAY` | `1` |
| `DRV_DEBUG_BACKEND_PINS` | `0` |
| `DRV_DEBUG_DISPLAY_TYPE_LCD_2X16` | `1` |
| `DRV_DEBUG_DISPLAY_INTERFACE_I2C` | `1` |
| `DRV_DEBUG_DISPLAY_INTERFACE_PARALLEL` | `0` |
| `DRV_DEBUG_DISPLAY_I2C_ADDR` | `0x27` |

## Апаратна частина

- LCD 2x16 (HD44780) + PCF8574 I2C backpack
- VCC, GND
- SDA на RC4 / вивід 23
- SCL на RC3 / вивід 18
- I2C-адреса `0x27`

## Підключення

| LCD / PCF8574 | PIC18F452 | Фізичний вивід PIC |
| --- | --- | ---: |
| VCC | VCC | 11, 32 |
| GND | GND | 12, 31 |
| SDA | RC4 | 23 |
| SCL | RC3 | 18 |

На SDA та SCL потрібні підтягувальні резистори 4.7 кОм.

## Source isolation

Цей приклад збирає лише I2C display backend. Він не компілює `uart.c`,
UART backend, GPIO pins backend або паралельний LCD-драйвер. Точний список
джерел у `nbproject/configurations.xml`.

## Збірка

```cmd
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .clean-conf
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .build-conf
```

## Очікуваний екран

```text
SYSTEM OK
COUNT=<значення>
```

Екран очищається, на рядку 1 виводиться `SYSTEM OK`, на рядку 2 - `COUNT=`
та значення лічильника.
