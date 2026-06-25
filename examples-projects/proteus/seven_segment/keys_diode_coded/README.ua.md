# Нотатки Proteus: keys_diode_coded

## Призначення

Документація-заготовка для перевірки diode-coded shared-line keys із вибором blink mask і перемиканням між часом та числом.

## Проєкт прикладу

- `examples-projects/xc8/seven_segment/keys_diode_coded.X`

## Що перевірити

- UP циклічно перемикає predefined blink masks
- DOWN перемикає режим числа і режим `HHMM`
- OK оновлює активну сцену без очищення моделі display buffer
- invalid або ambiguous masks не змінюють видимий вміст і не створюють логічних key events

## Статус

Готово до перевірки в Proteus.
