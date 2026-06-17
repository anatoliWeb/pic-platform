# encoder.X

## Призначення

Окремий приклад фази 1 для quadrature-енкодера на PIC18F452.

## Що перевіряє

- Визначення напрямку.
- Вивід delta.
- Лічильник абсолютної позиції.
- Опційні події кнопки енкодера через UART.

## Компоненти Proteus

- PIC18F452.
- Rotary encoder з кнопкою.
- Опційні pull-up резистори або PORTB pull-ups у Proteus.
- Virtual Terminal.

## Піни підключення

- RB0: encoder A.
- RB1: encoder B.
- RB2: encoder SW.
- Опційні pull-ups на A, B і SW.
- RC6/TX: UART debug-вивід.

## Очікувана поведінка

- Обертання ліворуч або праворуч змінює delta та position.
- Натискання кнопки енкодера друкує press, release, click, double click, hold та hold repeat.
- Menu logic у цій фазі відсутня.

## Примітки

- Рекомендований polling: кожні 1-5 ms.
- Кнопка енкодера використовує існуючий helper `button_t`.
- Menu navigation заплановано окремо на Phase 2 і не входить до цього прикладу.

## Статус

Ready for Proteus validation.