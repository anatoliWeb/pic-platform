# debug_default_uart XC8 приклад

## Призначення

Показує стандартний `UART` backend бібліотеки `core/debug.h` без явного
вибору backend-а.

## Апаратна частина

- PIC18F452
- UART TX на RC6 / ніжка 25
- UART RX на RC7 / ніжка 26
- Virtual Terminal: 9600 baud, 8N1

## Збірка

```cmd
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .clean-conf
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .build-conf
```

## Очікуваний результат

У терміналі з'являються `BOOT` і лічильник `tick=` раз на секунду.
