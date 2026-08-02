# debug_display_parallel XC8 приклад

## Призначення

Показує `DISPLAY` backend з LCD 2x16 через parallel-адаптер.

## Апаратна частина

- PIC18F452
- LCD 2x16 у 4-bit mode
- Data bus: PORTD
- RS/EN: RB0/RB1
- D4..D7: RD4..RD7

## Збірка

```bash
mplab-make -f examples-projects/xc8/debug_display_parallel.X/Makefile build
```

## Очікуваний результат

На LCD видно стартове повідомлення і лічильник `tick=`.
