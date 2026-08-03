# i2c_pcf8574 приклад для XC8

[English version](./README.md)

## Призначення

Демонструє повторне використання I2C-транспорту PCF8574 для символьного LCD
HD44780 (`libraries/display/lcd_hd44780/lcd_i2c.h`) напряму, без debug-фасаду.

## Обрані налаштування

У `project_config.h` та MPLAB `define-macros`:

| Define | Значення |
| --- | ---: |
| `PIC_PLATFORM_CLOCK_HZ` | `8000000` |

Розведення PCF8574 (`LCD_I2C_PIN_RS/RW/EN/BL`, `LCD_I2C_DATA_SHIFT`) за
замовчуванням відповідає типовій backpack-платі і може бути перевизначене
через compiler defines. I2C-адреса - 7-бітна, `0x27`; адреси вище `0x7F`
відхиляються.

## Апаратна частина

- PIC18F452
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

Цей приклад збирає лише `main.c`, `config_bits.c`, `core/delay.c`,
`drivers/communication/i2c/i2c.c` та
`libraries/display/lcd_hd44780/lcd_i2c.c`. Він не компілює debug-фасад, UART,
GPIO або паралельний LCD-драйвер. Точний список джерел у
`nbproject/configurations.xml`.

## Збірка

```cmd
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .clean-conf
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .build-conf
```

## Очікуваний екран

```text
LCD I2C OK
PCF8574
```

## Поведінка при помилці

`main.c` перевіряє значення, повернуте `lcd_i2c_init()`. Якщо воно відмінне
від `LCD_I2C_OK` (backpack-плата відсутня, неправильна адреса або збій на
шині), приклад нічого не виводить на LCD і входить у fail-state цикл.
Опційний світлодіод на RB0 блимає, поки триває цей стан. Причина збою
повідомляється через `lcd_i2c_last_status()` (`LCD_I2C_NO_ACK`), а
`lcd_i2c_is_ready()` повертає 0.
