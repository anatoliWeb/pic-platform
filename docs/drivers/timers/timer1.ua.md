[🇬🇧 English version](./timer1.md)

# Драйвер Timer1

## Опис
Низькорівневий драйвер Timer1 з керуванням старт/стоп, читанням/записом значення, interrupt і callback.

## API
- `timer1_init()`
- `timer1_start()`
- `timer1_stop()`
- `timer1_set()`
- `timer1_get()`
- `timer1_enable_interrupt()`
- `timer1_disable_interrupt()`
- `timer1_set_callback()`
- `timer1_irq_handler()`

## Приклад
```c
timer1_init(8u);
timer1_set_callback(on_tmr1_overflow);
timer1_enable_interrupt();
timer1_start();
```

## Особливості
- `timer1_irq_handler()` має викликатися з ISR.
- Часто використовується як база часу/tick.

## Залежності
- `core/compiler.h`
- `core/types.h`
