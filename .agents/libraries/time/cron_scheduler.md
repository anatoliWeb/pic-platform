# cron_scheduler

## Purpose

Cron-like scheduler.

## Location

```text
libraries/time/cron_scheduler/cron_scheduler.h
libraries/time/cron_scheduler/cron_scheduler.c
```

## Use when

- you need reusable schedule matching.

## Do not use when

- the project only needs a simple periodic timer.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `CRON_MAX_TASKS` | macro | queue size | none | value | none |  |
| `CRON_ANY` | macro | wildcard | none | value | none |  |
| `cron_callback_t` | function pointer typedef | callback type | scheduled event | none | callback |  |
| `cron_rule_t` | struct | rule state | minute/hour/day/month/dow | state | none |  |
| `cron_task_t` | struct | task state | rule/callback/enabled | state | none |  |
| `cron_scheduler_t` | struct | scheduler state | task list | state | none |  |
| `cron_scheduler_init` | `uint8_t cron_scheduler_init(...);` | initialize scheduler | state | success flag | stores state | exact args in header |
| `cron_scheduler_add` | `int8_t cron_scheduler_add(...);` | add task | scheduler, task | index/status | mutates task list | exact args in header |
| `cron_scheduler_remove` | `uint8_t cron_scheduler_remove(...);` | remove task | scheduler, task | status | mutates task list | exact args in header |
| `cron_scheduler_update` | `void cron_scheduler_update(...);` | evaluate schedule | scheduler, datetime | none | invokes callbacks | exact args in header |
| `cron_scheduler_enable` | `uint8_t cron_scheduler_enable(...);` | enable task | scheduler, task | status | mutates task | exact args in header |
| `cron_scheduler_disable` | `uint8_t cron_scheduler_disable(...);` | disable task | scheduler, task | status | mutates task | exact args in header |

## Configuration structures

Public fields are the config and state structs described in the header.

## Error/status model

Add/remove return status or index results; init returns success flag.

## Source inclusion strategy

### Shared implementation

```text
libraries/time/cron_scheduler/cron_scheduler.c
```

### XC8 implementation/wrapper

```text
Not applicable for this module.
```

### C18 implementation/wrapper

```text
Not applicable for this module.
```

### Select exactly one route

```text
For XC8 add:
libraries/time/cron_scheduler/cron_scheduler.c

For C18 add:
libraries/time/cron_scheduler/cron_scheduler.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/time/cron_scheduler/cron_scheduler.h
libraries/time/cron_scheduler/cron_scheduler.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
```

### Callback-provided dependencies

```text
cron_callback_t callbacks
```

### Optional feature dependencies

Not applicable for this module.

### Example-only dependencies

Not applicable for this module.

### Transitive dependencies

Not applicable for this module.

## Compile-time defines

| Define | Default | Where | Effect | Required files | Resource impact | Code-size impact |
| --- | ---: | --- | --- | --- | --- | --- |
| `CRON_MAX_TASKS` | project-specific | header | task count | shared source | task array size | small |
| `CRON_ANY` | wildcard | header | wildcard field | shared source | none | none |

## Runtime model

Scheduler evaluates rules against the supplied datetime.

## ISR requirements

Not applicable for this module.

## Resource ownership

Task storage is caller-owned.

## Integration recipe

1. Add the shared cron scheduler source.
2. Define rules and tasks.
3. Initialize and update against current time.

## Minimal usable example

```c
cron_scheduler_t scheduler;
cron_scheduler_init(&scheduler);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Task count is bounded at compile time.

## Common mistakes

- Ignoring the wildcard semantics.
- Exceeding the task count.

## AI decision rule

Use this module for cron-like scheduling.
