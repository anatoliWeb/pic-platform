[🇬🇧 English version](./ccp_compare.md)

# CCP Compare Драйвер

## Опис
CCP драйвер у режимі Compare для подій за умовою збігу таймера.

## API
- `ccp_compare_init()`
- `ccp_compare_enable()`
- `ccp_compare_disable()`
- `ccp_compare_set_value()`
- `ccp_compare_set_callback()`
- `ccp_compare_irq_handler()`

## Приклад
```c
ccp_compare_init();
ccp_compare_set_value(50000u);
ccp_compare_set_callback(on_compare);
ccp_compare_enable();
```

## Особливості
- ISR має викликати `ccp_compare_irq_handler()`.
- Використовується для детермінованих таймінгових подій.

## Залежності
- `core/compiler.h`
- `core/types.h`
- База таймера
