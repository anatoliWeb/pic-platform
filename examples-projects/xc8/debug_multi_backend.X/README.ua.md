# debug_multi_backend XC8 приклад

## Призначення

Показує сумісну роботу `UART + DISPLAY + PINS`.

## Апаратна частина

- PIC18F452
- UART TX на RC6 / ніжка 25
- LCD 2x16 I2C backpack на адресі `0x27`
- GPIO канали на бітах PORTC 0..3

## Збірка

```cmd
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .clean-conf
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .build-conf
```

## Очікуваний результат

Текст йде в UART і на LCD, а канал 0 дає heartbeat-імпульси.
