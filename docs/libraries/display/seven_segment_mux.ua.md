[English version](./seven_segment_mux.md)

# Бібліотека мультиплексного seven segment

## Опис
Перевикористовувана неблокуюча бібліотека для багаторозрядних LED seven-segment індикаторів.

## Публічний API
- `seven_segment_mux_init()`
- `seven_segment_mux_set_digit()`
- `seven_segment_mux_set_number()`
- `seven_segment_mux_set_hex()`
- `seven_segment_mux_clear()`
- `seven_segment_mux_set_dot()`
- `seven_segment_mux_update()`

## Підтримувані конфігурації
- 2 розряди
- 4 розряди
- довільна кількість до 8 (`SEVEN_SEGMENT_MUX_MAX_DIGITS`)
- common cathode
- common anode

## Модель конфігурації
`seven_segment_mux_config_t` містить:
- шину сегментів (`segment_config`)
- піни вибору розряду (`digit_pins`)
- кількість розрядів (`digit_count`)
- активний рівень вибору розряду (`digit_active_level`)
- опційне приглушення лідируючих нулів (`suppress_leading_zero`)

## Модель refresh (неблокуюча)
- `seven_segment_mux_update()` оновлює рівно один розряд за виклик.
- Внутрішніх блокуючих delay немає.
- Викликати з:
  - main loop
  - timer ISR hook
  - scheduler task

## Як працює мультиплексування
1. вимкнути всі розряди
2. виставити сегментний патерн поточного розряду
3. увімкнути поточний розряд
4. перейти до наступного розряду

Такий порядок зменшує ghosting.

## Рекомендації по частоті refresh
- Для 4 розрядів бажано повний кадр >= 100 Гц.
- Тобто `update()` викликати щонайменше 400 разів/сек.
- Вища частота зменшує мерехтіння.

## Нотатки по підключенню
- Лінії сегментів A..G(+DP) спільні для всіх розрядів.
- Кожен розряд має окрему лінію вибору.
- Дотримуйтесь обмежень струму (резистори, ключі за потреби).
- Логіка common cathode/common anode задається в конфігу.

## Продуктивність
- Тримайте `update()` коротким і частим.
- Уникайте довгих blocking-ділянок поруч із refresh.
- Якщо refresh у ISR, ISR має бути коротким.

## Залежності
- `core/*`
- `drivers/gpio/*`
- `libraries/display/seven_segment/*` (повторне використання типів)