# screen_manager

## Purpose

Screen stack manager.

## Location

```text
libraries/ui/screen_manager/screen_manager.h
libraries/ui/screen_manager/screen_manager.c
```

## Use when

- you need reusable screen stack management.

## Do not use when

- the project does not use screen stacks.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `SCREEN_MANAGER_STACK_SIZE` | macro | stack size | none | value | none |  |
| `screen_t` | struct | screen state | callbacks | state | none |  |
| `screen_on_enter_cb_t` | function pointer typedef | enter callback | screen | none | callback |  |
| `screen_on_exit_cb_t` | function pointer typedef | exit callback | screen | none | callback |  |
| `screen_on_update_cb_t` | function pointer typedef | update callback | screen | none | callback |  |
| `screen_on_draw_cb_t` | function pointer typedef | draw callback | screen | none | callback |  |
| `screen_on_event_cb_t` | function pointer typedef | event callback | screen,event | none | callback |  |
| `screen_manager_t` | struct | manager state | stack/current | state | none |  |
| `screen_manager_init` | `uint8_t screen_manager_init(...);` | initialize manager | state | success flag | stores stack | exact args in header |
| `screen_manager_set` | `uint8_t screen_manager_set(...);` | set current | manager, screen | status | changes current | exact args in header |
| `screen_manager_push` | `uint8_t screen_manager_push(...);` | push screen | manager, screen | status | mutates stack | exact args in header |
| `screen_manager_pop` | `uint8_t screen_manager_pop(...);` | pop screen | manager | status | mutates stack | exact args in header |
| `screen_manager_update` | `void screen_manager_update(...);` | update current | manager | none | calls current screen | exact args in header |
| `screen_manager_draw` | `void screen_manager_draw(...);` | draw current | manager | none | calls current screen | exact args in header |
| `screen_manager_send_event` | `void screen_manager_send_event(...);` | send event | manager,event | none | forwards event | exact args in header |
| `screen_manager_get_current` | `screen_t* screen_manager_get_current(...);` | current screen | manager | screen ptr | none | exact args in header |

## Configuration structures

Public fields are the state struct described in the header.

## Error/status model

Manager functions return success flags where applicable.

## Source inclusion strategy

### Shared implementation

```text
libraries/ui/screen_manager/screen_manager.c
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
libraries/ui/screen_manager/screen_manager.c

For C18 add:
libraries/ui/screen_manager/screen_manager.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/ui/screen_manager/screen_manager.h
libraries/ui/screen_manager/screen_manager.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
```

### Callback-provided dependencies

```text
screen_on_enter_cb_t, screen_on_exit_cb_t, screen_on_update_cb_t, screen_on_draw_cb_t, screen_on_event_cb_t
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
| `SCREEN_MANAGER_STACK_SIZE` | project-specific | header | stack size | shared source | stack storage | small |

## Runtime model

Screens are updated and drawn synchronously.

## ISR requirements

Not applicable for this module.

## Resource ownership

Screen stack storage is caller-owned.

## Integration recipe

1. Add the shared screen manager source.
2. Define screens and callbacks.
3. Push or set the active screen.

## Minimal usable example

```c
screen_manager_t manager;
screen_manager_init(&manager, screens);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Stack depth is bounded at compile time.

## Common mistakes

- Pushing more screens than the stack allows.

## AI decision rule

Use this module for reusable screen stack management.
