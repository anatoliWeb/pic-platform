[English version](./README.md)

# keys_single_line.X

## Призначення

Приклад shared-line buttons, який поєднує декодування клавіш із segment bus та buffered-форматування числа
з блиманням одного активного розряду.

## Що демонструє

- `segment_keys` exact-mask decoding на спільній шині сегментів
- події `button`: click, double-click і hold-repeat із зовнішніх raw states
- форматований чотирирозрядний вивід значення
- перемикання leading zero
- перемикання вирівнювання left/right
- блимання активного редагованого розряду
- зміну яскравості без зупинки refresh

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
- DOWN: маска сегментів `0x02`
- OK: маска сегментів `0x04`

## Динамічна поведінка

- UP і DOWN змінюють форматоване значення з кроком, що відповідає поточному блимному розряду
- активний розряд автоматично переміщується між чотирма позиціями дисплея
- click на OK перемикає leading-zero mode
- double-click на OK перемикає вирівнювання left/right
- hold-repeat на OK циклічно змінює preset-и яскравості

## Примітки

- Refresh лишається ручним через `seven_segment_process()`.
- `tick_get()` дає неблокуючу часову базу для button logic і blink timing.
- Це лише демонстрація API, а не menu system.

## Статус

Готово до перевірки в Proteus.
