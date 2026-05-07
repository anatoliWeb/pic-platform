[🇬🇧 English version](./encoder.md)

# Encoder Драйвер

## Опис

Драйвер квадратичного енкодера з визначенням напрямку/дельти та інтегрованою кнопкою.
Підтримує відстеження CW/CCW через періодичний виклик `encoder_update()`.

## API

- `encoder_init()`
- `encoder_update()`
- `encoder_get_delta()`

## Приклад

```c
encoder_t enc;
encoder_init(&enc, &PORTB, &TRISB, 4u, 5u, &PORTB, &TRISB, 0u);
encoder_update(&enc);

delta = encoder_get_delta(&enc); // + / - для CW / CCW
button_update(&enc.button);
```

## Особливості

- `encoder_get_delta()` повертає signed-кроки обертання.
- Кнопка енкодера працює через вбудований стан `button_t`.

## Залежності

- `core/compiler.h`
- `core/types.h`
- `drivers/button`

