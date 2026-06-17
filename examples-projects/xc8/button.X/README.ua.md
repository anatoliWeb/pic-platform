# button.X

## Призначення

Окремий приклад фази 1 для debounce кнопки на PIC18F452.

## Що перевіряє

- Зчитування кнопки active-low.
- Програмний debounce.
- Перемикання LED на стабільному натисканні.
- UART debug-вивід.

## Компоненти Proteus

- PIC18F452.
- Push button.
- Підтягувальний резистор 10 k.
- LED.
- Опційний Virtual Terminal.

## Піни підключення

- RB0: вхід кнопки, active-low.
- RB1: вихід LED.
- RC6/TX: опційний RXD для Virtual Terminal.

## Очікувана поведінка

- Відпущено = HIGH.
- Натиснуто = LOW.
- Нове стабільне натискання перемикає LED.
- Відпускання LED повторно не перемикає.

## Примітки

- Debounce працює через polling.
- Бібліотека кнопки підтримує press, release, click, double click, hold та hold repeat.
- Menu navigation заплановано окремо на Phase 2 і не входить до цього прикладу.

## Статус

Ready for Proteus validation.