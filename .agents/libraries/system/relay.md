# relay

## Purpose

Relay helper.

## Location

```text
libraries/system/relay/relay.h
libraries/system/relay/relay.c
```

## Use when

- you need reusable relay control.

## Do not use when

- another actuator module is required.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `relay_t` | struct | relay state | pin/state/inversion | state | none |  |
| `relay_init` | `uint8_t relay_init(...);` | initialize relay | config | success flag | stores config | exact args in header |
| `relay_on` | `void relay_on(...);` | turn on | relay | none | drives pin | exact args in header |
| `relay_off` | `void relay_off(...);` | turn off | relay | none | drives pin | exact args in header |
| `relay_toggle` | `void relay_toggle(...);` | toggle | relay | none | toggles pin | exact args in header |
| `relay_set_state` | `void relay_set_state(...);` | set state | relay, state | none | drives pin | exact args in header |
| `relay_get_state` | `uint8_t relay_get_state(...);` | read state | relay | flag | none | exact args in header |
| `relay_set_inverted` | `void relay_set_inverted(...);` | set inversion | relay, flag | none | updates polarity | exact args in header |
| `relay_pulse` | `void relay_pulse(...);` | pulse relay | relay, duration | none | starts pulse | exact args in header |
| `relay_delayed_off` | `void relay_delayed_off(...);` | delayed off | relay, duration | none | schedules off | exact args in header |
| `relay_update` | `void relay_update(...);` | progress timers | relay | none | advances state | exact args in header |

## Configuration structures

Public fields are the `relay_t` members described in the header.

## Error/status model

Init returns a success flag.

## Source inclusion strategy

### Shared implementation

```text
libraries/system/relay/relay.c
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
libraries/system/relay/relay.c

For C18 add:
libraries/system/relay/relay.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/system/relay/relay.h
libraries/system/relay/relay.c
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

Relay operations are synchronous and stateful.

## ISR requirements

Not applicable for this module.

## Resource ownership

Relay output pin is project-owned.

## Integration recipe

1. Add the shared relay source.
2. Configure inversion if needed.
3. Drive the relay or schedule pulses.

## Minimal usable example

```c
relay_t relay;
relay_init(&relay, &cfg);
relay_on(&relay);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Timing helpers depend on periodic `relay_update()`.

## Common mistakes

- Forgetting inversion settings.
- Assuming pulse duration is automatic.

## AI decision rule

Use this module for reusable relay control.
