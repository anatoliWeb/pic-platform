# debug_pins_gpio приклад для XC8

[English version](./README.md)

## Призначення

Демонструє backend `PINS` debug-фасаду (`core/debug.h`) через прямий
GPIO-вивід. Текстовий вивід не використовується; діагностичні сигнали
подаються на біти PORTC 0..3.

## Обрані налаштування

У `project_config.h`:

| Define | Значення |
| --- | ---: |
| `DRV_DEBUG_BACKEND_UART` | `0` |
| `DRV_DEBUG_BACKEND_DISPLAY` | `0` |
| `DRV_DEBUG_BACKEND_PINS` | `1` |
| `DRV_DEBUG_PINS_INTERFACE_GPIO` | `1` |
| `DRV_DEBUG_PINS_INTERFACE_I2C` | `0` |
| `DRV_DEBUG_PINS_PORT` | `PORTC` |
| `DRV_DEBUG_PINS_TRIS` | `TRISC` |
| `DRV_DEBUG_PINS_START_BIT` | `0` |
| `DRV_DEBUG_PINS_CHANNEL_COUNT` | `4` |
| `DRV_DEBUG_PINS_PULSE_US` | `10000` |

## Таблиця каналів

Канали debug — це підрядні біти PORTC, починаючи з біта 0.

| Канал debug | Біт порту | Фізичний вивід PIC18F452 | Очікуваний сигнал |
| --- | --- | ---: | --- |
| 0 | RC0 | 15 | імпульс 10 мс; блимання error-code |
| 1 | RC1 | 16 | високий/низький рівень (500 мс) |
| 2 | RC2 | 17 | утримується за `debug_pin_code()` |
| 3 | RC3 | 18 | утримується за `debug_pin_code()` |

Сигнали активні за високим рівнем: високий рівень — це стан "увімкнено".

## Підключення світлодіода

Підключіть один світлодіод на кожний канал через струмообмежувальний
резистор 330–1000 Ом. PIC керує анодом світлодіода через резистор на землю:

```
Port bit  --[330..1000 Ом]--|>|-- GND
```

Ніколи не підключайте світлодіод без струмообмежувального резистора.

## Спостереження за сигналами

- Світлодіод: підключіть як вище; світиться — канал у високому рівні.
- Логічний аналізатор / осцилограф: вимірюйте біт PORTC на очікуваний рівень.
- Тривалість імпульсу — `DRV_DEBUG_PINS_PULSE_US` (за замовчуванням 10000 мкс).
- Паттерн помилки (`debug_pin_error(code)`): канал 0 блимає `(code & 0x0F) + 1`
  разів із кроком 50 мс, далі пауза 200 мс.

## Source isolation

Цей приклад збирає лише GPIO pins backend. Він не компілює `uart.c`, UART
backend, `i2c.c`, `lcd.c` та display backend. Точний список джерел у
`nbproject/configurations.xml`.

## Збірка

```cmd
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .clean-conf
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .build-conf
```

## Очікуваний результат

- Канал 0 дає імпульс щопівсекунди.
- Канал 1 піднімається і опускається щопівсекунди.
- `debug_pin_code(0x05)` тримає канали 0 і 2 високо впродовж секунди.
- `debug_pin_error(2)` змушує канал 0 блимати тричі, далі цикл повторюється.
