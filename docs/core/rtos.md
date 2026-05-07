[???? Ukrainian version](./rtos.ua.md)

# RTOS Abstraction (Core)

## Overview

`core/rtos` provides a lightweight tasking abstraction for PIC18 projects.
It supports bare-metal mode by default and prepares the codebase for optional
future FreeRTOS integration.

## Public API

- `rtos_init()`
- `rtos_delay_ms(uint32_t ms)`
- `rtos_get_tick_ms()`
- `rtos_is_enabled()`

Optional placeholders:

- `rtos_yield()`
- `rtos_enter_critical()`
- `rtos_exit_critical()`

## Bare-Metal Behavior

- `DRV_USE_FREERTOS` is `0` by default.
- `rtos_delay_ms()` uses existing `delay_ms()`.
- `rtos_get_tick_ms()` uses `tick_get()` when the tick driver is initialized.
- If tick is not running, a local fallback counter is used.

## FreeRTOS Preparation Notes

- If `DRV_USE_FREERTOS` is set to `1`, hooks are ready for future mapping.
- No FreeRTOS headers are included yet.
- No direct dependency on FreeRTOS is required.

## Design Rules

- No dynamic memory
- No task stacks in this layer
- No mutex/queue backend yet (placeholders only)
- Integer-only logic
- PIC18-friendly RAM usage

## Guidance

- PIC18F452 is best suited for cooperative/bare-metal tasking.
- FreeRTOS on low-RAM PIC18 is optional and limited.
- Libraries must use `core/rtos` APIs, not native FreeRTOS calls.

## Files

- `core/rtos/rtos.h`
- `core/rtos/rtos.c`
- `core/rtos/rtos_port.h`
- `core/rtos/example.c`
