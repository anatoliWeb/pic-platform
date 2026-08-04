# multi_channel_relay.X

## Призначення

Багатоканальний приклад phase-control для PIC18F452, який демонструє опційний per-channel relay bypass: triac phase-control вихід з незалежним реле, яке перебирає керування на повній потужності.

## Що перевіряє

- Спільну zero-cross синхронізацію на `RB0 / INT0`
- Чотири незалежні phase-control канали
- Relay bypass на каналах 0, 1 і 2 (канал 3 — лише phase)
- Гістерезис між relay ON threshold (`98%`) і relay OFF threshold (`96%`)
- Break-before-make і мінімальний час ON/OFF реле
- Запит `100%` утримує реле увімкненим, а triac gate залишається мовчазним
- Zero-cross timeout fail-safe: `ac_phase_control_process()` переходить у `ZERO_CROSS_LOST` і вимикає всі виходи, якщо імпульси на `RB0` припиняються на `zero_cross_timeout_ms`
- Emergency all-off через `RB1` (active low)

## Джерело таймера

- Таймер обирається у [project_config.h](./project_config.h).
- Alias за замовчуванням: `AC_PHASE_CONTROL_SELECTED_TIMER`
- Значення за замовчуванням: `AC_PHASE_CONTROL_TIMER2`
- Доступні значення enum:
  - `AC_PHASE_CONTROL_TIMER2`

## Підключення пінів

- `RB0 / INT0`: fake zero-cross pulse source на `100 Hz`
- `RB1`: emergency all-off вхід (active low)
- `RD0`: triac gate каналу 0
- `RD1`: triac gate каналу 1
- `RD2`: triac gate каналу 2
- `RD3`: triac gate каналу 3
- `RD4`: relay bypass каналу 0
- `RD5`: relay bypass каналу 1
- `RD6`: relay bypass каналу 2
- Підключення каналів використовує bit mask, наприклад `RD0 -> 1 << 0`, а не raw number bit.
- Група активується лише після zero-cross і стає неактивною після завершення півхвилі.

## Очікувана поведінка

- Усі виходи синхронізуються з одним zero-cross pulse на `RB0`.
- Кожен канал формує не більше одного короткого gate pulse за півхвилю.
- Канал 0 проходить `50% -> 97% -> 98% -> 100% -> 50%`: реле вмикається на `98%` (з гістерезисом) і вимикається на `96%`.
- Канали 1 і 2 залишаються на фіксованих `50%` і `100%`.
- Канал 3 залишається на фіксованому `30%` (без реле).
- На `100%` triac gate мовчить, а реле комутує всю півхвилю.
- `0%` не формує pulse.
- Жоден вихід не повинен залишатися HIGH.
- Без zero-cross pulse імпульс не повинен з’являтися.

## Примітки

- Це phase-control simulation, а не звичайний PWM.
- Основна ціль перевірки — waveform.
- `Timer2` є перевіреним джерелом за замовчуванням на цьому етапі.
- Реальний mains і `220V AC` залишаються поза межами цього етапу.
- Додаткові diagnostic pins:
  - `RC0`: toggle для zero-cross
  - `RC1`: toggle для timer callback
  - `RC2`: індикатор gate-active

## Статус

Готово до перевірки в Proteus.
