[🇬🇧 English version](./encoder.md)

# Encoder Driver

## Опис

Драйвер квадратичного енкодера з визначенням напряму, delta та абсолютної позиції.
Кнопка енкодера інтегрована через існуючий `button_t`.
Ця версія працює через polling і не потребує interrupts.

## API

- `encoder_init()`
- `encoder_tick()`
- `encoder_update()` - wrapper для сумісності
- `encoder_get_delta()`
- `encoder_get_position()`
- `encoder_set_position()`
- `encoder_clicked()`
- `encoder_double_clicked()`
- `encoder_held()`
- `encoder_get_click_count()`

## Приклад

```c
encoder_t enc;
encoder_init(&enc, &PORTB, &TRISB, 0u, 1u, &PORTB, &TRISB, 2u);

while (1)
{
    encoder_tick(&enc);

    if (encoder_get_delta(&enc) > 0) { /* CW step */ }
    if (encoder_get_delta(&enc) < 0) { /* CCW step */ }
    if (encoder_clicked(&enc)) { /* click action */ }
    if (encoder_double_clicked(&enc)) { /* double click action */ }
    if (encoder_held(&enc)) { /* hold action */ }
}
```

## Особливості

- `encoder_get_delta()` зберігає стару сумісну поведінку і повертає по одному кроку за виклик.
- `encoder_get_position()` повертає накопичену signed-позицію.
- Кнопка працює через вбудований state machine `button_t`.
- Рекомендований polling: кожні 1-5 ms.

## Залежності

- `core/compiler.h`
- `core/types.h`
- `drivers/timers/tick`
- `libraries/input/button`
