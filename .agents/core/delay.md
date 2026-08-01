# delay

## Purpose

Blocking delay helpers for milliseconds, microseconds, and cycle-level timing.

## Location

```text
core/delay.h
core/delay.c
```

## Use when

- you need a simple blocking delay in a project or example;
- you need timing-critical bit-bang protocols such as 1-Wire, DHT, IR, or software serial-like pulses;
- you need sub-microsecond cycle delays on PIC18.

## Do not use when

- the code must remain non-blocking;
- the module can use `tick` or an async state machine instead.

## Public API

| Function/type/macro | Exact signature or type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `delay_us` | `void delay_us(uint16_t us);` | blocking microsecond delay | `us` | none | stalls CPU | generic delay |
| `delay_ms` | `void delay_ms(uint16_t ms);` | blocking millisecond delay | `ms` | none | stalls CPU | generic delay |
| `micro_delay_us` | `void micro_delay_us(uint16_t us);` | precise protocol delay | `us` | none | stalls CPU | used by bit-banged protocols |
| `micro_delay_cycles` | `void micro_delay_cycles(uint16_t cycles);` | cycle-accurate short delay | `cycles` | none | stalls CPU | based on instruction cycles |
| `DRV_DELAY_US` | macro | alias to `delay_us` | `us` | none | macro expansion | convenience |
| `DRV_DELAY_MS` | macro | alias to `delay_ms` | `ms` | none | macro expansion | convenience |
| `DRV_MICRO_DELAY_US` | macro | alias to `micro_delay_us` | `us` | none | macro expansion | protocol timing |
| `DRV_MICRO_DELAY_CYCLES` | macro | alias to `micro_delay_cycles` | `cycles` | none | macro expansion | protocol timing |

## Configuration structures

```text
none
```

## Error/status model

```text
none; blocking helpers do not return status
```

## Source inclusion strategy

### Shared implementation

```text
core/delay.c
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
Add core/delay.c once.
Do not add compiler wrapper duplicates.
```

## Required files

```text
core/delay.h
core/delay.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/device.h
```

### Callback-provided dependencies

```text
none
```

### Optional feature dependencies

```text
none
```

### Example-only dependencies

```text
onewire, dht, IR, software bit-bang protocols, example loops
```

### Transitive dependencies

```text
none
```

## Compile-time defines

| Define | Default | Where defined | Effect | Required files | Resource impact | Code-size impact |
| --- | ---: | --- | --- | --- | --- | --- |
| none | n/a | n/a | no feature flags | core/delay.c | CPU blocked while delaying | small/medium |

## Runtime model

- Blocking helper only.
- Use with care in examples and timing-critical bit-bang code.

## ISR requirements

```text
none
```

## Resource ownership

```text
CPU time only
```

## Integration recipe

1. Add `core/delay.c`.
2. Include `core/delay.h` where needed.
3. Keep delays out of non-blocking library paths.

## Minimal usable example

```c
delay_ms(100u);
micro_delay_us(10u);
```

## Working examples

```text
drivers/communication/onewire/example.c
libraries/sensors/ds18b20/example.c
```

## Proteus integration

```text
used indirectly by timing-sensitive examples
```

## HEX artifacts

```text
none
```

## Human documentation

```text
none specific; see protocol docs that rely on delay
```

## Known limitations

- Blocking.
- No scheduler integration.

## Extension points

- If you need non-blocking timing, move the logic into `tick` / state machine code instead.

## Common mistakes

- Using blocking delay inside reusable event loops.
- Replacing protocol timing with arbitrary project-local loops.

## AI decision rule

Use only for blocking delay needs or low-level protocol timing. Do not duplicate these helpers in project code.
