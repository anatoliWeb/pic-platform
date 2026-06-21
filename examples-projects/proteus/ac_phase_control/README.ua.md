# AC Phase Control / Proteus

## Призначення

Proteus-приклад для low-voltage phase-control симуляції на PIC18F452.

## Що перевіряє

- Спільну zero-cross синхронізацію
- Чотири незалежні канали керування фазою
- Короткі gate pulses після zero-cross
- Non-blocking fade на одному каналі

## Компоненти Proteus

- PIC18F452
- Fake zero-cross pulse source на `RB0 / INT0`
- LED або logic analyzer на `RD0..RD3`
- Oscilloscope для перевірки waveform

## Підключення

- `RB0 / INT0`: fake zero-cross input
- `RD0..RD3`: gate pulse outputs

## Очікувана поведінка

- Усі канали запускаються від одного zero-cross edge.
- Кожен канал формує не більше одного короткого pulse за півхвилю.
- `RD0` змінює timing pulse під час fade.
- Виходи не повинні зависати в HIGH.

## Статус

Готово до перевірки в Proteus.
