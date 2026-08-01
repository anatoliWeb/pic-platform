# popup_manager

## Purpose

Popup dialog manager.

## Location

```text
libraries/ui/popup_manager/popup_manager.h
libraries/ui/popup_manager/popup_manager.c
```

## Use when

- you need reusable popup dialog flow.

## Do not use when

- the project does not use popups.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `POPUP_MAX_BUTTONS` | macro | button count | none | value | none |  |
| `POPUP_QUEUE_SIZE` | macro | queue size | none | value | none |  |
| `popup_type_t` | enum | popup type | info/warning/error/confirm/notification | type | none |  |
| `popup_result_t` | enum | result | none/ok/cancel/yes/no/timeout | result | none |  |
| `popup_button_t` | struct | button definition | label/result | state | none |  |
| `popup_t` | struct | popup state | title/message/buttons | state | none |  |
| `popup_manager_t` | struct | manager state | popup queue | state | none |  |
| `popup_manager_init` | `uint8_t popup_manager_init(...);` | initialize manager | state | success flag | stores state | exact args in header |
| `popup_show` | `uint8_t popup_show(...);` | show popup | manager, popup | status | queues popup | exact args in header |
| `popup_close` | `void popup_close(...);` | close popup | manager | none | clears popup | exact args in header |
| `popup_update` | `void popup_update(...);` | update popup | manager | none | advances state | exact args in header |
| `popup_draw` | `void popup_draw(...);` | draw popup | manager | none | draws UI | exact args in header |
| `popup_is_active` | `uint8_t popup_is_active(...);` | active state | manager | flag | none | exact args in header |
| `popup_get_result` | `popup_result_t popup_get_result(...);` | result | manager | result enum | none | exact args in header |

## Configuration structures

Public fields are the state structs described in the header.

## Error/status model

Functions return success flags or result enums.

## Source inclusion strategy

### Shared implementation

```text
libraries/ui/popup_manager/popup_manager.c
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
libraries/ui/popup_manager/popup_manager.c

For C18 add:
libraries/ui/popup_manager/popup_manager.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/ui/popup_manager/popup_manager.h
libraries/ui/popup_manager/popup_manager.c
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
| `POPUP_MAX_BUTTONS` | project-specific | header | button count | shared source | array size | small |
| `POPUP_QUEUE_SIZE` | project-specific | header | popup queue size | shared source | queue storage | small |

## Runtime model

Popups are queued and updated synchronously.

## ISR requirements

Not applicable for this module.

## Resource ownership

Popup queue state is caller-owned.

## Integration recipe

1. Add the shared popup manager source.
2. Queue popups.
3. Update and draw them in the UI loop.

## Minimal usable example

```c
popup_manager_t popup;
popup_manager_init(&popup, &cfg);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Queue capacity is bounded at compile time.

## Common mistakes

- Queueing too many popups.

## AI decision rule

Use this module for reusable popup dialogs.
