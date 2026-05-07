[🇺🇦 Ukrainian version](./input_adapter.ua.md)

# Input Adapter Library

## Description

`input_adapter` is a reusable input abstraction layer for embedded UI/navigation systems.
It converts hardware-specific input libraries into generic UI events.

The module is independent from display and rendering systems.

## Public API

- `input_adapter_init()`
- `input_adapter_update()`
- `input_adapter_get_event()`
- `input_adapter_clear()` (optional)
- `input_adapter_peek()` (optional)

## Input Event Model

Supported events:

- `INPUT_EVENT_NONE`
- `INPUT_EVENT_UP`
- `INPUT_EVENT_DOWN`
- `INPUT_EVENT_LEFT`
- `INPUT_EVENT_RIGHT`
- `INPUT_EVENT_SELECT`
- `INPUT_EVENT_BACK`
- `INPUT_EVENT_HOLD`

## Supported Input Sources

- `button` library
- `encoder` library (including encoder push button)
- `keypad_matrix` library

Multiple sources can be active simultaneously.

## Buffering / Translation Notes

- Polling-based update model.
- Lightweight fixed-size FIFO queue (`INPUT_ADAPTER_QUEUE_SIZE`).
- Queue overflow policy: drop oldest event.
- Keypad characters are translated through configurable key mapping fields.

Default keypad mapping:

- up: `'2'`
- down: `'8'`
- left: `'4'`
- right: `'6'`
- select: `'#'`
- back: `'*'`

## Event Flow

1. `input_adapter_update()` polls all configured input sources.
2. Source-specific states are translated to `input_event_t`.
3. Events are pushed into queue.
4. Application reads events using `input_adapter_get_event()`.

## Embedded Constraints

- no dynamic memory
- integer-only logic
- small RAM footprint

## Dependencies

- `core/*`
- `libraries/input/button`
- `libraries/input/encoder`
- `libraries/input/keypad_matrix`
