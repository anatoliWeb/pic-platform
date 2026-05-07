[English version](./rtos.md)

# RTOS-абстракція (Core)

## Огляд

`core/rtos` — це легка абстракція затримок і таймінгу для PIC18-проєктів.
Вона в першу чергу орієнтована на bare-metal режим і залишає кодову базу готовою
до можливого майбутнього використання RTOS.

## Публічний API

- `rtos_init()`
- `rtos_delay_ms(uint32_t ms)`
- `rtos_sleep_ms(uint32_t ms)`
- `rtos_sleep_us(uint32_t us)`
- `rtos_get_tick_ms()`
- `rtos_is_enabled()`

Опціональні допоміжні функції:

- `rtos_yield()`
- `rtos_yield_if_needed()`
- `rtos_enter_critical()`
- `rtos_exit_critical()`

## Поведінка затримок у Bare-Metal режимі

- `DRV_USE_FREERTOS` за замовчуванням дорівнює `0`.
- `rtos_sleep_ms()` використовує наявну `delay_ms()` з кроком 1 мс.
- `rtos_sleep_us()` використовує наявну `delay_us()`.
- `rtos_delay_ms()` залишено як compatibility wrapper для `rtos_sleep_ms()`.
- `rtos_get_tick_ms()` використовує `tick_get()`, коли tick-драйвер активний.
- Внутрішній fallback-лічильник використовується, коли системний tick недоступний.

## Примітки щодо кооперативного планування

- Затримки є блокуючими для поточного потоку виконання.
- `rtos_yield_if_needed()` викликається під час мілісекундних кроків sleep.
- У bare-metal режимі yield можна підключити через `DRV_RTOS_YIELD_HOOK()`
  для прокачування scheduler/events.
- Блокуючі задачі все одно знижують швидкість реакції в кооперативних системах.

## Сумісність із майбутньою RTOS

- Якщо `DRV_USE_FREERTOS = 1`, захищені hooks готові для майбутнього мапування
  на `vTaskDelay()` та `taskYIELD()`.
- Заголовки FreeRTOS зараз не потрібні.
- Жорстка залежність від FreeRTOS не додається.

## Обмеження дизайну

- Без динамічної пам’яті
- Лише цілочисельна логіка
- Без backend для queue/mutex у цьому оновленні
- PIC18F452-friendly використання RAM і CPU

## Рекомендації щодо міграції

Бібліотеки слід поступово переводити з прямих викликів затримок:

- `delay_ms()`
- `DRV_DELAY_MS()`

на RTOS-safe абстрактні виклики:

- `rtos_sleep_ms()`
- `rtos_sleep_us()`

## Файли

- `core/rtos/rtos.h`
- `core/rtos/rtos.c`
- `core/rtos/rtos_port.h`
- `core/rtos/example.c`
