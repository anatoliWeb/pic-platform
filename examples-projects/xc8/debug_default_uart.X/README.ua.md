# debug_default_uart приклад для XC8

[English version](./README.md)

## Призначення

Демонструє backend `UART` debug-фасаду (`core/debug.h`) як backend за
замовчуванням. У `project_config.h` жоден інший backend явно не вибрано.

## Обрані налаштування

Застосовуються значення за замовчуванням з `libraries/system/debug/debug.h`:

| Define | Значення |
| --- | ---: |
| `DRV_DEBUG_BACKEND_UART` | `1` (за замовчуванням) |
| `DRV_DEBUG_BACKEND_DISPLAY` | `0` (за замовчуванням) |
| `DRV_DEBUG_BACKEND_PINS` | `0` (за замовчуванням) |
| `DRV_DEBUG_UART_BAUD` | `9600` |

## Апаратна частина

- PIC18F452
- UART TX на RC6 / вивід 25
- UART RX на RC7 / вивід 26
- Virtual Terminal на 9600 бод, 8N1

## Source isolation

Цей приклад збирає лише UART backend. Він не компілює I2C, LCD, display
backend або GPIO pins backend. Точний список джерел у `nbproject/configurations.xml`.

## Збірка

```cmd
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .clean-conf
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .build-conf
```

## Очікуваний результат

`BOOT` з'являється один раз, далі кожну секунду друкується лічильник
`counter=`.

## Апаратні піни

| Сигнал | Порт / біт | Фізичний вивід |
| --- | --- | ---: |
| UART TX | RC6 | 25 |
| UART RX | RC7 | 26 |
