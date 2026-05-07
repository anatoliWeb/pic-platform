# NTP Synchronization Library

## Description

`ntp_sync` is a compact, transport-independent NTP client utility for embedded systems.
It provides NTP packet creation/parsing and RTC synchronization without implementing sockets.

## Public API

- `ntp_sync_init()`
- `ntp_sync_request()`
- `ntp_sync_process_response()`
- `ntp_sync_apply_to_rtc()`
- `ntp_sync_is_valid()` (optional)
- `ntp_sync_get_unix()` (optional)

## Transport Abstraction Model

User/network layer provides callbacks:

- send callback
- receive callback

The library only exchanges raw NTP UDP payload bytes through these callbacks.
No WiFi/Ethernet stack assumptions are made.

## Packet Parsing Notes

- Request packet size: 48 bytes
- Client mode packet with configurable NTP version
- Response validation includes:
  - minimum size check
  - mode/version check
  - transmit timestamp check
- UNIX extraction:
  - reads transmit timestamp seconds field
  - converts from NTP epoch to UNIX epoch

## RTC and Timezone Integration

- `ntp_sync_apply_to_rtc()` writes synchronized time through `rtc_framework`.
- Optional timezone conversion can be applied via `timezone` library before final RTC set.

## Error Handling

Status codes cover:

- send failure
- no/short response
- invalid mode/version
- invalid timestamp
- RTC update failure

## Embedded Constraints

- No dynamic memory
- Integer-only logic
- Minimal protocol subset (NTP client basics)

## Dependencies

- `core/*`
- `libraries/time/rtc_framework`
- `libraries/time/timezone`
