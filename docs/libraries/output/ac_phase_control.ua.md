# AC Phase Control / Zero-Cross Output

## Опис

`ac_phase_control` — це low-voltage, Proteus-first багатоканальна група керування фазою для PIC18F452.
Вона відокремлена від `software_pwm`, тому що не генерує безперервний duty-cycle сигнал. Замість цього один спільний zero-cross запускає нову півхвилю, а кожен увімкнений канал формує один короткий gate pulse зі своєю фазовою затримкою.

## Архітектура

- Один `ac_phase_control_group_t` зберігає спільний timing і масив каналів.
- Підтримується до `AC_PHASE_CONTROL_MAX_CHANNELS` каналів.
- Кожен канал має власний gate pin, enabled state, power percent і delay.
- Вибір таймера передається через `ac_phase_control_init_group()`.
- `ac_phase_control_attach_channel()` очікує gate **bit mask**, наприклад `(1U << 0U)` для `RD0`.
- Вибір timer-driver і reload logic залишаються всередині бібліотеки.
- Програма викликає лише public `ac_phase_control_irq_handler()` зі свого ISR.
- Dynamic allocation не використовується.
- Використовується лише integer-only timing.
- Група активується лише після zero-cross події і стає неактивною після завершення півхвилі.

## Public API

- `ac_phase_control_init_group()`
- `ac_phase_control_attach_channel()`
- `ac_phase_control_detach_channel()`
- `ac_phase_control_set_power_percent()`
- `ac_phase_control_set_delay_us()`
- `ac_phase_control_enable_channel()`
- `ac_phase_control_get_power_percent()`
- `ac_phase_control_is_channel_enabled()`
- `ac_phase_control_on_zero_cross()`
- `ac_phase_control_update_us()`
- `ac_phase_control_stop_group()`
- `ac_phase_control_irq_handler()`
- `ac_phase_control_get_tick_ms()`
- `ac_phase_control_is_any_channel_active()`

## Вибір таймера

- Enum таймера бібліотеки:
- `AC_PHASE_CONTROL_TIMER2`
- Alias за замовчуванням:
  - `AC_PHASE_CONTROL_SELECTED_TIMER`
- Значення за замовчуванням:
  - `AC_PHASE_CONTROL_TIMER2`
- На цьому етапі реалізовано лише перевірений `Timer2` шлях.
- Доступ до timer driver у коді прикладу не потрібен.
- У прикладі `Timer2` лишається перевіреним значенням за замовчуванням і використовується лише існуючий timer API.

## Модель таймінгу

- Типова ціль симуляції AC — `50 Hz`.
- Один повний цикл — `20 ms`.
- Одна півхвиля — `10 ms`.
- `ac_phase_control_on_zero_cross()` починає нову півхвилю.
- `ac_phase_control_update_us()` рухає elapsed time без блокування.
- Кожен увімкнений канал може сформувати не більше одного короткого gate pulse за півхвилю.
- Великі значення elapsed обробляються через ширші тимчасові типи, щоб уникнути overflow.
- Gate output повертається в LOW після завершення pulse або після завершення півхвилі.

## Поведінка fade

- XC8 example використовує повільний non-blocking fade на каналі `0`.
- Fade керується через millisecond helper, який повертає бібліотека.
- `RD0` рухається `0% -> 100% -> 0%`.
- `RD1`, `RD2` і `RD3` залишаються фіксованими на `25%`, `50%` і `75%`.

## Статус валідації

- `Timer2`: реалізований і використовується як значення за замовчуванням.
- `Timer0/1/3` зарезервовані для майбутньої роботи і не входять до цього етапу.
- Proteus-валідація multi-channel: pending.
- Proteus-валідація slow fade: pending.

## Примітки для Proteus

- Використовуйте один fake zero-cross pulse source на `RB0 / INT0`.
- Перевіряйте `RD0..RD3` через oscilloscope або logic analyzer.
- Додаткові diagnostic pins:
  - `RC0`: toggle для zero-cross
  - `RC1`: toggle для timer callback
  - `RC2`: індикатор gate-active
- Усі канали мають синхронізуватись по одному zero-cross edge.
- Менша power означає пізніший gate pulse.
- `0%` не формує pulse.
- `100%` формує pulse близько до `min_delay_us`.
- Жоден вихід не повинен залишатись HIGH.
- Реальні mains та `220V AC` виходять за межі цього модуля.

## Статус

Готово до перевірки в Proteus.
