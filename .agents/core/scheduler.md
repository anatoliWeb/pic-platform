# scheduler

## Purpose

Cooperative task scheduler.

## Location

```text
core/scheduler/scheduler.h
core/scheduler/scheduler.c
core/scheduler/example.c
```

## Public API

| Function/type/macro | Exact signature or type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `scheduler_task_cb_t` | `typedef void (*scheduler_task_cb_t)(void* ctx);` | task callback type | context | none | callback |  |
| `scheduler_task_t` | struct | task storage | callback, ctx, interval, last_run, flags | state | none |  |
| `scheduler_init` | `void scheduler_init(void);` | init scheduler | none | none | resets scheduler |  |
| `scheduler_add_task` | `uint8_t scheduler_add_task(scheduler_task_t* task);` | register task | task | success flag | mutates scheduler |  |
| `scheduler_remove_task` | `uint8_t scheduler_remove_task(scheduler_task_t* task);` | unregister task | task | success flag | mutates scheduler |  |
| `scheduler_update` | `void scheduler_update(uint32_t now_ms);` | run due tasks | current ms | none | may call callbacks |  |
| `scheduler_pause_task` | `uint8_t scheduler_pause_task(scheduler_task_t* task);` | pause task | task | success flag | mutates task |  |
| `scheduler_resume_task` | `uint8_t scheduler_resume_task(scheduler_task_t* task);` | resume task | task | success flag | mutates task |  |
| `scheduler_set_interval` | `uint8_t scheduler_set_interval(scheduler_task_t* task, uint32_t interval_ms);` | change period | task, interval | success flag | mutates task |  |
| `scheduler_run_now` | `uint8_t scheduler_run_now(scheduler_task_t* task, uint32_t now_ms);` | immediate run | task, time | success flag | mutates task |  |
