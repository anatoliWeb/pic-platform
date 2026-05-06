[🇬🇧 English version](./comparator.md)

# Comparator Драйвер

## Опис
Драйвер аналогового компаратора для порогового визначення цифрового стану.

## API
- `comparator_init()`
- `comparator_enable()`
- `comparator_disable()`
- `comparator_get_output()`

## Приклад
```c
comparator_init(CMP_MODE_1);
comparator_enable();
state = comparator_get_output();
```

## Особливості
- Підтримує базові режими компаратора.
- Вихід повертається як логічний 0/1.

## Залежності
- `core/compiler.h`
- `core/types.h`
