# seven_segment

[English version](./seven_segment.md)

## Опис

Буферизована бібліотека seven-segment індикаторів для однорозрядних і мультиплексованих дисплеїв.
Бібліотека підтримує два режими володіння refresh:

- `SEVEN_SEGMENT_REFRESH_MANUAL`
- `SEVEN_SEGMENT_REFRESH_TIMER`

У manual mode оновлення лишається в головному циклі застосунку.
У timer mode бібліотека сама бере на себе refresh і прив'язує один екземпляр дисплея до одного апаратного timer backend.

## API

- `seven_segment_init()`
- `seven_segment_refresh()`
- `seven_segment_process()`
- `seven_segment_stop()`
- `seven_segment_deinit()`
- `seven_segment_irq_handler()`
- `seven_segment_is_timer_running()`
- `seven_segment_get_refresh_count()`
- `seven_segment_set_number()`
- `seven_segment_set_number_formatted()`
- `seven_segment_set_fixed()`
- `seven_segment_set_fixed_formatted()`
- `seven_segment_set_time_hhmm()`
- `seven_segment_set_brightness()`
- `seven_segment_set_blink_mask()`
- `seven_segment_set_blink_period_ms()`
- `seven_segment_set_blink_enabled()`

## Власник Refresh

### Ручний Режим

Використовуйте manual mode, коли мультиплексний крок виконує сам застосунок:

```c
config.refresh_mode = SEVEN_SEGMENT_REFRESH_MANUAL;
config.timer = SEVEN_SEGMENT_TIMER_NONE;
config.refresh_period_us = 0U;

seven_segment_init(&display, &config);

while (1)
{
    seven_segment_process(&display);
    seven_segment_blink_update(&display, now_ms);
}
```

Поведінка:

- `seven_segment_process()` виконує один реальний refresh step;
- `seven_segment_refresh()` еквівалентний `seven_segment_process()`;
- blink timing просувається лише через `seven_segment_blink_update()` у main context.

### Режим Від Таймера

Використовуйте timer mode, коли refresh має належати бібліотеці:

```c
config.refresh_mode = SEVEN_SEGMENT_REFRESH_TIMER;
config.timer = SEVEN_SEGMENT_TIMER2;
config.refresh_period_us = 1000U;

seven_segment_init(&display, &config);

INTCONbits.PEIE = 1U;
INTCONbits.GIE = 1U;

while (1)
{
    /* No manual display refresh call is required here. */
}
```

Поведінка:

- бібліотека конфігурує вибраний timer backend;
- бібліотека реєструє внутрішній timer callback;
- timer callback напряму викликає приватний refresh core;
- `seven_segment_process()` і `seven_segment_refresh()` стають safe no-op;
- blink timing автоматично просувається всередині timer-owned шляху.

## Вибір Таймера

Підтримані runtime-вибори:

- `SEVEN_SEGMENT_TIMER0`
- `SEVEN_SEGMENT_TIMER1`
- `SEVEN_SEGMENT_TIMER2`
- `SEVEN_SEGMENT_TIMER3`

Поточне обмеження:

- кожен timer driver має лише один callback slot;
- один timer backend може обслуговувати лише один `seven_segment_t` одночасно;
- `seven_segment_init()` повертає помилку, якщо вибраний таймер уже зайнятий іншим власником callback.

## Видимість Конфігурації Проєкту

Для timer-enabled збірки бібліотеки `project_config.h` має бути видимий і для library translation units.
У MPLAB example projects додайте директорію проєкту в include paths, наприклад:

```text
.;../../../../;../../../../core;../../../../drivers;../../../../libraries
```

Так `seven_segment.c` бачить локальну конфігурацію проєкту та backend macros на кшталт `SEVEN_SEGMENT_ENABLE_TIMER2`.

## Вимоги До Переривань

Бібліотека не встановлює глобальний MCU ISR замість застосунку.
Застосунок усе одно має передавати переривання в dispatcher бібліотеки:

```c
void __interrupt() isr(void)
{
    seven_segment_irq_handler();
}
```

Що має зробити application у timer mode:

- надати глобальний ISR;
- увімкнути `PEIE` і `GIE` після завершення ініціалізації.

## Діагностика

Для timer-owned перевірки:

- `seven_segment_is_timer_running()` повідомляє, чи належить вибраний timer slot поточному екземпляру дисплея;
- `seven_segment_get_refresh_count()` повертає read-only refresh counter, який оновлюється з timer callback після реального refresh step.

Ці helper-и призначені для діагностики й Proteus bring-up, а не для основної логіки дисплея.

## Примітки

- оновлення буфера й далі захищене від tearing під час refresh;
- timer-owned shared-line key queueing лишається окремим pending-кроком інтеграції;
- timer mode варто спочатку перевіряти на мінімальному прикладі з постійним вмістом, а вже потім повертати складніші сцени.
