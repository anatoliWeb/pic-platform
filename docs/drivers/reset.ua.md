[🇬🇧 English version](./reset.md)

# Reset Helper

## Опис
Helper для визначення причини ресету та обслуговування reset flags.

## API
- `reset_init()`
- `reset_get_cause()`
- `reset_clear_flags()`

## Приклад
```c
reset_init();
cause = reset_get_cause();
reset_clear_flags();
```

## Особливості
- Надає інформацію про системну причину ресету.
- Корисно для діагностики після перезапуску.

## Залежності
- `core/compiler.h`
- `core/types.h`
