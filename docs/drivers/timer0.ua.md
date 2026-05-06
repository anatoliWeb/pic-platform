[🇬🇧 English version](./timer0.md)

# Драйвер Timer0

## Опис
Низькорівневий драйвер Timer0 для задач лічильника/таймінгу з callback на overflow interrupt.

## API
- `timer0_init()`
- `timer0_start()`
- `timer0_stop()`
- `timer0_set()`
- `timer0_get()`
- `timer0_enable_interrupt()`
- `timer0_disable_interrupt()`
- `timer0_set_callback()`
- `timer0_irq_handler()`

## Приклад
```c
timer0_init(256u);
timer0_set_callback(on_tmr0_overflow);
timer0_enable_interrupt();
timer0_start();
```

## Особливості
- `timer0_irq_handler()` викликається з ISR мікроконтролера.
- Може бути базою для періодичного tick.

## Залежності
- `core/compiler.h`
- `core/types.h`
