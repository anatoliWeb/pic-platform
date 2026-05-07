# RTC Framework

## Опис

`rtc_framework` — уніфікований шар абстракції для RTC.
Він дає один API для software RTC і hardware backend-ів (DS1302, DS3231, майбутні реалізації).

## Публічний API

- `rtc_init()`
- `rtc_set_datetime()`
- `rtc_get_datetime()`
- `rtc_set_unix()`
- `rtc_get_unix()`
- `rtc_tick()` (опціональний tick для software RTC)
- `rtc_is_valid()`

## Модель datetime

`rtc_datetime_t` містить:

- year, month, day
- hour, minute, second
- weekday

Framework перевіряє коректність дати/часу з урахуванням leap year.

## Модель backend-абстракції

Інтерфейс backend:

- `set_datetime(ctx, dt)`
- `get_datetime(ctx, dt)`

`rtc_t` працює у двох режимах:

- hardware backend mode (є callbacks)
- software RTC mode (без callbacks)

Це дозволяє зберегти стабільний API для різних RTC чипів.

## Нотатки про Unix конвертацію

Підтримується:

- datetime -> unix timestamp
- unix timestamp -> datetime

Реалізація:

- тільки integer math
- правила leap year (400/100/4)
- epoch: 1970-01-01

## Цілі переносимості

- одна прикладна логіка для DS1302, DS3231, software RTC
- без dynamic memory
- мале RAM споживання
- embedded-friendly поведінка

## Обмеження

- Unix timestamp 32-bit.
- Немає timezone/DST логіки в framework layer.
- Alarm API залишено для майбутнього розширення.

## Залежності

- `core/*`
