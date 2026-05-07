# Бібліотека Software RTC

## Опис

`software_rtc` — легкий RTC backend, що працює без зовнішнього RTC чипа.
Підходить для симуляції, тестів, Proteus і fallback-режиму часу.

## Публічний API

- `software_rtc_init()`
- `software_rtc_tick_ms()`
- `software_rtc_tick_seconds()`
- `software_rtc_set_datetime()`
- `software_rtc_get_datetime()`
- `software_rtc_sync()` (опційно)
- `software_rtc_is_running()` (опційно)
- `software_rtc_get_driver()` (адаптер backend для `rtc_framework`)

## Tick модель

- `software_rtc_tick_ms()` накопичує мілісекунди.
- Коли сума досягає 1000 мс, час збільшується на секунду.
- `software_rtc_tick_seconds()` напряму додає секунди.
- Ролловер обробляє:
  - секунди/хвилини/години
  - день/місяць/рік
  - leap year
  - weekday

Без delay-based таймінгів.

## Інтеграція з backend

`software_rtc_get_driver()` повертає `rtc_driver_t`.
Це дозволяє підключити software RTC у `rtc_framework`:

- `rtc_init(&rtc, software_rtc_get_driver(), &sw_ctx);`

Конвертації datetime/unix залишаються централізованими у `rtc_framework`.

## Архітектурні нотатки

- Без dynamic memory.
- Лише integer logic.
- Малий стан:
  - поточний datetime
  - ms accumulator
  - running flag

## Drift і синхронізація

- Точність software RTC залежить від якості source tick.
- При jitter у scheduler/timer можливий накопичений drift.
- Для корекції часу використовуйте `software_rtc_sync()`.

## Використання в симуляторі / Proteus

- Зручно, коли RTC чип відсутній.
- Може працювати від періодичного виклику з timer ISR або scheduler task.
- Підходить для детермінованих тестових сценаріїв.

## Залежності

- `core/*`
- `libraries/time/rtc_framework`
