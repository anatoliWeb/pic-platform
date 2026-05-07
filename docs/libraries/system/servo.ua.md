[🇬🇧 English version](./servo.md)

# Бібліотека Керування Servo

## Опис

`servo` — легка універсальна бібліотека керування сервоприводами для embedded-систем.
Підтримує пряме позиціювання за кутом і плавний асинхронний рух без блокуючих затримок.

## Публічний API

- `servo_init()`
- `servo_set_backend()`
- `servo_enable()`
- `servo_disable()`
- `servo_set_angle()`
- `servo_get_angle()`
- `servo_set_pulse_us()`
- `servo_move_smooth()` (опційно)
- `servo_update()` (опційно)
- `servo_is_moving()` (опційно)

## Модель Servo

Налаштовуються параметри:

- діапазон імпульсу (`pulse_min_us`, `pulse_max_us`)
- діапазон кута (`angle_min_deg`, `angle_max_deg`)
- період PWM (`period_us`)

Типові значення для hobby servo:

- 1000 us -> 0 deg
- 1500 us -> центр
- 2000 us -> 180 deg
- період 20 ms

## Модель Руху

`servo_move_smooth()` задає плавний рух до цілі:

- цільовий кут
- швидкість (deg за крок)
- інтервал кроку (ms)

`servo_update(now_ms)` неблокуюче просуває позицію до цілі.
`delay_ms()` не використовується.

## Нотатки по PWM Інтеграції

Бібліотека працює через callback backend, без hardware-specific коду:

- `servo_pwm_apply_cb_t`
- `servo_pwm_enable_cb_t`
- `servo_pwm_disable_cb_t`

Це зберігає переносимість і дозволяє легко підключати наявний PWM driver.

## Приклад

`example.c` демонструє:

- пряме позиціювання
- плавний рух
- неблокуючий sweep
- дві інстанції servo

## Живлення і Безпека

- Серво можуть споживати великі імпульсні струми.
- Рекомендується окреме стабільне живлення для сервоприводів.
- Обов'язково об'єднуйте GND MCU і GND живлення сервоприводів.

## Залежності

- `core/*`
- опційний PWM backend через `drivers/timers/pwm`
- опційне джерело часу через `drivers/timers/tick`

## Обмеження для Embedded

- Без dynamic memory
- Лише integer-математика
- Невеликий стан на кожен екземпляр
