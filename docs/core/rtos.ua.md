[???? English version](./rtos.md)

# RTOS Абстракція (Core)

## Огляд

`core/rtos` — легка абстракція затримок/таймінгу для PIC18-проєктів.
Вона орієнтована на bare-metal і готує кодову базу до опціонального RTOS у майбутньому.

## Публічний API

- `rtos_init()`
- `rtos_delay_ms(uint32_t ms)`
- `rtos_sleep_ms(uint32_t ms)`
- `rtos_sleep_us(uint32_t us)`
- `rtos_get_tick_ms()`
- `rtos_is_enabled()`

Опціональні helper-и:

- `rtos_yield()`
- `rtos_yield_if_needed()`
- `rtos_enter_critical()`
- `rtos_exit_critical()`

## Поведінка затримок у Bare-Metal

- `DRV_USE_FREERTOS` за замовчуванням дорівнює `0`.
- `rtos_sleep_ms()` використовує наявний `delay_ms()` кроками по 1 мс.
- `rtos_sleep_us()` використовує наявний `delay_us()`.
- `rtos_delay_ms()` збережено як сумісний wrapper над `rtos_sleep_ms()`.
- `rtos_get_tick_ms()` використовує `tick_get()`, якщо tick driver активний.
- Якщо системного tick немає, використовується внутрішній fallback-лічильник.

## Нотатки для Cooperative Scheduler

- Затримки блокують поточний потік виконання.
- `rtos_yield_if_needed()` викликається під час мілісекундних кроків затримки.
- У bare-metal режимі yield можна підключити через `DRV_RTOS_YIELD_HOOK()`
  для scheduler/event pumping.
- Блокуючі задачі все одно знижують реактивність cooperative системи.

## Сумісність з Майбутнім RTOS

- Якщо `DRV_USE_FREERTOS = 1`, є guarded hooks для майбутнього мапінгу
  на `vTaskDelay()` і `taskYIELD()`.
- Заголовки FreeRTOS зараз не потрібні.
- Жорстка залежність від FreeRTOS не додається.

## Дизайн-Обмеження

- Без dynamic memory
- Лише integer-логіка
- У цьому оновленні немає queue/mutex backend
- PIC18F452-friendly витрати RAM і CPU

## Рекомендації по Міграції

Бібліотекам бажано поступово переходити з прямих delay-викликів:

- `delay_ms()`
- `DRV_DELAY_MS()`

на RTOS-safe абстракцію:

- `rtos_sleep_ms()`
- `rtos_sleep_us()`

## Файли

- `core/rtos/rtos.h`
- `core/rtos/rtos.c`
- `core/rtos/rtos_port.h`
- `core/rtos/example.c`
