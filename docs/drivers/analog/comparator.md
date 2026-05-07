[🇺🇦 Ukrainian version](./comparator.ua.md)

# Comparator Driver

## Description
Analog comparator driver for threshold-based digital output decision.

## API
- `comparator_init()`
- `comparator_enable()`
- `comparator_disable()`
- `comparator_get_output()`

## Example
```c
comparator_init(CMP_MODE_1);
comparator_enable();
state = comparator_get_output();
```

## Notes
- Supports basic comparator modes.
- Output is returned as logic 0/1.

## Dependencies
- `core/compiler.h`
- `core/types.h`
