# ac_phase_control.X

## Призначення

Багатоканальний low-voltage phase-control приклад для PIC18F452 з одним спільним zero-cross входом і чотирма незалежними gate outputs.

## Що перевіряє

- Спільну zero-cross синхронізацію на `RB0 / INT0`
- Чотири незалежні phase-control канали
- Різні фазові затримки для різних рівнів потужності
- Один non-blocking slow fade на `RD0`
- Керування таймером і timer interrupt dispatch через бібліотеку

## Джерело таймера

- Таймер обирається у [project_config.h](./project_config.h).
- Alias за замовчуванням: `AC_PHASE_CONTROL_SELECTED_TIMER`
- Значення за замовчуванням: `AC_PHASE_CONTROL_TIMER2`
- Доступні значення enum:
  - `AC_PHASE_CONTROL_TIMER2`

## Підключення пінів

- `RB0 / INT0`: fake zero-cross pulse source на `100 Hz`
- `RD0`: phase-control channel 0, повільний fade `0% -> 100% -> 0%`
- `RD1`: phase-control channel 1, фіксовано `25%`
- `RD2`: phase-control channel 2, фіксовано `50%`
- `RD3`: phase-control channel 3, фіксовано `75%`
- Підключення каналів використовує bit mask, наприклад `RD0 -> 1 << 0`, а не raw number bit.
- Група активується лише після zero-cross і стає неактивною після завершення півхвилі.

## Очікувана поведінка

- Усі виходи синхронізуються з одним zero-cross pulse на `RB0`.
- Кожен канал формує не більше одного короткого gate pulse за півхвилю.
- `RD1`, `RD2` і `RD3` мають стабільні, але різні затримки.
- `RD0` повільно змінює timing pulse, поки fade рухається від `0%` до `100%` і назад.
- `0%` не формує pulse.
- `100%` формує pulse поблизу `min_delay_us`.
- Жоден вихід не повинен залишатися HIGH.
- Без zero-cross pulse імпульс не повинен з’являтися.

## Примітки

- Це phase-control simulation, а не звичайний PWM.
- Основна ціль перевірки — waveform.
- `Timer2` є перевіреним джерелом за замовчуванням на цьому етапі.
- `Timer0`, `Timer1` і `Timer3` зарезервовані для майбутньої роботи і не входять до цього етапу.
- Реальний mains і `220V AC` залишаються поза межами цього етапу.
- Додаткові diagnostic pins:
  - `RC0`: toggle для zero-cross
  - `RC1`: toggle для timer callback
  - `RC2`: індикатор gate-active

## Статус

Готово до перевірки в Proteus.
