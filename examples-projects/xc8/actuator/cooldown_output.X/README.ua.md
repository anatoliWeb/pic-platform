# cooldown_output.X

[English version](./README.md)

## Призначення

Simulation/debug demo для `libraries/actuator/cooldown_output`. Він керує helper-ом через синтетичні
зміни request та timestamp-и.

## Потрібні source files

- `main.c`
- `config_bits.c`
- `project_config.h`
- `../../../../libraries/actuator/cooldown_output/cooldown_output.c`

## Що показує

- request on і негайний active state
- request off, який запускає cooldown
- re-request, що скасовує shutdown
- remaining time та force-off

## Примітки

- Це demo лише для simulation/debug.
- Жодної policy для fan, heater або tachometer у прикладі немає.

## Як зібрати

```cmd
python scripts\build_xc8_project.py examples-projects\xc8\actuator\cooldown_output.X
```

## Статус

Готово до XC8 build validation.
