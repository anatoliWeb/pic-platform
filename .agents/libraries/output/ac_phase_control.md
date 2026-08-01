# ac_phase_control

## Purpose

Low-voltage multi-channel phase-control group for PIC18F452.

## Location

```text
libraries/output/ac_phase_control/ac_phase_control.h
libraries/output/ac_phase_control/ac_phase_control.c
libraries/output/ac_phase_control/example.c
docs/libraries/output/ac_phase_control.md
docs/libraries/output/ac_phase_control.ua.md
examples-projects/xc8/ac_phase_control.X
examples-projects/proteus/ac_phase_control/README.md
```

## Use when

- you need a zero-cross driven phase-delay controller for low-voltage simulation or protected loads;
- you need one half-cycle pulse per channel rather than a continuous PWM waveform.

## Do not use when

- you need ordinary software PWM;
- you are controlling mains hardware directly without the documented protection/safety design;
- you need a generic output helper without zero-cross timing.

## Public API

| Function/type/macro | Exact signature or type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `ac_phase_control_timer_t` | enum | timer selection | `TIMER0..3` | timer id | none | example validates Timer2 |
| `ac_phase_control_config_t` | struct | half-cycle timing | half_cycle/min_delay/max_delay/gate_pulse | config | none | caller config |
| `ac_phase_control_channel_t` | struct | per-channel output state | gate registers, mask, delay, power, flags | state | GPIO config | caller-owned |
| `ac_phase_control_group_t` | struct | group state | timer, config, channels, count | state | group metadata | one shared zero-cross domain |
| `ac_phase_control_init_group` | `drv_status_t ac_phase_control_init_group(ac_phase_control_group_t* group, ac_phase_control_timer_t timer, const ac_phase_control_config_t* config, ac_phase_control_channel_t* channels, uint8_t channel_count);` | initialize group | group, timer, config, channels, count | status | initializes internal state |  |
| `ac_phase_control_attach_channel` | `drv_status_t ac_phase_control_attach_channel(ac_phase_control_group_t* group, uint8_t channel, volatile uint8_t* gate_lat, volatile uint8_t* gate_tris, uint8_t gate_mask);` | attach gate output | group, index, LAT/TRIS, bitmask | status | configures gate pin | bitmask example `1U << 0U` |
| `ac_phase_control_detach_channel` | `drv_status_t ac_phase_control_detach_channel(ac_phase_control_group_t* group, uint8_t channel);` | detach gate output | group, index | status | clears channel |  |
| `ac_phase_control_set_power_percent` | `drv_status_t ac_phase_control_set_power_percent(ac_phase_control_group_t* group, uint8_t channel, uint8_t percent);` | set power | group, index, percent | status | updates delay | `0..100` |
| `ac_phase_control_set_delay_us` | `drv_status_t ac_phase_control_set_delay_us(ac_phase_control_group_t* group, uint8_t channel, uint16_t delay_us);` | set delay | group, index, delay | status | updates channel delay |  |
| `ac_phase_control_enable_channel` | `drv_status_t ac_phase_control_enable_channel(ac_phase_control_group_t* group, uint8_t channel, uint8_t enabled);` | enable output | group, index, flag | status | updates channel state |  |
| `ac_phase_control_get_power_percent` | `uint8_t ac_phase_control_get_power_percent(const ac_phase_control_group_t* group, uint8_t channel);` | read power | group, index | percent | none | getter |
| `ac_phase_control_is_channel_enabled` | `uint8_t ac_phase_control_is_channel_enabled(const ac_phase_control_group_t* group, uint8_t channel);` | read enabled state | group, index | flag | none | getter |
| `ac_phase_control_on_zero_cross` | `void ac_phase_control_on_zero_cross(ac_phase_control_group_t* group);` | start half-cycle | group | none | starts new pulse window | zero-cross ISR/event |
| `ac_phase_control_update_us` | `void ac_phase_control_update_us(ac_phase_control_group_t* group, uint16_t elapsed_us);` | advance elapsed time | group, elapsed | none | updates pulse timing | non-blocking |
| `ac_phase_control_stop_group` | `void ac_phase_control_stop_group(ac_phase_control_group_t* group);` | stop outputs | group | none | forces outputs low |  |
| `ac_phase_control_irq_handler` | `void ac_phase_control_irq_handler(void);` | timer IRQ dispatch | none | none | timer callback path | example uses Timer2 |
| `ac_phase_control_get_tick_ms` | `uint32_t ac_phase_control_get_tick_ms(const ac_phase_control_group_t* group);` | millisecond tick | group | ms tick | none | helper |
| `ac_phase_control_is_any_channel_active` | `uint8_t ac_phase_control_is_any_channel_active(const ac_phase_control_group_t* group);` | activity flag | group | flag | none | helper |

## Source inclusion strategy

### Shared implementation

```text
libraries/output/ac_phase_control/ac_phase_control.c
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
Add the shared source once. Do not search for wrapper duplicates.
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
```

### Callback-provided dependencies

```text
zero-cross event source from application
timer interrupt dispatch from application
```

### Optional feature dependencies

```text
none
```

### Example-only dependencies

```text
timer2 validation, diagnostic pins, Proteus wiring, example app state
```

### Transitive dependencies

```text
none
```

## Compile-time defines

| Define | Default | Where defined | Effect | Required files | Resource impact | Code-size impact |
| --- | ---: | --- | --- | --- | --- | --- |
| `AC_PHASE_CONTROL_MAX_CHANNELS` | `4U` | header | max channels | shared source | array sizing | small |

## Runtime model

- One group owns shared timing and the channel array.
- One zero-cross event starts a half-cycle.
- Each enabled channel may produce at most one short gate pulse per half-cycle.

## ISR requirements

- Application must call `ac_phase_control_irq_handler()` from its ISR or timer callback path.

## Resource ownership

- Gate pins are project-bound.
- Timer2 is the validated default path in docs/examples.

## Integration recipe

1. Add the shared source file.
2. Define group and channel arrays.
3. Attach gate pins using LAT/TRIS pointers and a bitmask.
4. Initialize with Timer2 or the chosen timer enum.
5. Call zero-cross and update functions from the application timing path.

## Minimal usable example

```c
static ac_phase_control_group_t g_group;
static ac_phase_control_channel_t g_channels[AC_PHASE_CONTROL_MAX_CHANNELS];

static const ac_phase_control_config_t g_config = { 10000u, 500u, 9500u, 120u };

(void)ac_phase_control_init_group(&g_group, AC_PHASE_CONTROL_TIMER2, &g_config, g_channels, AC_PHASE_CONTROL_MAX_CHANNELS);
```

## Working examples

```text
libraries/output/ac_phase_control/example.c
examples-projects/xc8/ac_phase_control.X
examples-projects/proteus/ac_phase_control/README.md
```

## Proteus integration

```text
examples-projects/proteus/ac_phase_control/README.md
```

## HEX artifacts

```text
examples-projects/hex/xc8/ac_phase_control/ac_phase_control.hex
```

## Human documentation

```text
docs/libraries/output/ac_phase_control.md
docs/libraries/output/ac_phase_control.ua.md
```

## Known limitations

- Multi-channel Proteus validation is still pending in docs.
- Timer0/1/3 are reserved for future work in the docs.

## Extension points

- If the task is ordinary GPIO PWM, use `software_pwm` instead.

## Common mistakes

- Using this module as if it were a normal duty-cycle PWM library.
- Forgetting that this is zero-cross driven and timer-backed.

## AI decision rule

Use this module for phase-delay control. Do not replace it with project-local timing loops.
