# IR Receiver Library

## Description

`ir_receiver` is a lightweight, non-blocking IR decode library with initial NEC support.
Its internal design keeps protocol decoding separated from signal capture logic, so more protocols can be added later.

## Public API

- `ir_receiver_init()`
- `ir_receiver_update()`
- `ir_receiver_available()`
- `ir_receiver_read()`
- `ir_receiver_clear()`

## Supported Protocols

- NEC (initial implementation)
- Decoder architecture is prepared for future:
  - RC5
  - Sony SIRC
  - other protocols

## Decoder Architecture Notes

- Input sampling and edge timing are generic.
- Protocol-specific decode logic is selected by `config.protocol`.
- Current NEC decoder uses a falling-edge period model:
  - leader period detection
  - repeat frame period detection
  - 32-bit bitstream decode
  - inversion validation for command bytes

## NEC Timing Overview

Typical NEC frame (demodulated output, active-low marks):

- Leader mark + space
- 32 data bits
- optional repeat frame

Bit classification is done by period ranges with tolerance windows.

## Repeat Frame Handling

Repeat frames are detected and reported via:

- `frame.repeat = 1`
- same protocol marker (`IR_PROTOCOL_NEC`)

## Timeout and Error Protection

- Non-blocking operation: no waiting loops for full frames.
- Incomplete/broken frames auto-reset on timeout.
- Error/status reporting includes:
  - invalid timing
  - incomplete frame
  - timeout reset
  - checksum/inversion mismatch

## Wiring Notes

- Use IR receiver module output pin (demodulated TTL output) to MCU input.
- Typical modules idle HIGH and pull LOW during IR marks.
- Add stable supply decoupling near receiver module.

## Integration Recommendations

- Call `ir_receiver_update()` from:
  - fast main loop
  - periodic scheduler task
  - timer-driven update
- Choose `sample_period_us` small enough for protocol timing resolution.

## Dependencies

- `core/*`
- `drivers/gpio`
- optional timer/interrupt infrastructure in application scheduling path
