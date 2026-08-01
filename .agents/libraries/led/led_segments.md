# led_segments

## Purpose

LED segment mapping helper.

## Location

```text
libraries/led/led_segments/led_segments.h
libraries/led/led_segments/led_segments.c
```

## Use when

- you need reusable segment-to-LED mapping.

## Do not use when

- the project does not use mapped LED segments.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `led_segment_parent_t` | enum | parent type | `LED_SEGMENT_PARENT_STRIP`, `LED_SEGMENT_PARENT_MATRIX_REGION` | parent | none |  |
| `led_segment_config_t` | struct | config | parent mapping | state | none |  |
| `led_segment_t` | struct | segment state | runtime state | state | none |  |
| `led_segment_init` | `uint8_t led_segment_init(...);` | initialize mapping | config | success flag | stores mapping | exact args in header |
| `led_segment_set_pixel` | `uint8_t led_segment_set_pixel(...);` | set segment pixel | segment, x, y, color | status | updates parent | exact args in header |
| `led_segment_fill` | `void led_segment_fill(...);` | fill segment | segment, color | none | updates parent | exact args in header |
| `led_segment_clear` | `void led_segment_clear(...);` | clear segment | segment | none | clears parent region | exact args in header |
| `led_segment_show` | `void led_segment_show(...);` | flush segment | segment | none | updates parent | exact args in header |
| `led_segment_reverse` | `void led_segment_reverse(...);` | reverse mapping | segment, flag | none | updates mapping | exact args in header |
| `led_segment_mirror` | `uint8_t led_segment_mirror(...);` | mirror mapping | segment | status | maps coordinates | exact args in header |

## Configuration structures

Public fields are the config and state structs described in the header.

## Error/status model

Init returns success flag; pixel helpers return status where applicable.

## Source inclusion strategy

### Shared implementation

```text
libraries/led/led_segments/led_segments.c
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
libraries/led/led_segments/led_segments.c

For C18 add:
libraries/led/led_segments/led_segments.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/led/led_segments/led_segments.h
libraries/led/led_segments/led_segments.c
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

Segment coordinates are translated into parent LED operations.

## ISR requirements

Not applicable for this module.

## Resource ownership

Parent LED strip or matrix region is project-owned.

## Integration recipe

1. Add the shared led_segments source.
2. Bind the parent LED target.
3. Draw and flush segment pixels.

## Minimal usable example

```c
led_segment_t seg;
led_segment_init(&seg, &cfg);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Parent target mapping is fixed by config.

## Common mistakes

- Misconfiguring the parent target type.
- Treating the helper as a physical LED driver.

## AI decision rule

Use this helper for reusable LED segment mapping.
