# debug_multi_backend XC8 приклад

## Призначення

Показує сумісну роботу `UART + DISPLAY + PINS`.

## Апаратна частина

- PIC18F452
- UART TX на RC6 / ніжка 25
- LCD 2x16 I2C backpack на адресі `0x27`
- GPIO канали на бітах PORTC 0..3

## Збірка

```bash
mplab-make -f examples-projects/xc8/debug_multi_backend.X/Makefile build
```

## Очікуваний результат

Текст йде в UART і на LCD, а канал 0 дає heartbeat-імпульси.
