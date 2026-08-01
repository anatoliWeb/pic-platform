# servo

## Purpose

Servo helper.

## Location

```text
libraries/system/servo/servo.h
libraries/system/servo/servo.c
```

## Use when

- you need reusable servo positioning with a PWM backend.

## Do not use when

- another actuator module is required.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `servo_config_t` | struct | servo config | pulse range/limits | state | none |  |
| `servo_pwm_apply_cb_t` | function pointer typedef | apply PWM callback | pulse | none | callback |  |
| `servo_pwm_enable_cb_t` | function pointer typedef | enable PWM callback | none | none | callback |  |
| `servo_pwm_disable_cb_t` | function pointer typedef | disable PWM callback | none | none | callback |  |
| `servo_t` | struct | servo state | runtime state | state | none |  |
| `servo_init` | `uint8_t servo_init(...);` | initialize servo | config | success flag | stores config | exact args in header |
| `servo_set_backend` | `void servo_set_backend(...);` | install backend | callbacks | none | stores callbacks | exact args in header |
| `servo_enable` | `void servo_enable(...);` | enable output | servo | none | enables PWM | exact args in header |
| `servo_disable` | `void servo_disable(...);` | disable output | servo | none | disables PWM | exact args in header |
| `servo_set_angle` | `void servo_set_angle(...);` | set angle | servo, angle | none | updates target | exact args in header |
| `servo_get_angle` | `uint16_t servo_get_angle(...);` | get angle | servo | angle | none | exact args in header |
| `servo_set_pulse_us` | `void servo_set_pulse_us(...);` | set pulse | servo, pulse | none | updates target | exact args in header |
| `servo_move_smooth` | `void servo_move_smooth(...);` | smooth move | servo, angle, speed | none | updates target | exact args in header |
| `servo_update` | `void servo_update(...);` | progress move | servo | none | advances position | exact args in header |
| `servo_is_moving` | `uint8_t servo_is_moving(...);` | moving state | servo | flag | none | exact args in header |

## Configuration structures

Public fields are the config and state structs described in the header.

## Error/status model

Init returns a success flag.

## Source inclusion strategy

### Shared implementation

```text
libraries/system/servo/servo.c
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
libraries/system/servo/servo.c

For C18 add:
libraries/system/servo/servo.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/system/servo/servo.h
libraries/system/servo/servo.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
```

### Callback-provided dependencies

```text
servo_pwm_apply_cb_t, servo_pwm_enable_cb_t, servo_pwm_disable_cb_t
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

Servo movement updates on periodic calls and backend callbacks.

## ISR requirements

Not applicable for this module.

## Resource ownership

Servo PWM backend is project-owned.

## Integration recipe

1. Add the shared servo source.
2. Install the backend callbacks.
3. Initialize and set angle.
4. Call update periodically.

## Minimal usable example

```c
servo_t servo;
servo_init(&servo, &cfg);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Backend and pulse limits are configuration-dependent.

## Common mistakes

- Forgetting to install backend callbacks.
- Driving beyond configured range.

## AI decision rule

Use this module for reusable servo control.
