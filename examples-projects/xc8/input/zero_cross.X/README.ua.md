# zero_cross.X

Автономний XC8 smoke test для `libraries/input/zero_cross`.

## Що показує

- перший edge у timestamp `0`
- переведення через `zero_cross_process()` у `LOST`
- перший recovery edge після `LOST`
- повернення до `ALIVE`
- класифікацію напівперіоду для `50 Hz` і `60 Hz`

## Файли

- `main.c`
- `project_config.h`
- `config_bits.c`
- `nbproject/configurations.xml`
- `nbproject/project.xml`

## Примітки

- Triac і relay outputs не використовуються.
- Демонстрація подає явні synthetic edges і не покладається на прихований zero-cross generator.
