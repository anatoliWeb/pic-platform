# software_pwm

## Purpose

Software PWM library for multiple GPIO channels driven by a shared timer tick.

## Location

```text
libraries/output/software_pwm/software_pwm.h
libraries/output/software_pwm/software_pwm.c
docs/libraries/output/software_pwm.md
docs/libraries/output/software_pwm.ua.md
examples-projects/xc8/output/software_pwm.X
examples-projects/proteus/software_pwm/README.md
```

## Use when

- you need flexible PWM on arbitrary GPIO pins;
- you need one timer-driven group for multiple channels;
- you do not want or cannot use hardware CCP PWM.

## Do not use when

- you need a continuous hardware PWM peripheral;
- you need AC mains phase control, which is a different module;
- you need non-GPIO outputs.

## Public API

| Function/type/macro | Exact signature or type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `software_pwm_timer_t` | enum | timer selection | `TIMER0..3` | timer id | none | group metadata |
| `software_pwm_channel_t` | struct | one PWM channel | port, tris, mask, duty, enabled | state | GPIO config | channels are caller-owned |
| `software_pwm_group_t` | struct | one PWM group | timer, resolution, counter, channels, count | state | group metadata | one timer drives one group |
| `software_pwm_init_group` | `void software_pwm_init_group(software_pwm_group_t* group, software_pwm_timer_t timer, uint16_t resolution, software_pwm_channel_t* channels, uint8_t channel_count);` | initialize a PWM group | group, timer, resolution, channel array, count | none | configures channel directions | |
| `software_pwm_set_duty` | `void software_pwm_set_duty(software_pwm_group_t* group, uint8_t channel_index, uint16_t duty);` | change duty | group, index, duty | none | updates GPIO output | |
| `software_pwm_enable_channel` | `void software_pwm_enable_channel(software_pwm_group_t* group, uint8_t channel_index, uint8_t enabled);` | enable/disable channel | group, index, flag | none | updates GPIO output | |
| `software_pwm_tick` | `void software_pwm_tick(software_pwm_group_t* group);` | advance PWM group | group | none | toggles outputs based on phase | call from timer tick |

## Configuration structures

| Field | Type | Required | Meaning | Valid range | Notes |
| --- | --- | --- | --- | --- | --- |
| `software_pwm_channel_t.port` | `volatile uint8_t*` | yes | output latch | register ptr | GPIO latch register |
| `software_pwm_channel_t.tris` | `volatile uint8_t*` | yes | direction register | register ptr | output direction set in init |
| `software_pwm_channel_t.bit_mask` | `uint8_t` | yes | pin bit | one-hot mask | channel pin |
| `software_pwm_channel_t.duty` | `uint16_t` | yes | duty threshold | `0..resolution` | caller may initialize before init |
| `software_pwm_channel_t.enabled` | `uint8_t` | yes | channel enable | `0/1` |  |
| `software_pwm_group_t.timer` | enum | yes | selected timer | `TIMER0..3` | metadata only |
| `software_pwm_group_t.resolution` | `uint16_t` | yes | PWM resolution | `>0` | `0` is clamped to `1` |
| `software_pwm_group_t.channels` | pointer | yes | channel list | non-null | caller owns array |
| `software_pwm_group_t.channel_count` | `uint8_t` | yes | number of channels | >0 | |

## Error/status model

```text
No explicit status returns; invalid inputs are ignored.
```

## Source inclusion strategy

### Shared implementation

```text
libraries/output/software_pwm/software_pwm.c
```

### XC8 implementation/wrapper

```text
none
```

### C18 implementation/wrapper

```text
none
```

### Select exactly one route

```text
Add the shared .c once. Do not look for or add wrapper duplicates.
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
```

### Callback-provided dependencies

```text
timer tick callback from the application
```

### Optional feature dependencies

```text
none
```

### Example-only dependencies

```text
drivers/timers/timer2/timer2.h or another timer source in the example
```

### Transitive dependencies

```text
none
```

## Compile-time defines

| Define | Default | Where defined | Effect | Required files | Resource impact | Code-size impact |
| --- | ---: | --- | --- | --- | --- | --- |
| none module-specific | n/a | n/a | timer source selected in group metadata | shared source | GPIO pins in channels | small |

## Runtime model

- One group equals one PWM frequency group.
- One timer drives one group tick.
- The application must call `software_pwm_tick()` from the selected timer context.

## ISR requirements

```text
the application must schedule software_pwm_tick() from a timer interrupt or timer-driven loop
```

## Resource ownership

- Output GPIO pins are caller-owned.
- One timer per group.

## Integration recipe

1. Add the shared source file.
2. Define channel arrays and a group.
3. Set TRIS and PORT pointers for each channel.
4. Pick a timer source and call `software_pwm_tick()` at the required rate.
5. Set duty and enable states per channel.

## Minimal usable example

```c
static software_pwm_channel_t channels[4] = {
    { &LATB, &TRISB, (uint8_t)(1u << 0u), 1u, 1u },
    { &LATB, &TRISB, (uint8_t)(1u << 1u), 3u, 1u }
};

static software_pwm_group_t group;

software_pwm_init_group(&group, SOFTWARE_PWM_TIMER2, 10u, channels, 2u);
```

## Working examples

```text
examples-projects/xc8/output/software_pwm.X
```

## Proteus integration

```text
examples-projects/proteus/software_pwm/README.md
```

## Human documentation

```text
docs/libraries/output/software_pwm.md
docs/libraries/output/software_pwm.ua.md
```

## Known limitations

- Higher resolution lowers achievable PWM frequency.
- Many channels increase timer workload.
- Timer0/1/3 examples are still planned according to docs.

## Extension points

- If you need a different gate-control model, use `ac_phase_control` instead of bending this module.

## Common mistakes

- Treating software PWM as hardware PWM.
- Using PORT registers instead of the correct latch register in examples.

## AI decision rule

Use this module for arbitrary GPIO PWM. Do not duplicate PWM groups in project code.
