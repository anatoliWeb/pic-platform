# ui_events

## Purpose

UI event queue and dispatch helper.

## Location

```text
libraries/ui/ui_events/ui_events.h
libraries/ui/ui_events/ui_events.c
```

## Use when

- you need reusable UI event queuing and dispatch.

## Do not use when

- the project does not use UI events.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `UI_EVENTS_QUEUE_SIZE` | macro | queue size | none | value | none |  |
| `UI_EVENTS_MAX_SUBSCRIBERS` | macro | subscriber count | none | value | none |  |
| `ui_event_type_t` | enum | event type | event values | type | none |  |
| `ui_event_t` | struct | event | type/code/value | state | none |  |
| `ui_event_cb_t` | function pointer typedef | callback | event | none | callback |  |
| `ui_events_t` | struct | state | queue/subscribers | state | none |  |
| `ui_events_init` | `void ui_events_init(...);` | initialize queue | state | none | stores state | exact args in header |
| `ui_events_push` | `void ui_events_push(...);` | enqueue event | events, event | none | mutates queue | exact args in header |
| `ui_events_pop` | `uint8_t ui_events_pop(...);` | dequeue event | events, event | flag | consumes queue | exact args in header |
| `ui_events_peek` | `uint8_t ui_events_peek(...);` | peek event | events, event | flag | none | exact args in header |
| `ui_events_clear` | `void ui_events_clear(...);` | clear queue | events | none | clears queue | exact args in header |
| `ui_events_subscribe` | `uint8_t ui_events_subscribe(...);` | subscribe | events, callback | status | mutates subscribers | exact args in header |
| `ui_events_dispatch` | `void ui_events_dispatch(...);` | dispatch event | events, event | none | invokes callbacks | exact args in header |

## Configuration structures

Public fields are the state structs described in the header.

## Error/status model

Queue helpers return a flag where applicable.

## Source inclusion strategy

### Shared implementation

```text
libraries/ui/ui_events/ui_events.c
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
libraries/ui/ui_events/ui_events.c

For C18 add:
libraries/ui/ui_events/ui_events.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/ui/ui_events/ui_events.h
libraries/ui/ui_events/ui_events.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
```

### Callback-provided dependencies

```text
ui_event_cb_t subscribers
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
| `UI_EVENTS_QUEUE_SIZE` | project-specific | header | queue size | shared source | queue storage | small |
| `UI_EVENTS_MAX_SUBSCRIBERS` | project-specific | header | subscriber count | shared source | subscriber storage | small |

## Runtime model

Events are queued and dispatched synchronously.

## ISR requirements

Not applicable for this module.

## Resource ownership

Queue and subscriber storage are caller-owned.

## Integration recipe

1. Add the shared UI events source.
2. Register subscribers.
3. Push and dispatch events.

## Minimal usable example

```c
ui_events_t events;
ui_events_init(&events);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Queue and subscriber counts are bounded at compile time.

## Common mistakes

- Overflowing the queue.

## AI decision rule

Use this module for reusable UI events.
