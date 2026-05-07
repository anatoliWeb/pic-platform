# Бібліотека Cron Scheduler

## Опис

`cron_scheduler` — компактний embedded cron-style планувальник з інтеграцією через `rtc_framework`.
Реалізація навмисно мінімальна для MCU.

Це не повний Linux cron.

## Публічний API

- `cron_scheduler_init()`
- `cron_scheduler_add()`
- `cron_scheduler_remove()`
- `cron_scheduler_update()`
- `cron_scheduler_enable()` (опційно)
- `cron_scheduler_disable()` (опційно)

## Модель планувальника

Кожне правило задачі має поля:

- minute
- hour
- day
- month
- weekday

Підтримка для кожного поля:

- точне значення
- wildcard `CRON_ANY` (аналог `*`)

Задачі зберігаються у фіксованому пулі:

- `CRON_MAX_TASKS`
- без dynamic allocation

## Callback модель

Тип callback:

`typedef void (*cron_callback_t)(void* ctx);`

Кожна задача зберігає:

- callback функцію
- context pointer
- прапори enabled/used

## Модель оновлення

`cron_scheduler_update()`:

1. читає поточний datetime з `rtc_framework`
2. перевіряє всі активні задачі
3. викликає callback для тих, що співпали
4. запам'ятовує last run час, щоб уникнути дубля в межах хвилини

Функцію слід викликати періодично з main loop/scheduler.

## Обмеження для embedded

- Немає текстового cron parser
- Немає dynamic створення задач
- Немає timezone/DST логіки
- Точність спрацювання на рівні хвилини

## Рекомендації по частоті виклику

- Викликайте `cron_scheduler_update()` мінімум раз на секунду.
- Частіші виклики безпечні завдяки захисту від дублювання.

## Залежності

- `core/*`
- `libraries/time/rtc_framework`
