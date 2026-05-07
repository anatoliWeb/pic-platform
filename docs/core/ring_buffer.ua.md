[🇬🇧 English version](./ring_buffer.md)

# Ring Buffer Драйвер

## Опис

Байтовий FIFO-буфер для потокової обробки даних.
Розрахований на UART RX та подібні producer/consumer сценарії.

## API

- `rb_init()`
- `rb_push()`
- `rb_pop()`
- `rb_is_empty()`
- `rb_is_full()`
- `rb_available()`

## Приклад

```c
static ring_buffer_t rx_rb;
static uint8_t rx_storage[64];

rb_init(&rx_rb, rx_storage, sizeof(rx_storage));
(void)rb_push(&rx_rb, received_byte);  // сторона ISR

while (rb_pop(&rx_rb, &data) != 0u)    // сторона main loop
{
    // обробка даних
}
```

## Особливості

- Підходить для ISR + main loop використання (`head`/`tail` оголошені як volatile).
- Поведінка при overflow керується макросом `RB_OVERWRITE`.

## Залежності

- `core/compiler.h`
- `core/types.h`

