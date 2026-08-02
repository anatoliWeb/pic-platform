# debug_disabled приклад для XC8

[English version](./README.md)

## Призначення

Показує, що debug API компілюється, а всі виклики `debug_*` стають no-op,
коли `DRV_DEBUG_ENABLE=0`.

## Обрані налаштування

У `project_config.h`:

| Define | Значення |
| --- | ---: |
| `DRV_DEBUG_ENABLE` | `0` |

При `DRV_DEBUG_ENABLE=0` фасад, усі backend-и та всі debug-залежності
компілюються повністю. `debug.c` відсутній у списку джерел, тому в образ не
потрапляє жоден UART/I2C/LCD/GPIO backend-код.

## Source isolation

Цей приклад складається лише з `main.c`, `config_bits.c` і `core/delay.c`.
Жоден файл `libraries/system/debug/*` не компілюється. Точний список джерел у
`nbproject/configurations.xml`.

## Збірка

```cmd
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .clean-conf
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .build-conf
```

## Очікуваний результат

Проєкт збирається чисто. Виклики debug залишаються у вихідному коді, але
компілюються в no-op, тому не потрібні ні debug-пристрій, ні backend-залежності.
backend-залежностей.
