# debug_pins_gpio XC8 приклад

## Призначення

Показує `PINS` backend з прямим GPIO-виводом.

## Апаратна частина

- PIC18F452
- Канали на бітах PORTC 0..3
- Один канал використовується як heartbeat

## Збірка

```bash
mplab-make -f examples-projects/xc8/debug_pins_gpio.X/Makefile build
```

## Очікуваний результат

Канал 0 дає імпульс кожні 500 мс, а стартовий шаблон виводиться один раз.
