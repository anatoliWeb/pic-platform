[🇬🇧 English version](./timer3.md)

# Драйвер Timer3

## Опис
Низькорівневий драйвер Timer3 з механізмом interrupt/callback для таймінгових задач.

## API
- `timer3_init()`
- `timer3_start()`
- `timer3_stop()`
- `timer3_set()`
- `timer3_get()`
- `timer3_enable_interrupt()`
- `timer3_disable_interrupt()`
- `timer3_set_callback()`
- `timer3_irq_handler()`

## Приклад
```c
timer3_init(8u);
timer3_set_callback(on_tmr3_overflow);
timer3_enable_interrupt();
timer3_start();
```

## Особливості
- `timer3_irq_handler()` викликається з ISR.
- Може використовуватись як додаткова база часу.

## Залежності
- `core/compiler.h`
- `core/types.h`
