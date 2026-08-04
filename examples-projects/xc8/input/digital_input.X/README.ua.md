# digital_input.X

[English version](./README.md)

## Призначення

Simulation/debug demo для `libraries/input/digital_input`. Він подає синтетичні raw levels і
timestamp-и, щоб показати debounce, edges та latch behavior.

## Потрібні source files

- `main.c`
- `config_bits.c`
- `project_config.h`
- `../../../../libraries/input/digital_input/digital_input.c`

## Що показує

- active-high і active-low mapping
- debounce та stable state transitions
- single rise / fall events
- optional latch mode

## Примітки

- Це demo лише для simulation/debug.
- `digital_input_rose()` і `digital_input_fell()` споживають свої event flags.

## Як зібрати

```cmd
python scripts\build_xc8_project.py examples-projects\xc8\input\digital_input.X
```

## Статус

Готово до XC8 build validation.
