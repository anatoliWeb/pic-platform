# buzzer

## Purpose

Buzzer helper.

## Location

```text
libraries/system/buzzer/buzzer.h
libraries/system/buzzer/buzzer.c
```

## Use when

- you need reusable buzzer control and patterns.

## Do not use when

- another actuator module is required.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `buzzer_mode_t` | enum | buzzer mode | `BUZZER_MODE_ACTIVE`, `BUZZER_MODE_PASSIVE` | mode | none |  |
| `buzzer_tone_start_cb_t` | function pointer typedef | tone start callback | freq | none | callback |  |
| `buzzer_tone_stop_cb_t` | function pointer typedef | tone stop callback | none | none | callback |  |
| `buzzer_pattern_step_t` | struct | pattern step | tone/duration | state | none |  |
| `buzzer_t` | struct | buzzer state | runtime state | state | none |  |
| `buzzer_init` | `uint8_t buzzer_init(...);` | initialize buzzer | config | success flag | stores config | exact args in header |
| `buzzer_on` | `void buzzer_on(...);` | enable output | buzzer | none | turns on | exact args in header |
| `buzzer_off` | `void buzzer_off(...);` | disable output | buzzer | none | turns off | exact args in header |
| `buzzer_toggle` | `void buzzer_toggle(...);` | toggle output | buzzer | none | toggles state | exact args in header |
| `buzzer_beep` | `void buzzer_beep(...);` | beep | buzzer, duration | none | emits tone | exact args in header |
| `buzzer_update` | `void buzzer_update(...);` | progress pattern | buzzer | none | advances state | exact args in header |
| `buzzer_set_tone_backend` | `void buzzer_set_tone_backend(...);` | set backend | start/stop callbacks | none | stores backend | exact args in header |
| `buzzer_play_tone` | `void buzzer_play_tone(...);` | play tone | buzzer, freq, duration | none | emits tone | exact args in header |
| `buzzer_play_pattern` | `void buzzer_play_pattern(...);` | play pattern | buzzer, pattern | none | advances pattern | exact args in header |
| `buzzer_stop` | `void buzzer_stop(...);` | stop output | buzzer | none | stops tone | exact args in header |

## Configuration structures

Public fields are the `buzzer_t` and `buzzer_pattern_step_t` members described in the header.

## Error/status model

Init returns a success flag.

## Source inclusion strategy

### Shared implementation

```text
libraries/system/buzzer/buzzer.c
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
libraries/system/buzzer/buzzer.c

For C18 add:
libraries/system/buzzer/buzzer.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/system/buzzer/buzzer.h
libraries/system/buzzer/buzzer.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
```

### Callback-provided dependencies

```text
buzzer_tone_start_cb_t, buzzer_tone_stop_cb_t
```

### Optional feature dependencies

Not applicable for this module.

### Example-only dependencies

Not applicable for this module.

### Transitive dependencies

Not applicable for this module.

## Compile-time defines

Not applicable for this module.

## Runtime model

Pattern updates are driven by periodic calls.

## ISR requirements

Not applicable for this module.

## Resource ownership

Buzzer output pin is project-owned.

## Integration recipe

1. Add the shared buzzer source.
2. Configure the backend callbacks.
3. Initialize and play tones/patterns.

## Minimal usable example

```c
buzzer_t buzzer;
buzzer_init(&buzzer, &cfg);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Pattern behavior depends on the configured backend.

## Common mistakes

- Forgetting to call `buzzer_update()`.
- Using a backend that does not match the hardware.

## AI decision rule

Use this module for reusable buzzer control.
