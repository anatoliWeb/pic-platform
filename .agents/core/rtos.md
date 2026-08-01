# rtos

## Purpose

Lightweight RTOS abstraction for bare-metal and future backend mapping.

## Location

```text
core/rtos/rtos.h
core/rtos/rtos.c
core/rtos/rtos_port.h
core/rtos/example.c
```

## Public API

| Function/type/macro | Exact signature or type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `rtos_init` | `void rtos_init(void);` | init abstraction | none | none | backend init | bare-metal available |
| `rtos_delay_ms` | `void rtos_delay_ms(uint32_t ms);` | delay by ms | ms | none | may block | bare-metal maps to delay |
| `rtos_sleep_ms` | `void rtos_sleep_ms(uint32_t ms);` | sleep by ms | ms | none | may block |  |
| `rtos_sleep_us` | `void rtos_sleep_us(uint32_t us);` | sleep by us | us | none | may block |  |
| `rtos_get_tick_ms` | `uint32_t rtos_get_tick_ms(void);` | get ms tick | none | tick value | none |  |
| `rtos_is_enabled` | `uint8_t rtos_is_enabled(void);` | backend state | none | flag | none |  |
| `rtos_yield` | `void rtos_yield(void);` | yield placeholder | none | none | backend hook | future |
| `rtos_yield_if_needed` | `void rtos_yield_if_needed(void);` | conditional yield | none | none | backend hook | future |
| `rtos_enter_critical` | `void rtos_enter_critical(void);` | enter critical section | none | none | backend hook | future |
| `rtos_exit_critical` | `void rtos_exit_critical(void);` | exit critical section | none | none | backend hook | future |
| `rtos_mutex_lock` | `uint8_t rtos_mutex_lock(void* mutex, uint32_t timeout_ms);` | future mutex API | mutex, timeout | status | placeholder | future |
| `rtos_mutex_unlock` | `void rtos_mutex_unlock(void* mutex);` | future mutex unlock | mutex | none | placeholder | future |
| `rtos_queue_send` | `uint8_t rtos_queue_send(void* queue, const void* item, uint32_t timeout_ms);` | future queue send | queue, item, timeout | status | placeholder | future |
| `rtos_queue_receive` | `uint8_t rtos_queue_receive(void* queue, void* item, uint32_t timeout_ms);` | future queue receive | queue, item, timeout | status | placeholder | future |
