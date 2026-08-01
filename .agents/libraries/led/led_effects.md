# led_effects

## Purpose

LED effect engine.

## Location

```text
libraries/led/led_effects/led_effects.h
libraries/led/led_effects/led_effects.c
```

## Use when

- you need reusable LED animation effects.

## Do not use when

- the project only needs static colors.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `led_effect_mode_t` | enum | effect type | `LED_EFFECT_MODE_BLINK`, `BREATHING`, `RAINBOW_CYCLE`, `COLOR_WIPE`, `SCANNER`, `PULSE` | mode | none |  |
| `led_effects_config_t` | struct | config | effect parameters | state | none |  |
| `led_effects_t` | struct | effect state | runtime state | state | none |  |
| `led_effects_init` | `uint8_t led_effects_init(...);` | initialize effects | config | success flag | stores config | exact args in header |
| `led_effects_set_mode` | `void led_effects_set_mode(...);` | set effect | state, mode | none | updates mode | exact args in header |
| `led_effects_set_speed` | `void led_effects_set_speed(...);` | set speed | state, speed | none | updates timing | exact args in header |
| `led_effects_set_color` | `void led_effects_set_color(...);` | set color | state, color | none | updates color | exact args in header |
| `led_effects_update` | `void led_effects_update(...);` | advance effect | state | none | updates output | exact args in header |
| `led_effects_is_finished` | `uint8_t led_effects_is_finished(...);` | finished state | state | flag | none | exact args in header |

## Configuration structures

Public fields are the config and state structs described in the header.

## Error/status model

Init returns success flag.

## Source inclusion strategy

### Shared implementation

```text
libraries/led/led_effects/led_effects.c
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
libraries/led/led_effects/led_effects.c

For C18 add:
libraries/led/led_effects/led_effects.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/led/led_effects/led_effects.h
libraries/led/led_effects/led_effects.c
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

Effects update on periodic calls.

## ISR requirements

Not applicable for this module.

## Resource ownership

LED target ownership is caller-defined.

## Integration recipe

1. Add the shared effect engine source.
2. Initialize the effect state.
3. Update regularly.

## Minimal usable example

```c
led_effects_t fx;
led_effects_init(&fx, &cfg);
led_effects_update(&fx);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Effect behavior depends on the target backend.

## Common mistakes

- Expecting hardware timing.
- Updating too slowly for the chosen effect.

## AI decision rule

Use this module for reusable LED effects.
