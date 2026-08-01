# ir_receiver

## Purpose

Infrared receiver decoder.

## Location

```text
libraries/input/ir_receiver/ir_receiver.h
libraries/input/ir_receiver/ir_receiver.c
```

## Use when

- you need reusable IR frame decoding.

## Do not use when

- the project does not use IR reception.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `ir_protocol_t` | enum | protocol type | `IR_PROTOCOL_UNKNOWN`, `IR_PROTOCOL_NEC` | protocol | none |  |
| `ir_status_t` | enum | status | `IR_STATUS_OK`, `ERROR`, `INVALID_TIMING`, `INCOMPLETE`, `TIMEOUT`, `CHECKSUM` | status | none |  |
| `ir_receiver_config_t` | struct | receiver config | pin/timing/protocol | state | none |  |
| `ir_frame_t` | struct | decoded frame | raw fields | state | none |  |
| `ir_receiver_t` | struct | receiver state | runtime state | state | none |  |
| `ir_receiver_init` | `uint8_t ir_receiver_init(...);` | initialize receiver | config | success flag | stores config | exact args in header |
| `ir_receiver_update` | `void ir_receiver_update(...);` | poll/update | receiver | none | samples input | exact args in header |
| `ir_receiver_available` | `uint8_t ir_receiver_available(...);` | frame ready | receiver | flag | none | exact args in header |
| `ir_receiver_read` | `ir_status_t ir_receiver_read(...);` | read frame | receiver, frame | status | consumes frame | exact args in header |
| `ir_receiver_clear` | `void ir_receiver_clear(...);` | clear state | receiver | none | resets decoder state | exact args in header |

## Configuration structures

Public fields are the config and state structs described in the header.

## Error/status model

`ir_receiver_read()` returns a status enum.

## Source inclusion strategy

### Shared implementation

```text
libraries/input/ir_receiver/ir_receiver.c
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
libraries/input/ir_receiver/ir_receiver.c

For C18 add:
libraries/input/ir_receiver/ir_receiver.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/input/ir_receiver/ir_receiver.h
libraries/input/ir_receiver/ir_receiver.c
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

The decoder is polled until a frame becomes available.

## ISR requirements

Not applicable for this module.

## Resource ownership

IR input pin is project-owned.

## Integration recipe

1. Add the shared IR receiver source.
2. Configure the timing/protocol.
3. Poll update regularly.
4. Read decoded frames when available.

## Minimal usable example

```c
ir_receiver_t ir;
ir_receiver_init(&ir, &cfg);
ir_receiver_update(&ir);
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

Protocol support is limited to the declared public enum.

## Extension points

Keep protocol expansion in reusable decoder code.

## Common mistakes

- Treating the module as interrupt-driven when the API is polled.
- Ignoring timing configuration.

## AI decision rule

Use this module for reusable IR reception and decoding.
