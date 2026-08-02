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

```bash
mplab-make -f examples-projects/xc8/debug_default_uart.X/Makefile build
```

## Очікуваний результат

У терміналі з'являються `BOOT` і лічильник `tick=` раз на секунду.
