# Бібліотека NTP Synchronization

## Опис

`ntp_sync` — компактна transport-independent NTP клієнтська бібліотека для embedded.
Вона формує/парсить NTP пакети і синхронізує RTC без реалізації сокетів.

## Публічний API

- `ntp_sync_init()`
- `ntp_sync_request()`
- `ntp_sync_process_response()`
- `ntp_sync_apply_to_rtc()`
- `ntp_sync_is_valid()` (опційно)
- `ntp_sync_get_unix()` (опційно)

## Модель транспортної абстракції

Мережевий шар користувача надає callbacks:

- send callback
- receive callback

Бібліотека працює тільки з raw NTP UDP payload через ці callbacks.
Ніяких припущень про WiFi/Ethernet стек.

## Нотатки по парсингу пакету

- Розмір запиту: 48 байт
- Клієнтський пакет з налаштованою NTP версією
- Перевірки відповіді:
  - мінімальний розмір
  - mode/version
  - валідність transmit timestamp
- Виділення UNIX часу:
  - з поля transmit timestamp seconds
  - конвертація з NTP epoch у UNIX epoch

## Інтеграція з RTC і timezone

- `ntp_sync_apply_to_rtc()` записує час через `rtc_framework`.
- За потреби застосовується конвертація UTC->local через `timezone`.

## Обробка помилок

Статуси покривають:

- send failure
- no/short response
- invalid mode/version
- invalid timestamp
- RTC update failure

## Embedded обмеження

- Без dynamic memory
- Лише integer логіка
- Мінімальна реалізація NTP client функцій

## Залежності

- `core/*`
- `libraries/time/rtc_framework`
- `libraries/time/timezone`
