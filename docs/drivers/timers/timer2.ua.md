[🇬🇧 English version](./timer2.md)

# Драйвер Timer2

## Опис
Низькорівневий драйвер Timer2 для періодичних задач з callback через interrupt.

## API
- `timer2_init()`
- `timer2_start()`
- `timer2_stop()`
- `timer2_set()`
- `timer2_get()`
- `timer2_enable_interrupt()`
- `timer2_disable_interrupt()`
- `timer2_set_callback()`
- `timer2_irq_handler()`

## Приклад
```c
timer2_init(16u);
timer2_set_callback(on_tmr2_overflow);
timer2_enable_interrupt();
timer2_start();
```

## Особливості
- `timer2_irq_handler()` є ISR-hook.
- Часто використовується разом із PWM.

## Залежності
- `core/compiler.h`
- `core/types.h`
