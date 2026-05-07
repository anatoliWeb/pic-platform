[🇬🇧 English version](./buzzer.md)

# Бібліотека Buzzer

## Опис

`buzzer` — універсальна бібліотека звукових сповіщень для embedded-систем.
Підтримує active buzzer (тільки GPIO) і passive buzzer (режим тону).

## Публічний API

- `buzzer_init()`
- `buzzer_on()`
- `buzzer_off()`
- `buzzer_toggle()`
- `buzzer_beep()`
- `buzzer_update()`
- `buzzer_set_tone_backend()` (опційно)
- `buzzer_play_tone()` (опційно)
- `buzzer_play_pattern()` (опційно)
- `buzzer_stop()` (опційно)

## Модель відтворення (Non-Blocking)

Бібліотека працює через state machine з періодичним update:

- `buzzer_beep(duration_ms, now_ms)` планує короткий сигнал.
- `buzzer_play_pattern(...)` запускає покроковий шаблон.
- `buzzer_update(now_ms)` просуває таймінг і стани.

Для відтворення не використовується `delay_ms()`.

## Active vs Passive

- Active buzzer: пряме керування GPIO рівнем.
- Passive buzzer: опційний backend колбеків для генерації частоти.
- Якщо tone callbacks не задані, passive режим переходить у GPIO fallback.

## Pattern Engine

Кроки шаблону задаються структурою `buzzer_pattern_step_t`:

- `duration_ms`
- `state` (on/off)
- `frequency_hz`

Підтримуються:

- короткий beep
- подвійний beep
- циклічні послідовності

## Приклад

`example.c` показує:

- короткий сигнал для active buzzer
- тон для passive buzzer через PWM callbacks
- неблокуючий повторюваний шаблон з `tick_get()`

## Залежності

- `core/*`
- `drivers/gpio`
- опційно: `drivers/timers/pwm` (через callbacks на рівні застосунку/прикладу)
- опційно: `drivers/timers/tick` як джерело часу

## Обмеження для Embedded

- Без dynamic memory
- Лише integer-логіка
- Невелике використання RAM
