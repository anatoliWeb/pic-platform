# Бібліотека Timezone Utilities

## Опис

`timezone` надає компактні утиліти конвертації UTC/local для embedded систем.
Модель орієнтована на фіксований UTC offset і простий DST режим.

Це не повна timezone database система.

## Публічний API

- `timezone_init()`
- `timezone_set_offset_minutes()`
- `timezone_set_dst()`
- `timezone_utc_to_local()`
- `timezone_local_to_utc()`
- `timezone_get_offset_minutes()` (опційно)
- `timezone_is_dst_enabled()` (опційно)

## Модель timezone

- Базовий offset у хвилинах (наприклад UTC+2 = `120`).
- Опційний DST прапорець:
  - `0` = вимкнено
  - `1` = додати одну годину

Підсумковий offset:

- `offset_minutes * 60` секунд
- плюс `3600` секунд при ввімкненому DST

## Модель datetime

Використовується `rtc_datetime_t` з `rtc_framework`.
Дублювання datetime структур відсутнє.

## Нотатки по конвертації

- UTC -> local: `local = utc + offset`
- local -> UTC: `utc = local - offset`
- Всередині використовується integer unix-time математика.
- Легка обробка leap year/day/month присутня.

## DST нотатки

- Підтримується лише простий DST on/off.
- Немає регіональних правил, timezone бази або історичних переходів.
- Архітектура придатна для майбутнього розширення rule-based DST.

## Embedded обмеження

- Без dynamic memory.
- Без floating-point.
- Діапазон 32-bit unix-time обмежений.

## Залежності

- `core/*`
- `libraries/time/rtc_framework`
