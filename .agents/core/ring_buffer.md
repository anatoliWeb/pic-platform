# ring_buffer

## Purpose

Fixed-size byte ring buffer.

## Location

```text
core/ring_buffer/ring_buffer.h
core/ring_buffer/ring_buffer.c
core/ring_buffer/example.c
```

## Public API

| Function/type/macro | Exact signature or type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `ring_buffer_t` | struct | ring buffer state | buffer/size/head/tail | state | none | byte buffer |
| `rb_init` | `void rb_init(ring_buffer_t* rb, uint8_t* buf, uint16_t size);` | initialize buffer | rb, buf, size | none | resets indices |  |
| `rb_push` | `uint8_t rb_push(ring_buffer_t* rb, uint8_t data);` | push one byte | rb, data | success flag | mutates buffer |  |
| `rb_pop` | `uint8_t rb_pop(ring_buffer_t* rb, uint8_t* data);` | pop one byte | rb, output ptr | success flag | mutates buffer |  |
| `rb_is_empty` | `uint8_t rb_is_empty(ring_buffer_t* rb);` | check empty | rb | flag | none |  |
| `rb_is_full` | `uint8_t rb_is_full(ring_buffer_t* rb);` | check full | rb | flag | none |  |
| `rb_available` | `uint16_t rb_available(ring_buffer_t* rb);` | bytes available | rb | count | none |  |
