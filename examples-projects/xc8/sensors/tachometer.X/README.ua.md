# tachometer.X

[English version](./README.md)

## Призначення

Simulation/debug demo для `libraries/sensors/tachometer`. Він подає синтетичні timestamp-и імпульсів
і відстежує RPM/status без будь-якого board-specific підключення tachometer.

## Потрібні source files

- `main.c`
- `config_bits.c`
- `project_config.h`
- `../../../../libraries/sensors/tachometer/tachometer.c`

## Що показує

- керування expected-running
- перший імпульс, другий імпульс і обчислення RPM
- startup grace та timeout
- noise rejection через minimum pulse interval

## Примітки

- Це demo лише для simulation/debug.
- Жодної policy для fan, relay, PWM або heater у прикладі немає.

## Як зібрати

```cmd
python scripts\build_xc8_project.py examples-projects\xc8\sensors\tachometer.X
```

Для збірки LIGHTWEIGHT-профілю передайте `TACHOMETER_LIGHTWEIGHT=1` як
project-wide compiler define (однаково для прикладу і `tachometer.c`):
```cmd
xc8 -mcpu=18F452 -DTACHOMETER_LIGHTWEIGHT=1 ...\main.c ...\config_bits.c ...\tachometer.c
```

## Статус

Готово до XC8 build validation.
