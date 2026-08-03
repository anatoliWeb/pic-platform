# encoder

## Purpose

Phase 1 quadrature encoder driver with direction, delta, and absolute position tracking.

## Location

```text
libraries/input/encoder/encoder.h
libraries/input/encoder/encoder.c
libraries/input/encoder/example.c
XC8/libraries/input/encoder/encoder.c
C18/libraries/input/encoder/encoder.c
docs/libraries/input/encoder.md
docs/libraries/input/encoder.ua.md
examples-projects/xc8/input/encoder.X
examples-projects/proteus/encoder/README.md
```

## Use when

- you need a quadrature encoder with push button support;
- you need delta and absolute position tracking;
- you need the encoder button to reuse `button_t`.

## Do not use when

- you need menu navigation logic that is not part of this driver;
- you only need a raw debounce-only button.

## Public API

| Function/type/macro | Exact signature or type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `encoder_t` | struct | encoder state | port, pins, last_state, delta, position, button | state | holds embedded `button_t` |  |
| `encoder_init` | `void encoder_init(encoder_t* enc, volatile uint8_t* port, volatile uint8_t* tris, uint8_t pin_a, uint8_t pin_b, volatile uint8_t* btn_port, volatile uint8_t* btn_tris, uint8_t btn_pin);` | init encoder | encoder + pin binding | none | configures input pins | button integrated |
| `encoder_tick` | `void encoder_tick(encoder_t* enc);` | poll/update encoder | encoder | none | updates delta/position/button | recommended periodic call |
| `encoder_update` | `void encoder_update(encoder_t* enc);` | compatibility wrapper | encoder | none | same as tick | alias |
| `encoder_get_delta` | `int8_t encoder_get_delta(encoder_t* enc);` | drain one step | encoder | step delta | consumes delta | compatibility behavior |
| `encoder_get_position` | `int16_t encoder_get_position(encoder_t* enc);` | read absolute position | encoder | signed position | none | accumulates steps |
| `encoder_set_position` | `void encoder_set_position(encoder_t* enc, int16_t value);` | set absolute position | encoder, value | none | updates position | |
| `encoder_clicked` | `uint8_t encoder_clicked(encoder_t* enc);` | button click | encoder | flag | consumes button flag | uses embedded button |
| `encoder_double_clicked` | `uint8_t encoder_double_clicked(encoder_t* enc);` | double click | encoder | flag | consumes button flag | uses embedded button |
| `encoder_held` | `uint8_t encoder_held(encoder_t* enc);` | hold event | encoder | flag | consumes button flag | uses embedded button |
| `encoder_get_click_count` | `uint8_t encoder_get_click_count(encoder_t* enc);` | click counter | encoder | count | none | uses embedded button |

## Configuration structures

| Field | Type | Required | Meaning | Valid range | Notes |
| --- | --- | --- | --- | --- | --- |
| `encoder_t.port` | `volatile uint8_t*` | yes | shared port register | register ptr | A/B inputs |
| `encoder_t.tris` | `volatile uint8_t*` | yes | TRIS register | register ptr | input pins |
| `encoder_t.pin_a` | `uint8_t` | yes | phase A pin | bit index |  |
| `encoder_t.pin_b` | `uint8_t` | yes | phase B pin | bit index |  |
| `encoder_t.button` | `button_t` | yes | integrated push button | embedded struct | reuse existing driver |

## Error/status model

```text
No explicit status returns; invalid pointers are ignored in implementation.
```

## Source inclusion strategy

### Shared implementation

```text
libraries/input/encoder/encoder.c
```

### XC8 implementation/wrapper

```text
XC8/libraries/input/encoder/encoder.c
```

### C18 implementation/wrapper

```text
C18/libraries/input/encoder/encoder.c
```

### Select exactly one route

```text
Compile either the shared source or the wrapper source for the selected compiler, not both.
The wrapper .c includes the shared source in this repository.
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
libraries/input/button/button.h
```

### Callback-provided dependencies

```text
none
```

### Optional feature dependencies

```text
the embedded button uses tick timing through the button driver
```

### Example-only dependencies

```text
drivers/timers/tick, uart, uart_debug
```

### Transitive dependencies

```text
core/bit_utils.h
```

## Compile-time defines

| Define | Default | Where defined | Effect | Required files | Resource impact | Code-size impact |
| --- | ---: | --- | --- | --- | --- | --- |
| none module-specific | n/a | n/a | driver always includes button helper | shared/wrapper source | encoder A/B/SW pins | small |

## Runtime model

- Poll with `encoder_tick()` or `encoder_update()`.
- `encoder_get_delta()` drains one step at a time.
- Button events are forwarded from embedded `button_t`.

## ISR requirements

```text
none required; polling example recommended every 1-5 ms
```

## Resource ownership

- Two quadrature input pins and one optional button pin.
- Example uses `RB0/RB1/RB2`.

## Integration recipe

1. Add the shared or wrapper source route for the selected compiler.
2. Bind A/B/SW pins.
3. Call `encoder_tick()` periodically.
4. Read `encoder_get_delta()` or `encoder_get_position()`.
5. Use button helpers for click/hold actions.

## Minimal usable example

```c
encoder_t enc;

encoder_init(&enc, &PORTB, &TRISB, 0u, 1u, &PORTB, &TRISB, 2u);
encoder_tick(&enc);
```

## Working examples

```text
libraries/input/encoder/example.c
examples-projects/xc8/input/encoder.X
```

## Proteus integration

```text
examples-projects/proteus/encoder/README.md
```

## HEX artifacts

```text
examples-projects/hex/xc8/encoder/encoder.hex
examples-projects/hex/c18/encoder/encoder.hex
```

## Human documentation

```text
docs/libraries/input/encoder.md
docs/libraries/input/encoder.ua.md
```

## Known limitations

- Phase 2 menu navigation is intentionally out of scope.

## Extension points

- Reuse the embedded `button_t` rather than adding another button scanner.

## Common mistakes

- Treating `encoder_get_delta()` as a non-draining read.
- Assuming menu logic is included.

## AI decision rule

Use this driver for standalone encoder input. Do not confuse it with the unsupported encoder backend placeholder in `position_drive`.
