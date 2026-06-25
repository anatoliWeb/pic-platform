[English version](./README.md)

# keys_diode_coded.X

## Призначення

Приклад diode-coded shared-line keys для вибору blink mask і перемикання між режимом форматованого числа та режимом `HHMM`.

## Що демонструє

- diode-coded exact masks через `segment_keys`
- відсутність зміни буфера для unknown або ambiguous masks
- перемикання між числовим режимом і режимом `HHMM`
- blink mask для одного розряду і для кількох розрядів
- optional leading zero для годин у time mode
- повторне використання одного display buffer для number і time scenes

## Source Files

- `main.c`
- `config_bits.c`
- `../../../../core/delay.c`
- `../../../../drivers/gpio/gpio.c`
- `../../../../drivers/timers/tick/tick.c`
- `../../../../drivers/timers/timer1/timer1.c`
- `../../../../libraries/display/seven_segment/seven_segment.c`
- `../../../../libraries/input/button/button.c`
- `../../../../libraries/input/segment_keys/segment_keys.c`

## Pin Mapping

- RD0..RD6: спільні сегменти A..G
- RD7: спільна десяткова крапка
- RC0..RC3: лінії вибору розрядів

## Логічні кнопки

- UP: маска сегментів `0x01`
- DOWN: маска сегментів `0x03`
- OK: маска сегментів `0x07`

## Динамічна поведінка

- UP циклічно перемикає predefined blink masks
- DOWN перемикає числовий режим і режим `HHMM`
- OK оновлює поточну сцену без руйнування моделі display buffer
- invalid masks ігноруються та не створюють логічних button events

## Примітки

- У time mode ефект двокрапки зроблено через десяткові крапки середніх розрядів.
- У number mode приклад лишає форматування з вирівнюванням праворуч.
- Ambiguous masks тут навмисно обробляються як no-op.

## Статус

Готово до перевірки в Proteus.
