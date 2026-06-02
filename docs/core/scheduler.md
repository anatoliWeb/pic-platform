[Ukrainian version](./scheduler.ua.md)

# Cooperative Scheduler (Core)

## Overview

`core/scheduler` is a deterministic cooperative task scheduler for bare-metal
PIC18 systems. It runs periodic callbacks from a fixed task pool.

## Public API

- `scheduler_init()`
- `scheduler_add_task()`
- `scheduler_remove_task()`
- `scheduler_update()`
- `scheduler_pause_task()`
- `scheduler_resume_task()`

Optional helpers:

- `scheduler_set_interval()`
- `scheduler_run_now()`

## Task Model

Task callback type:

- `typedef void (*scheduler_task_cb_t)(void* ctx);`

Each task contains:

- callback
- context pointer
- `interval_ms`
- `last_run_ms`
- enabled flag
- run-immediately flag

## Timing / Execution Model

- `scheduler_update(now_ms)` iterates static task slots.
- A task runs when `now_ms - last_run_ms >= interval_ms`.
- `run_immediately` executes task once on next update.
- Scheduler does not block and does not call delay internally.

Time source guidance:

- pass `rtos_get_tick_ms()` to `scheduler_update()` from the main loop.

## Deterministic Storage Model

- fixed-size static pool (`SCHEDULER_MAX_TASKS`)
- no dynamic allocation
- no context switching
- no task stacks

## Limitations

- fully cooperative: callbacks must return quickly
- blocking callbacks reduce responsiveness for all tasks
- no preemptive behavior

## PIC18 Notes

- optimized for PIC18F452-class MCUs
- lightweight integer-only implementation
- suitable for UI polling, sensor reads, communication updates, animation ticks
