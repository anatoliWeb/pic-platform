# screen_transitions

## Purpose

Screen transition helper.

## Location

```text
libraries/ui/screen_transitions/screen_transitions.h
libraries/ui/screen_transitions/screen_transitions.c
```

## Use when

- you need reusable screen transition effects.

## Do not use when

- the project does not animate screen changes.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `screen_transition_type_t` | enum | transition type | `SCREEN_TRANSITION_FADE`, `SLIDE_LEFT`, `SLIDE_RIGHT`, `SLIDE_UP`, `SLIDE_DOWN` | type | none |  |
| `transition_t` | struct | transition state | progress/duration | state | none |  |
| `screen_transition_manager_t` | struct | manager state | transition state | state | none |  |
| `screen_transition_init` | `uint8_t screen_transition_init(...);` | initialize manager | state | success flag | stores state | exact args in header |
| `screen_transition_start` | `uint8_t screen_transition_start(...);` | start transition | manager, type | status | updates state | exact args in header |
| `screen_transition_update` | `void screen_transition_update(...);` | update transition | manager | none | advances progress | exact args in header |
| `screen_transition_draw` | `void screen_transition_draw(...);` | draw transition | manager | none | renders transition | exact args in header |
| `screen_transition_is_active` | `uint8_t screen_transition_is_active(...);` | active state | manager | flag | none | exact args in header |
| `screen_transition_cancel` | `void screen_transition_cancel(...);` | cancel transition | manager | none | clears state | exact args in header |
| `screen_transition_set_speed` | `void screen_transition_set_speed(...);` | set speed | manager, speed | none | updates timing | exact args in header |

## Configuration structures

Public fields are the state structs described in the header.

## Error/status model

Start returns a success flag or status.

## Source inclusion strategy

### Shared implementation

```text
libraries/ui/screen_transitions/screen_transitions.c
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
libraries/ui/screen_transitions/screen_transitions.c

For C18 add:
libraries/ui/screen_transitions/screen_transitions.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/ui/screen_transitions/screen_transitions.h
libraries/ui/screen_transitions/screen_transitions.c
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

Not applicable for this module.

## Runtime model

Transitions advance on periodic updates.

## ISR requirements

Not applicable for this module.

## Resource ownership

Transition state is caller-owned.

## Integration recipe

1. Add the shared transition source.
2. Initialize and start transitions.
3. Update and draw.

## Minimal usable example

```c
screen_transition_manager_t tr;
screen_transition_init(&tr, &cfg);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Effects depend on the configured transition type.

## Common mistakes

- Updating without starting the transition.

## AI decision rule

Use this module for reusable screen transitions.
