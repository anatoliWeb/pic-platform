[Ukrainian version](./position_drive.ua.md)

# Position Drive Library

## Description

`position_drive` is a non-blocking closed-loop position control library for a DC gear motor with
a position sensor. It commands the motor direction until the measured position reaches the
requested target angle.

The library never touches hardware directly. All hardware access goes through callbacks
supplied by the application, so the same control logic works with any wiring or driver stack.

## Sensor Backends

The sensor backend is selected at compile time through `POSITION_DRIVE_SENSOR_TYPE`:

| Backend          | Status |
|------------------|--------|
| `POSITION_DRIVE_SENSOR_ADC` | implemented |
| `POSITION_DRIVE_SENSOR_ENCODER` | placeholder, `init()` returns `DRV_STATUS_UNSUPPORTED` |
| `POSITION_DRIVE_SENSOR_NONE` | placeholder, `init()` returns `DRV_STATUS_UNSUPPORTED` |

## Public API

- `position_drive_init()`
- `position_drive_process()`
- `position_drive_move_to_deg()`
- `position_drive_stop()`
- `position_drive_emergency_stop()`
- `position_drive_set_speed_percent()`
- `position_drive_get_current_deg()`
- `position_drive_get_target_deg()`
- `position_drive_get_current_raw()`
- `position_drive_get_state()`
- `position_drive_get_error()`
- `position_drive_clear_error()`

## Callbacks

- `position_drive_get_tick_fn_t` - millisecond time source (e.g. `tick_get`)
- `position_drive_read_raw_fn_t` - raw position sensor read
- `position_drive_motor_cb_t` - DC motor direction (STOP/FORWARD/REVERSE)
- `position_drive_set_speed_cb_t` - optional PWM speed, required only when PWM support is enabled
- `position_drive_debug_cb_t` - optional debug output, used only when UART debug is enabled

Every callback receives the same `context` pointer that the application stored in the config.

## Configuration

`position_drive_config_t`:

- sensor raw range (`sensor_raw_min`, `sensor_raw_max`) mapped to angle range
  (`angle_min_deg`, `angle_max_deg`)
- deadband around the target (`target_tolerance_deg`)
- move timeout (`move_timeout_ms`)
- stuck detection (`stuck_timeout_ms`, `stuck_min_delta_raw`)
- polarity flip (`direction_inverted`)
- speed range (`speed_min_percent`, `speed_max_percent`, `speed_default_percent`)

## Control Model

`position_drive_move_to_deg()` schedules an asynchronous move. The application must call
`position_drive_process()` regularly from the main loop. The library:

1. reads the sensor and converts the raw value to degrees (integer math only)
2. applies bang-bang direction control with overshoot correction
3. detects timeouts and mechanical stuck conditions
4. verifies the sensor keeps moving in the commanded direction

On any error the motor is stopped immediately and the drive enters `POSITION_DRIVE_STATE_ERROR`.

## Compile-Time Options

Defaults live in `core/pic_platform_config.h` and can be overridden with compiler `-D` flags:

| Option | Default |
|--------|---------|
| `POSITION_DRIVE_SENSOR_TYPE` | `POSITION_DRIVE_SENSOR_ADC` |
| `POSITION_DRIVE_ENABLE_PWM` | `0` |
| `POSITION_DRIVE_ENABLE_TIMEOUT` | `1` |
| `POSITION_DRIVE_ENABLE_STUCK_DETECTION` | `1` |
| `POSITION_DRIVE_ENABLE_DIRECTION_CHECK` | `1` |
| `POSITION_DRIVE_ENABLE_UART_DEBUG` | `0` |

Override them from the build flags, not from `project_config.h`, so the library translation unit
sees the same value.

## Example

`example.c` demonstrates a move sequence (30 deg -> 120 deg) with a potentiometer sensor and an
H-bridge motor. A complete MPLAB X project lives in
`examples-projects/xc8/actuator/position_drive_adc.X`.

## Safety Notes

- On `position_drive_init()` the motor is forced to STOP before any validation.
- A failed init never leaves the motor running.
- `position_drive_emergency_stop()` stops motor and PWM immediately and preserves the error state
  for inspection.
- `position_drive_clear_error()` leaves the error state but never restarts the motor.

## Dependencies

- `core/*` (compiler, types, platform config)
- optional time source via `drivers/timers/tick`
- optional ADC sensor via `drivers/analog/adc`
- optional PWM speed via `drivers/timers/pwm`

## Embedded Constraints

- No dynamic memory
- Integer-only math
- Small runtime state per instance
