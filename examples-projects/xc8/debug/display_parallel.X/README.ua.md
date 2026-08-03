# debug_display_parallel приклад для XC8

[English version](./README.md)

## Призначення

Показує backend `DISPLAY` через паралельний (4-bit) LCD-адаптер як додатковий
варіант до основного I2C LCD-прикладу.

## Обрані налаштування

У `project_config.h`:

| Define | Значення |
| --- | ---: |
| `DRV_DEBUG_BACKEND_UART` | `0` |
| `DRV_DEBUG_BACKEND_DISPLAY` | `1` |
| `DRV_DEBUG_DISPLAY_TYPE_LCD_2X16` | `1` |
| `DRV_DEBUG_DISPLAY_INTERFACE_PARALLEL` | `1` |

## Апаратна частина та карти пінів

- LCD 2x16 (HD44780) у 4-bit режимі
- Мережа даних на `PORTD` (`D4..D7` => `RD4..RD7`)
- RS на RB0
- EN на RB1

Драйвер використовує лише 4 лінії даних `RD4..RD7`.

## Ownership дисплея

Паралельний драйвер вимагає вказівники на порти перед ініціалізацією. Проект
викликає `lcd_prepare()` (усередині — `lcd_init_pins(...)`) **до**
`debug_init()`. Приклад володіє шиною `PORTD` та лініями керування
`RB0`/`RB1`.

## Source isolation

Цей приклад збирає лише паралельний LCD-шлях (`gpio.c`, `lcd.c`). Він не
компілює `i2c.c` та I2C-адаптер. Точний список джерел у
`nbproject/configurations.xml`.

## Збірка

```cmd
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .clean-conf
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .build-conf
```

## Очікуваний результат

На LCD видно стартове повідомлення і лічильник `tick=` на двох рядках.
