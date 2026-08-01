# pwm

## Purpose

Hardware PWM driver.

## Location

```text
drivers/timers/pwm/pwm.h
drivers/timers/pwm/pwm.c
XC8/drivers/timers/pwm/pwm.c
C18/drivers/timers/pwm/pwm.c
```

## Use when

- you need reusable CCP/PWM channel control.

## Do not use when

- the project uses software PWM instead.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `pwm_channel_t` | enum | PWM channel | `PWM_CHANNEL_1`, `PWM_CHANNEL_2` | channel | none |  |
| `pwm_init` | `void pwm_init(uint32_t frequency);` | initialize PWM | frequency | none | configures PWM hardware |  |
| `pwm_start` | `void pwm_start(pwm_channel_t channel);` | start channel | channel | none | enables output |  |
| `pwm_stop` | `void pwm_stop(pwm_channel_t channel);` | stop channel | channel | none | disables output |  |
| `pwm_set_duty` | `void pwm_set_duty(pwm_channel_t channel, uint16_t duty);` | set duty | channel, duty | none | updates duty cycle |  |

## Configuration structures

Not applicable for this module.

## Error/status model

Not applicable for this module.

## Source inclusion strategy

### Shared implementation

```text
drivers/timers/pwm/pwm.c
```

### XC8 implementation/wrapper

```text
XC8/drivers/timers/pwm/pwm.c
```

### C18 implementation/wrapper

```text
C18/drivers/timers/pwm/pwm.c
```

### Select exactly one route

```text
For XC8 add:
XC8/drivers/timers/pwm/pwm.c

For C18 add:
C18/drivers/timers/pwm/pwm.c

Do not add together:
shared pwm.c and the selected wrapper .c

Reason:
the wrapper includes the shared implementation.
```

## Required files

```text
drivers/timers/pwm/pwm.h
drivers/timers/pwm/pwm.c
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

PWM channels are configured synchronously and then driven by hardware.

## ISR requirements

Not applicable for this module.

## Resource ownership

PWM hardware channels are project-owned when selected.

## Integration recipe

1. Add the shared PWM source or the selected wrapper.
2. Initialize frequency.
3. Start the selected channels.
4. Update duty as needed.

## Minimal usable example

```c
pwm_init(1000UL);
pwm_start(PWM_CHANNEL_1);
pwm_set_duty(PWM_CHANNEL_1, 50u);
```

## Working examples

Not applicable for this module.

## Proteus integration

Not applicable for this module.

## HEX artifacts

Not applicable for this module.

## Human documentation

Not applicable for this module.

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Hardware channel availability is device-specific.

## Extension points

Use software PWM when the required output is not available in hardware.

## Common mistakes

- Compiling wrapper and shared source together.
- Mixing hardware PWM and software PWM for the same output.

## AI decision rule

Use this driver for reusable hardware PWM control.
