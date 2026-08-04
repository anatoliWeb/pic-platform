# zero_cross

## Purpose

Reusable AC zero-cross edge detector with glitch rejection, half-cycle measurement, frequency detection (50/60 Hz), timeout, and recovery. It owns no pin, no timer, and no output. The caller feeds edges with a monotonic microsecond time and dispatches the produced event to any number of consumers.

## Location

```text
libraries/input/zero_cross/zero_cross.h
libraries/input/zero_cross/zero_cross.c
libraries/input/zero_cross/example.c
scripts/tests/test_zero_cross.py
scripts/tests/fixtures/zero_cross_harness.c
scripts/tests/fixtures/zero_cross_event_harness.c
docs/libraries/input/zero_cross.md
docs/libraries/input/zero_cross.ua.md
```

No compiler-specific wrappers exist yet. Use the shared implementation directly.

## Use when

- you need a shared sync domain for phase-angle control, burst-fire, energy metering, or diagnostics;
- you want glitch rejection and half-cycle validation in reusable code;
- you want 50/60 Hz detection and timeout/recovery logic exposed once.

## Do not use when

- you only need a raw interrupt flag on a pin;
- you want to bind a specific pin or timer into a library;
- you already own a project-local detector you are not consolidating.

## Public API

| Function/type | Purpose | What to pass | Returns / output | Notes |
| --- | --- | --- | --- | --- |
| `zero_cross_t` | detector state | nothing | struct | caller-owned |
| `zero_cross_config_t` | thresholds | struct | config | see below |
| `zero_cross_event_t` | produced event | nothing | struct | fills `timestamp_us`, `half_cycle_us`, `sequence`, `frequency` |
| `zero_cross_init()` | seed state | zc, config | `drv_status_t` | validates config |
| `zero_cross_on_edge()` | feed an edge | zc, now_us, event | `uint8_t` (1 = event produced) | first edge arms the detector; `LOST` re-arms on the next edge |
| `zero_cross_process()` | advance timeout | zc, now_us | none | sets `LOST` on timeout |
| `zero_cross_is_alive()` | sync healthy check | zc | `uint8_t` | `1` only when `ALIVE` |
| `zero_cross_get_status()` | status query | zc | `zero_cross_status_t` | non-consuming |
| `zero_cross_get_frequency()` | freq query | zc | `zero_cross_frequency_t` | non-consuming |
| `zero_cross_get_half_cycle_us()` | half-cycle query | zc | `uint16_t` | non-consuming |
| `zero_cross_get_sequence()` | event counter | zc | `uint32_t` | non-consuming |
| `zero_cross_reset()` | clear sync | zc | none | back to `WAITING` |

## Configuration define

`zero_cross_config_t` fields:

| Field | Meaning |
| --- | --- |
| `timeout_ms` | time without a valid edge before `LOST`; `0` disables |
| `min_half_cycle_us` / `max_half_cycle_us` | accepted half-cycle window |
| `glitch_reject_us` | minimum accepted interval between edges |
| `recovery_event_count` | valid edges needed to recover from `LOST` |

## Config defines (feature toggles)

| Define | Effect |
| --- | --- |
| `ZERO_CROSS_DISABLE_TIMEOUT` | cuts timeout processing |
| `ZERO_CROSS_DISABLE_FREQUENCY_DETECTION` | cuts 50/60 Hz detection |
| `ZERO_CROSS_DISABLE_GLITCH_FILTER` | cuts glitch rejection |

All features are enabled by default.

## Dependencies

```text
core/types.h
core/compiler.h
```

## Runtime model

- The library never owns a clock; the caller passes `now_us` (monotonic microseconds).
- The first edge only arms the detector; a valid half-cycle needs two edges.
- A valid edge changes status from `WAITING`/`LOST` to `ALIVE` and produces an event (after recovery when re-arming from `LOST`).
- Callers dispatch the produced event to consumers such as `ac_phase_control`.

## Integration recipe

1. Add the `zero_cross` files to the build.
2. Call `zero_cross_init()` once with tuned thresholds.
3. Feed edges from the zero-cross ISR via `zero_cross_on_edge()`.
4. Advance timeout from the main loop via `zero_cross_process()`.
5. Dispatch produced events to consumers; optionally query `zero_cross_is_alive()`.

## Resource ownership

- The zero-cross pin, timer, and output devices are project-owned.
- The library exposes hardware only through a time feed and produced events.
- `armed` is explicit state; `timestamp_us == 0` is valid and not a sentinel.
- `sequence` increases only for events that can be dispatched to consumers.
- Recovery after `LOST` requires fresh stable edges before `ALIVE` returns.

## Standalone example

```text
examples-projects/xc8/input/zero_cross.X
```

## Known limitations

- Requires a monotonic microsecond time source.
- No compiler-specific wrappers yet; shared implementation only.

## AI decision rule

If a task needs zero-cross sync, reuse `zero_cross` rather than duplicating detector logic, and dispatch its event to consumers.
