[???? Ukrainian version](./rtos.ua.md)

# RTOS Abstraction (Core)

## Overview

`core/rtos` is a lightweight delay/timing abstraction for PIC18 projects.
It is bare-metal first and keeps the codebase ready for optional future RTOS use.

## Public API

- `rtos_init()`
- `rtos_delay_ms(uint32_t ms)`
- `rtos_sleep_ms(uint32_t ms)`
- `rtos_sleep_us(uint32_t us)`
- `rtos_get_tick_ms()`
- `rtos_is_enabled()`

Optional helpers:

- `rtos_yield()`
- `rtos_yield_if_needed()`
- `rtos_enter_critical()`
- `rtos_exit_critical()`

## Bare-Metal Delay Behavior

- `DRV_USE_FREERTOS` is `0` by default.
- `rtos_sleep_ms()` uses existing `delay_ms()` in 1 ms steps.
- `rtos_sleep_us()` uses existing `delay_us()`.
- `rtos_delay_ms()` is kept as a compatibility wrapper to `rtos_sleep_ms()`.
- `rtos_get_tick_ms()` uses `tick_get()` when tick driver is active.
- Fallback internal counter is used when no system tick is available.

## Cooperative Scheduling Notes

- Delays are blocking for the current flow.
- `rtos_yield_if_needed()` is called during millisecond sleep steps.
- In bare-metal mode, yield can be connected via `DRV_RTOS_YIELD_HOOK()`
  for scheduler/event pumping.
- Blocking tasks still reduce responsiveness in cooperative systems.

## Future RTOS Compatibility

- If `DRV_USE_FREERTOS = 1`, guarded hooks are ready for future mapping
  to `vTaskDelay()` and `taskYIELD()`.
- No FreeRTOS headers are required now.
- No hard dependency on FreeRTOS is introduced.

## Design Constraints

- No dynamic memory
- Integer-only logic
- No queue/mutex backend in this update
- PIC18F452-friendly RAM and CPU cost

## Migration Guidance

Libraries should gradually migrate from direct delay calls:

- `delay_ms()`
- `DRV_DELAY_MS()`

to RTOS-safe abstraction calls:

- `rtos_sleep_ms()`
- `rtos_sleep_us()`

## Files

- `core/rtos/rtos.h`
- `core/rtos/rtos.c`
- `core/rtos/rtos_port.h`
- `core/rtos/example.c`
