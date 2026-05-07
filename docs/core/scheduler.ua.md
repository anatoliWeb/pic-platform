[English version](./scheduler.md)

# Кооперативний планувальник (Core)

## Огляд

`core/scheduler` — це детермінований кооперативний планувальник задач для bare-metal
PIC18-систем. Він виконує періодичні callback-функції з фіксованого пулу задач.

## Публічний API

- `scheduler_init()`
- `scheduler_add_task()`
- `scheduler_remove_task()`
- `scheduler_update()`
- `scheduler_pause_task()`
- `scheduler_resume_task()`

Опціональні допоміжні функції:

- `scheduler_set_interval()`
- `scheduler_run_now()`

## Модель задачі

Тип callback-функції задачі:

- `typedef void (*scheduler_task_cb_t)(void* ctx);`

Кожна задача містить:

- callback
- вказівник на контекст
- `interval_ms`
- `last_run_ms`
- прапорець enabled
- прапорець run-immediately

## Модель часу / виконання

- `scheduler_update(now_ms)` проходить по статичних слотах задач.
- Задача виконується, коли `now_ms - last_run_ms >= interval_ms`.
- `run_immediately` виконує задачу один раз під час наступного update.
- Планувальник не блокує виконання і не викликає delay всередині.

Рекомендація щодо джерела часу:

- передавайте `rtos_get_tick_ms()` у `scheduler_update()` з головного циклу.

## Детермінована модель зберігання

- фіксований статичний пул (`SCHEDULER_MAX_TASKS`)
- без динамічного виділення пам’яті
- без перемикання контексту
- без стеків задач

## Обмеження

- повністю кооперативний режим: callback-функції повинні швидко завершуватися
- блокуючі callback-функції знижують швидкість реакції всіх задач
- без preemptive-поведінки

## Примітки для PIC18

- оптимізовано для MCU класу PIC18F452
- легка реалізація лише з цілочисельною логікою
- підходить для опитування UI, читання сенсорів, оновлення комунікації та animation ticks
