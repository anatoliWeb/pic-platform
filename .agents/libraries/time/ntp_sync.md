# ntp_sync

## Purpose

NTP synchronization helper.

## Location

```text
libraries/time/ntp_sync/ntp_sync.h
libraries/time/ntp_sync/ntp_sync.c
```

## Use when

- you need reusable NTP request/response handling.

## Do not use when

- the project has no network time sync.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `ntp_send_callback_t` | function pointer typedef | send callback | payload | none | callback |  |
| `ntp_receive_callback_t` | function pointer typedef | receive callback | buffer | status | callback |  |
| `ntp_sync_status_t` | enum | status | status values | status | none |  |
| `ntp_sync_config_t` | struct | config | server/timeouts/callbacks | state | none |  |
| `ntp_sync_t` | struct | state | runtime state | state | none |  |
| `ntp_sync_init` | `uint8_t ntp_sync_init(...);` | initialize sync | config | success flag | stores config | exact args in header |
| `ntp_sync_request` | `ntp_sync_status_t ntp_sync_request(...);` | send request | sync state | status | sends packet | exact args in header |
| `ntp_sync_process_response` | `ntp_sync_status_t ntp_sync_process_response(...);` | process response | sync state | status | parses packet | exact args in header |
| `ntp_sync_apply_to_rtc` | `ntp_sync_status_t ntp_sync_apply_to_rtc(...);` | apply to RTC | sync state, rtc | status | updates RTC | exact args in header |
| `ntp_sync_is_valid` | `uint8_t ntp_sync_is_valid(...);` | validity | sync state | flag | none | exact args in header |
| `ntp_sync_get_unix` | `uint8_t ntp_sync_get_unix(...);` | get unix | sync state | unix | none | exact args in header |

## Configuration structures

Public fields are the config and state structs described in the header.

## Error/status model

Sync functions return status enums.

## Source inclusion strategy

### Shared implementation

```text
libraries/time/ntp_sync/ntp_sync.c
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
libraries/time/ntp_sync/ntp_sync.c

For C18 add:
libraries/time/ntp_sync/ntp_sync.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/time/ntp_sync/ntp_sync.h
libraries/time/ntp_sync/ntp_sync.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
```

### Callback-provided dependencies

```text
ntp_send_callback_t, ntp_receive_callback_t
```

### Optional feature dependencies

Not applicable for this module.

### Example-only dependencies

Not applicable for this module.

### Transitive dependencies

Not applicable for this module.

## Compile-time defines

Not applicable for this module.

## Runtime model

Requests and responses are processed synchronously by the caller.

## ISR requirements

Not applicable for this module.

## Resource ownership

Network transport and RTC backend ownership remain with the caller.

## Integration recipe

1. Add the shared NTP sync source.
2. Configure send/receive callbacks.
3. Request sync and apply the result.

## Minimal usable example

```c
ntp_sync_t sync;
ntp_sync_init(&sync, &cfg);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Depends on external network transport.

## Common mistakes

- Treating callbacks as optional when transport is required.

## AI decision rule

Use this module for reusable NTP synchronization.
