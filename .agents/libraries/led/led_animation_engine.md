# led_animation_engine

## Purpose

LED animation engine.

## Location

```text
libraries/led/led_animation_engine/led_animation_engine.h
libraries/led/led_animation_engine/led_animation_engine.c
```

## Use when

- you need reusable queued LED animations.

## Do not use when

- simple static effects are enough.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `LED_ANIM_ENGINE_MAX_ANIMATIONS` | macro | queue size | none | value | none |  |
| `led_anim_target_t` | enum | target type | target values | state | none |  |
| `led_anim_effect_t` | enum | effect type | effect values | state | none |  |
| `led_animation_config_t` | struct | animation config | timing/target | state | none |  |
| `led_animation_t` | struct | animation state | runtime state | state | none |  |
| `led_animation_engine_t` | struct | engine state | queue/runtime state | state | none |  |
| `led_animation_engine_init` | `void led_animation_engine_init(...);` | initialize engine | config | none | stores config | exact args in header |
| `led_animation_engine_add` | `int8_t led_animation_engine_add(...);` | queue animation | engine, animation | index/status | mutates queue | exact args in header |
| `led_animation_engine_remove` | `uint8_t led_animation_engine_remove(...);` | remove animation | engine, index | status | mutates queue | exact args in header |
| `led_animation_engine_update` | `void led_animation_engine_update(...);` | advance engine | engine | none | updates animations | exact args in header |
| `led_animation_engine_pause` | `void led_animation_engine_pause(...);` | pause engine | engine | none | toggles state | exact args in header |
| `led_animation_engine_resume` | `void led_animation_engine_resume(...);` | resume engine | engine | none | toggles state | exact args in header |
| `led_animation_engine_is_running` | `uint8_t led_animation_engine_is_running(...);` | running state | engine | flag | none | exact args in header |

## Configuration structures

Public fields are the config and state structs described in the header.

## Error/status model

`led_animation_engine_add()` returns an index or negative status.

## Source inclusion strategy

### Shared implementation

```text
libraries/led/led_animation_engine/led_animation_engine.c
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
libraries/led/led_animation_engine/led_animation_engine.c

For C18 add:
libraries/led/led_animation_engine/led_animation_engine.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/led/led_animation_engine/led_animation_engine.h
libraries/led/led_animation_engine/led_animation_engine.c
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
| `LED_ANIM_ENGINE_MAX_ANIMATIONS` | project-specific | header | queue size | shared source | array size | small |

## Runtime model

The engine updates queued animations on periodic calls.

## ISR requirements

Not applicable for this module.

## Resource ownership

Animation targets are caller-owned.

## Integration recipe

1. Add the shared animation engine source.
2. Initialize the engine.
3. Queue animations.
4. Update periodically.

## Minimal usable example

```c
led_animation_engine_t engine;
led_animation_engine_init(&engine, &cfg);
led_animation_engine_update(&engine);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Queue size is bounded at compile time.

## Common mistakes

- Overflowing the animation queue.
- Updating too slowly for active animations.

## AI decision rule

Use this module for reusable LED animation scheduling.
