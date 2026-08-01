# input_adapter

## Purpose

UI input adapter.

## Location

```text
libraries/ui/input_adapter/input_adapter.h
libraries/ui/input_adapter/input_adapter.c
```

## Use when

- you need reusable UI event adapter logic.

## Do not use when

- the project does not need UI event translation.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `INPUT_ADAPTER_QUEUE_SIZE` | macro | queue size | none | value | none |  |
| `input_event_t` | struct | input event | type/code/value | state | none |  |
| `input_adapter_t` | struct | adapter state | queues/input refs | state | none |  |
| `input_adapter_init` | `uint8_t input_adapter_init(...);` | initialize adapter | state | success flag | stores refs | exact args in header |
| `input_adapter_update` | `void input_adapter_update(...);` | poll input | adapter | none | updates queue | exact args in header |
| `input_adapter_get_event` | `input_event_t input_adapter_get_event(...);` | pop event | adapter | event | consumes queue | exact args in header |
| `input_adapter_clear` | `void input_adapter_clear(...);` | clear queue | adapter | none | clears queue | exact args in header |
| `input_adapter_peek` | `input_event_t input_adapter_peek(...);` | peek event | adapter | event | none | exact args in header |

## Configuration structures

Public fields are the adapter state struct described in the header.

## Error/status model

Init returns a success flag.

## Source inclusion strategy

### Shared implementation

```text
libraries/ui/input_adapter/input_adapter.c
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
libraries/ui/input_adapter/input_adapter.c

For C18 add:
libraries/ui/input_adapter/input_adapter.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/ui/input_adapter/input_adapter.h
libraries/ui/input_adapter/input_adapter.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
```

### Callback-provided dependencies

Not applicable for this module.

### Optional feature dependencies

Not applicable for this module.

### Example-only dependencies

Not applicable for this module.

### Transitive dependencies

Not applicable for this module.

## Compile-time defines

| Define | Default | Where | Effect | Required files | Resource impact | Code-size impact |
| --- | ---: | --- | --- | --- | --- | --- |
| `INPUT_ADAPTER_QUEUE_SIZE` | project-specific | header | queue size | shared source | queue storage | small |

## Runtime model

Input events are queued and consumed synchronously.

## ISR requirements

Not applicable for this module.

## Resource ownership

Queue storage is caller-owned.

## Integration recipe

1. Add the shared input adapter source.
2. Bind input sources.
3. Update and consume events.

## Minimal usable example

```c
input_adapter_t adapter;
input_adapter_init(&adapter, &cfg);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Queue size is bounded at compile time.

## Common mistakes

- Overrunning the event queue.

## AI decision rule

Use this module for reusable UI input adaptation.
