[🇬🇧 English version](./ext_interrupt.md)

# Драйвер Зовнішніх Переривань

## Опис
Драйвер керування INT0/INT1/INT2 із callback-моделлю обробки.

## API
- `ext_interrupt_init()`
- `ext_interrupt_enable()`
- `ext_interrupt_disable()`
- `ext_interrupt_set_edge()`
- `ext_interrupt_set_callback()`
- `ext_interrupt_clear_flag()`
- `ext_interrupt_get_flag()`
- `ext_interrupt_irq_handler()`

## Приклад
```c
ext_interrupt_init();
ext_interrupt_set_edge(EXT_INT0, EXT_INT_EDGE_FALLING);
ext_interrupt_set_callback(EXT_INT0, on_button_irq);
ext_interrupt_enable(EXT_INT0);
```

## Особливості
- Реальний ISR належить проєкту; у ньому викликається `ext_interrupt_irq_handler()`.
- Callback-модель відділяє драйвер від логіки застосунку.

## Залежності
- `core/compiler.h`
- `core/types.h`
