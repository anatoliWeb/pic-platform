# Бібліотека Relay Control

## Опис

`relay` — легка універсальна бібліотека керування реле для embedded automation.
Підтримує як базове керування, так і неблокуючі timed сценарії.

## Публічний API

- `relay_init()`
- `relay_on()`
- `relay_off()`
- `relay_toggle()`
- `relay_set_state()`
- `relay_get_state()`
- `relay_set_inverted()` (опційно)
- `relay_pulse()` (опційно)
- `relay_delayed_off()` (опційно)
- `relay_update()` (опційний таймінг-обробник)

## Логіка інверсії (Active HIGH / Active LOW)

- `inverted = 0`: модулі active HIGH
- `inverted = 1`: модулі active LOW

Бібліотека зберігає логічний стан (`on/off`) і мапить його у фізичний GPIO рівень.

## Timing модель реле

Блокуючі затримки не використовуються.
Timed-логіка реалізована через періодичний update:

- `relay_pulse(duration_ms, now_ms)`
- `relay_delayed_off(delay_ms, now_ms)`
- `relay_update(now_ms)` перевіряє дедлайни і вимикає реле вчасно

Модель добре підходить для scheduler/tick циклу.

## Нотатки безпеки

- `safe_start_off` у `relay_init()` забезпечує fail-safe старт.
- Архітектура готова до watchdog/fail-safe розширень на рівні application.
- Для реального hardware враховуйте flyback/opto isolation вимоги relay board.

## Embedded обмеження

- Без dynamic memory
- Лише integer logic
- Малий RAM footprint

## Залежності

- `core/*`
- `drivers/gpio`
- опційна інтеграція зі scheduler/tick на рівні application
