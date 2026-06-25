[English version](./README.md)

# basic_manual.X

## Призначення

Однорозрядний приклад семисегментного індикатора з ручним refresh у `main` loop.

## Режим refresh

- Manual
- Функція refresh: `seven_segment_process()`
- Timer driver для refresh не використовується

## Source Files

- `main.c`
- `config_bits.c`
- `../../../../core/delay.c`
- `../../../../drivers/gpio/gpio.c`
- `../../../../libraries/display/seven_segment/seven_segment.c`

## Pin Mapping

- RD0..RD6: сегменти A..G
- RD7: десяткова крапка
- RC0: дозвіл розряду

## Тип індикатора

- Common cathode

## Динамічна поведінка

- Автоматично рахує `0 -> 9 -> 0`
- Десяткова крапка періодично перемикається
- Global brightness плавно змінюється від мінімуму до максимуму і назад

## Brightness

- Використовується `seven_segment_set_brightness()`
- Діапазон у прикладі: `10..100`

## Proteus wiring

- PIC18F452
- кварц 10 MHz
- однорозрядний common-cathode індикатор
- струмообмежувальні резистори на всіх сегментних лініях

## Troubleshooting

- Якщо цифра мерехтить, викликайте `seven_segment_process()` частіше.
- Довгі блокуючі ділянки в `main` loop погіршують refresh.

## Статус

Готово до перевірки в Proteus.
