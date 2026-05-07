[🇺🇦 Ukrainian version](./ring_buffer.ua.md)

# Ring Buffer Driver

## Description

Byte-oriented FIFO buffer for streaming data.
Designed for UART RX and similar producer/consumer data paths.

## API

- `rb_init()`
- `rb_push()`
- `rb_pop()`
- `rb_is_empty()`
- `rb_is_full()`
- `rb_available()`

## Example

```c
static ring_buffer_t rx_rb;
static uint8_t rx_storage[64];

rb_init(&rx_rb, rx_storage, sizeof(rx_storage));
(void)rb_push(&rx_rb, received_byte);  // ISR side

while (rb_pop(&rx_rb, &data) != 0u)   // main loop side
{
    // process data
}
```

## Notes

- Suitable for ISR + main-loop usage (`head`/`tail` are volatile).
- Overflow behavior is controlled by `RB_OVERWRITE` macro.

## Dependencies

- `core/compiler.h`
- `core/types.h`

