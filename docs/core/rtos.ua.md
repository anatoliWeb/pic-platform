[English version](./rtos.md)

# RTOS-абстракція (Core)

## Огляд

`core/rtos` забезпечує легку абстракцію задач для PIC18-проєктів.
За замовчуванням підтримує bare-metal режим і готує кодову базу до можливої
майбутньої інтеграції FreeRTOS.

## Публічний API

- `rtos_init()`
- `rtos_delay_ms(uint32_t ms)`
- `rtos_get_tick_ms()`
- `rtos_is_enabled()`

Опціональні заготовки:

- `rtos_yield()`
- `rtos_enter_critical()`
- `rtos_exit_critical()`

## Поведінка в Bare-Metal режимі

- `DRV_USE_FREERTOS` за замовчуванням дорівнює `0`.
- `rtos_delay_ms()` використовує наявну `delay_ms()`.
- `rtos_get_tick_ms()` використовує `tick_get()`, якщо tick-драйвер ініціалізований.
- Якщо tick не запущений, використовується локальний fallback-лічильник.

## Примітки щодо підготовки до FreeRTOS

- Якщо `DRV_USE_FREERTOS` встановлено в `1`, hooks готові для майбутнього мапування.
- Заголовки FreeRTOS поки що не підключаються.
- Пряма залежність від FreeRTOS не потрібна.

## Правила дизайну

- Без динамічної пам’яті
- Без стеків задач у цьому шарі
- Без backend для mutex/queue наразі (лише заготовки)
- Лише цілочисельна логіка
- PIC18-friendly використання RAM

## Рекомендації

- PIC18F452 найкраще підходить для cooperative/bare-metal tasking.
- FreeRTOS на PIC18 з малою RAM є опціональним і обмеженим.
- Бібліотеки повинні використовувати API `core/rtos`, а не прямі виклики FreeRTOS.

## Файли

- `core/rtos/rtos.h`
- `core/rtos/rtos.c`
- `core/rtos/rtos_port.h`
- `core/rtos/example.c`
