# overlay_system

## Purpose

Overlay management helper.

## Location

```text
libraries/ui/overlay_system/overlay_system.h
libraries/ui/overlay_system/overlay_system.c
```

## Use when

- you need reusable overlay stacking and drawing.

## Do not use when

- the project does not render overlays.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `OVERLAY_SYSTEM_MAX_OVERLAYS` | macro | overlay count | none | value | none |  |
| `overlay_t` | struct | overlay state | callbacks/visibility | state | none |  |
| `overlay_on_update_cb_t` | function pointer typedef | update callback | overlay | none | callback |  |
| `overlay_on_draw_cb_t` | function pointer typedef | draw callback | overlay | none | callback |  |
| `overlay_on_event_cb_t` | function pointer typedef | event callback | overlay,event | none | callback |  |
| `overlay_system_t` | struct | system state | overlay list | state | none |  |
| `overlay_system_init` | `uint8_t overlay_system_init(...);` | initialize system | state | success flag | stores state | exact args in header |
| `overlay_add` | `uint8_t overlay_add(...);` | add overlay | system, overlay | status | mutates list | exact args in header |
| `overlay_remove` | `uint8_t overlay_remove(...);` | remove overlay | system, overlay | status | mutates list | exact args in header |
| `overlay_update` | `void overlay_update(...);` | update overlays | system | none | calls update callbacks | exact args in header |
| `overlay_draw` | `void overlay_draw(...);` | draw overlays | system | none | calls draw callbacks | exact args in header |
| `overlay_set_visible` | `uint8_t overlay_set_visible(...);` | set visible | system, overlay, flag | status | changes visibility | exact args in header |
| `overlay_bring_to_front` | `uint8_t overlay_bring_to_front(...);` | reorder overlay | system, overlay | status | mutates order | exact args in header |

## Configuration structures

Public fields are the state structs described in the header.

## Error/status model

Functions return success flags or status values.

## Source inclusion strategy

### Shared implementation

```text
libraries/ui/overlay_system/overlay_system.c
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
libraries/ui/overlay_system/overlay_system.c

For C18 add:
libraries/ui/overlay_system/overlay_system.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/ui/overlay_system/overlay_system.h
libraries/ui/overlay_system/overlay_system.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
```

### Callback-provided dependencies

```text
overlay_on_update_cb_t, overlay_on_draw_cb_t, overlay_on_event_cb_t
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
| `OVERLAY_SYSTEM_MAX_OVERLAYS` | project-specific | header | overlay count | shared source | overlay array size | small |

## Runtime model

Overlays are updated and drawn synchronously.

## ISR requirements

Not applicable for this module.

## Resource ownership

Overlay list state is caller-owned.

## Integration recipe

1. Add the shared overlay system source.
2. Add overlays and callbacks.
3. Update and draw the overlay stack.

## Minimal usable example

```c
overlay_system_t overlays;
overlay_system_init(&overlays, &cfg);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Overlay count is bounded at compile time.

## Common mistakes

- Exceeding overlay capacity.

## AI decision rule

Use this module for reusable overlay stacking.
