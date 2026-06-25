[English version](./README.md)

# multiplex_manual.X

## Призначення

Чотирирозрядний приклад із ручним refresh, сфокусований на fixed-point форматуванні десяткових чисел для PIC18F452.

## Що демонструє

- `seven_segment_process()` у головному циклі
- display buffer, відокремлений від GPIO refresh
- `seven_segment_set_fixed_formatted()`
- fixed-point вивід без `float`
- позиціювання десяткової крапки через `scaled_value` і `decimal_places`
- значення менші за `1.0`, зокрема `0.05`
- приклад від'ємного fixed-point значення
- фіксовану яскравість `100%` і вимкнене blinking для першої простої перевірки
- індикацію помилки на RE0 і heartbeat на RE1

## Source Files

- `main.c`
- `config_bits.c`
- `../../../../core/delay.c`
- `../../../../drivers/gpio/gpio.c`
- `../../../../libraries/display/seven_segment/seven_segment.c`

## Pin Mapping

- RD0..RD6: спільні сегменти A..G
- RD7: спільна десяткова крапка
- RC0..RC3: лінії вибору розрядів
- RE0: вихід помилки
- RE1: heartbeat вихід

## Тип індикатора

- Common cathode

## Режим refresh

- Manual
- Функція refresh: `seven_segment_process()`
- Timer interrupt: не використовується

## Fixed-point API

- `scaled_value = 1234`, `decimal_places = 2` -> `12.34`
- `scaled_value = 75`, `decimal_places = 1` -> `7.5`
- `scaled_value = 5`, `decimal_places = 2` -> `0.05`

У прикладі використовується лише цілочисельний fixed-point шлях без floating-point форматування.

## Динамічна поведінка

Приклад автоматично перемикає підготовлені fixed-point сцени:

1. `0.00`
2. `0.25`
3. `0.50`
4. `0.75`
5. `1.00`
6. `1.25`
7. `12.34`
8. `7.5`
9. `0.05`
10. zero-padded `00.05`
11. `-12.3`

RE1 перемикається при кожній зміні сцени.
RE0 лишається в `0` у нормальному режимі й переходить у `1`, якщо форматування або ініціалізація завершились помилкою.

## Яскравість

- Зафіксована на `100%`
- Blinking вимкнено

## Troubleshooting

- Якщо видно мерехтіння, `main` loop працює надто повільно для стабільного manual multiplex refresh.
- Якщо десяткова крапка з'являється не на тій цифрі, перевір підключення DP до RD7 і порядок розрядів на RC0..RC3.
- Якщо RE0 переходить у `1`, перевір fixed-point конфігурацію або налаштування дисплея.

## Статус

Готово до перевірки в Proteus.
