# ac_phase_control

## Purpose

Low-voltage multi-channel phase-control group for PIC18F452 with an optional per-channel relay bypass for full-power mode and a zero-cross timeout fail-safe.

## Location

```text
libraries/output/ac_phase_control/ac_phase_control.h
libraries/output/ac_phase_control/ac_phase_control.c
libraries/output/ac_phase_control/example.c
docs/libraries/output/ac_phase_control.md
docs/libraries/output/ac_phase_control.ua.md
examples-projects/xc8/output/ac_phase_control.X
examples-projects/xc8/output/multi_channel_relay.X
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
| `ac_phase_status_t` | enum | group status | OK/NOT_INITIALIZED/CONFIG_ERROR/ZERO_CROSS_LOST | status | none | status enum |
| `ac_phase_relay_state_t` | enum | relay state machine | PHASE/WAIT_ON/HOLD_ON/WAIT_OFF | state | none | relay state enum |
| `ac_phase_control_config_t` | struct | half-cycle timing + relay policy | half_cycle/min_delay/max_delay/gate_pulse + relay thresholds/min times/break-before-make + zero_cross_timeout | config | none | caller config; zero fields use defaults |
| `ac_phase_control_channel_t` | struct | per-channel output state | gate registers, mask, delay, power, flags + relay registers/mask/state | state | GPIO config | caller-owned |
| `ac_phase_control_group_t` | struct | group state | timer, config, channels, count + zero-cross tracking/status | state | group metadata | one shared zero-cross domain |
| `ac_phase_control_init_group` | `drv_status_t ac_phase_control_init_group(ac_phase_control_group_t* group, ac_phase_control_timer_t timer, const ac_phase_control_config_t* config, ac_phase_control_channel_t* channels, uint8_t channel_count);` | initialize group | group, timer, config, channels, count | status | initializes internal state | validates timing + relay thresholds |
| `ac_phase_control_attach_channel` | `drv_status_t ac_phase_control_attach_channel(ac_phase_control_group_t* group, uint8_t channel, volatile uint8_t* gate_lat, volatile uint8_t* gate_tris, uint8_t gate_mask);` | attach gate output | group, index, LAT/TRIS, bitmask | status | configures gate pin | bitmask example `1U << 0U` |
| `ac_phase_control_attach_channel_relay` | `drv_status_t ac_phase_control_attach_channel_relay(ac_phase_control_group_t* group, uint8_t channel, volatile uint8_t* relay_lat, volatile uint8_t* relay_tris, uint8_t relay_mask);` | attach relay bypass | group, index, LAT/TRIS, bitmask | status | configures relay pin | bitmask example `1U << 4U` |
| `ac_phase_control_detach_channel` | `drv_status_t ac_phase_control_detach_channel(ac_phase_control_group_t* group, uint8_t channel);` | detach gate output | group, index | status | clears channel | also releases relay |
| `ac_phase_control_set_power_percent` | `drv_status_t ac_phase_control_set_power_percent(ac_phase_control_group_t* group, uint8_t channel, uint8_t percent);` | set power | group, index, percent | status | updates delay | `0..100` |
| `ac_phase_control_set_delay_us` | `drv_status_t ac_phase_control_set_delay_us(ac_phase_control_group_t* group, uint8_t channel, uint16_t delay_us);` | set delay | group, index, delay | status | updates channel delay |  |
| `ac_phase_control_enable_channel` | `drv_status_t ac_phase_control_enable_channel(ac_phase_control_group_t* group, uint8_t channel, uint8_t enabled);` | enable output | group, index, flag | status | updates channel state |  |
| `ac_phase_control_get_power_percent` | `uint8_t ac_phase_control_get_power_percent(const ac_phase_control_group_t* group, uint8_t channel);` | read power | group, index | percent | none | getter |
| `ac_phase_control_is_channel_enabled` | `uint8_t ac_phase_control_is_channel_enabled(const ac_phase_control_group_t* group, uint8_t channel);` | read enabled state | group, index | flag | none | getter |
| `ac_phase_control_is_channel_in_relay_mode` | `uint8_t ac_phase_control_is_channel_in_relay_mode(const ac_phase_control_group_t* group, uint8_t channel);` | read relay hold state | group, index | flag | none | getter; true when relay is held ON |
| `ac_phase_control_on_zero_cross` | `void ac_phase_control_on_zero_cross(ac_phase_control_group_t* group);` | legacy single-edge entry | group | none | feeds the owned `zero_cross` detector and dispatches its event | legacy wrapper; prefer the event API |
| `ac_phase_control_on_zero_cross_event` | `void ac_phase_control_on_zero_cross_event(ac_phase_control_group_t* group, const zero_cross_event_t* event);` | start half-cycle from a shared zero-cross event | group, event | none | starts new pulse window; requires `zero_cross_is_alive()` | preferred entry; one shared sync domain |
| `ac_phase_control_update_us` | `void ac_phase_control_update_us(ac_phase_control_group_t* group, uint16_t elapsed_us);` | advance elapsed time | group, elapsed | none | updates pulse timing | non-blocking |
| `ac_phase_control_process` | `void ac_phase_control_process(ac_phase_control_group_t* group);` | advance relay/timeout state | group | none | updates relay state; enters ZERO_CROSS_LOST on timeout | call from main loop |
| `ac_phase_control_all_off` | `void ac_phase_control_all_off(ac_phase_control_group_t* group);` | emergency all-off | group | none | disables channels + releases relays |  |
| `ac_phase_control_stop_group` | `void ac_phase_control_stop_group(ac_phase_control_group_t* group);` | stop outputs | group | none | forces outputs low |  |
| `ac_phase_control_irq_handler` | `void ac_phase_control_irq_handler(void);` | timer IRQ dispatch | none | none | timer callback path | example uses Timer2 |
| `ac_phase_control_get_tick_ms` | `uint32_t ac_phase_control_get_tick_ms(const ac_phase_control_group_t* group);` | millisecond tick | group | ms tick | none | helper |
| `ac_phase_control_is_any_channel_active` | `uint8_t ac_phase_control_is_any_channel_active(const ac_phase_control_group_t* group);` | activity flag | group | flag | none | helper |
| `ac_phase_control_get_status` | `ac_phase_status_t ac_phase_control_get_status(const ac_phase_control_group_t* group);` | group status | group | status | none | returns NOT_INITIALIZED when uninitialized |
| `ac_phase_control_is_zero_cross_alive` | `uint8_t ac_phase_control_is_zero_cross_alive(const ac_phase_control_group_t* group);` | zero-cross health | group | flag | none | delegates to the owned `zero_cross` detector |

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
libraries/input/zero_cross/zero_cross.h
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
| `AC_PHASE_CONTROL_DEFAULT_RELAY_ON_THRESHOLD` | `98U` | header | relay engage threshold | shared source | default config | none |
| `AC_PHASE_CONTROL_DEFAULT_RELAY_OFF_THRESHOLD` | `96U` | header | relay release threshold | shared source | default config | none |
| `AC_PHASE_CONTROL_DEFAULT_RELAY_BREAK_MAKE_MS` | `50U` | header | break-before-make | shared source | default config | none |
| `AC_PHASE_CONTROL_DEFAULT_RELAY_MIN_ON_MS` | `200U` | header | min relay ON | shared source | default config | none |
| `AC_PHASE_CONTROL_DEFAULT_RELAY_MIN_OFF_MS` | `200U` | header | min relay OFF | shared source | default config | none |
| `AC_PHASE_CONTROL_ZERO_CROSS_RECOVERY_EVENTS` | `2U` | header | recovery events | shared source | default config | none |
| `AC_PHASE_CONTROL_DEFAULT_GLITCH_REJECT_US` | `500U` | header | zero-cross glitch filter | shared source | default config | none |

## Runtime model

- One group owns a shared `zero_cross` detector instance as its sync domain.
- One shared zero-cross event starts a half-cycle and drives all channels.
- Each enabled channel may produce at most one short gate pulse per half-cycle.
- A channel in phase mode may also carry a relay bypass that takes over at `power_percent >= relay_on_threshold_percent` (default `98%`).
- Relay transitions respect hysteresis (`relay_off_threshold_percent`, default `96%`), break-before-make, and minimum ON/OFF times.
- `ac_phase_control_process()` advances the relay state machine and calls `zero_cross_process()` for the timeout fail-safe (`AC_PHASE_STATUS_ZERO_CROSS_LOST`).
- The group recovers after `AC_PHASE_CONTROL_ZERO_CROSS_RECOVERY_EVENTS` fresh zero-cross events.

## ISR requirements

- Application must call `ac_phase_control_irq_handler()` from its ISR or timer callback path.
- The relay/timeout state machine is advanced from the main loop via `ac_phase_control_process()`, not from the ISR.

## Integration recipe

1. Add the shared source file.
2. Define group and channel arrays.
3. Attach gate pins using LAT/TRIS pointers and a bitmask.
4. Optionally attach relay bypass pins with `ac_phase_control_attach_channel_relay()`.
5. Initialize with Timer2 or the chosen timer enum.
6. Call zero-cross and update functions from the application timing path.
7. Call `ac_phase_control_process()` from the main loop for relay/timeout handling.

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

static const ac_phase_control_config_t g_config = { 10000u, 500u, 9500u, 120u, 98u, 96u, 50u, 200u, 200u, 500u };

(void)ac_phase_control_init_group(&g_group, AC_PHASE_CONTROL_TIMER2, &g_config, g_channels, AC_PHASE_CONTROL_MAX_CHANNELS);
(void)ac_phase_control_attach_channel_relay(&g_group, 0u, &LATD, &TRISD, (uint8_t)(1u << 4u));
```

## Working examples

```text
libraries/output/ac_phase_control/example.c
examples-projects/xc8/output/ac_phase_control.X
examples-projects/xc8/output/multi_channel_relay.X
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
- Only Timer2 is currently implemented (`ac_phase_control_timer_init()` returns `DRV_STATUS_ERROR` for other timer enums).

## Extension points

- If the task is ordinary GPIO PWM, use `software_pwm` instead.
- Other timer drivers can be added inside `ac_phase_control_timer_init()`.
- More channels can be added by raising `AC_PHASE_CONTROL_MAX_CHANNELS` and supplying a larger channel array.

## Common mistakes

- Using this module as if it were a normal duty-cycle PWM library.
- Forgetting that this is zero-cross driven and timer-backed.

## AI decision rule

Use this module for phase-delay control. Do not replace it with project-local timing loops.
