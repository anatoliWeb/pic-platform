[🇺🇦 Ukrainian version](./servo.ua.md)

# Servo Control Library

## Description

`servo` is a lightweight reusable servo motor control library for embedded systems.
It supports direct angle positioning and smooth asynchronous movement without blocking delays.

## Public API

- `servo_init()`
- `servo_set_backend()`
- `servo_enable()`
- `servo_disable()`
- `servo_set_angle()`
- `servo_get_angle()`
- `servo_set_pulse_us()`
- `servo_move_smooth()` (optional)
- `servo_update()` (optional)
- `servo_is_moving()` (optional)

## Servo Model

Configurable parameters:

- pulse range (`pulse_min_us`, `pulse_max_us`)
- angle range (`angle_min_deg`, `angle_max_deg`)
- PWM period (`period_us`)

Default hobby-servo style values:

- 1000 us -> 0 deg
- 1500 us -> center
- 2000 us -> 180 deg
- 20 ms period

## Movement Model

`servo_move_smooth()` schedules target tracking with step timing:

- target angle
- speed (deg per step)
- step interval (ms)

`servo_update(now_ms)` performs non-blocking incremental movement toward target.
No `delay_ms()` is used.

## PWM Integration Notes

The library uses backend callbacks, not hardware-specific code:

- `servo_pwm_apply_cb_t`
- `servo_pwm_enable_cb_t`
- `servo_pwm_disable_cb_t`

This keeps the library portable and allows integration with existing PWM driver abstractions.

## Example

`example.c` demonstrates:

- direct positioning
- smooth movement
- non-blocking sweep
- two servo instances

## Power and Safety Notes

- Servos can draw high current spikes.
- Use a stable external power source for servo motors.
- Always connect MCU and servo grounds together.

## Dependencies

- `core/*`
- optional PWM backend via `drivers/timers/pwm`
- optional timing source via `drivers/timers/tick`

## Embedded Constraints

- No dynamic memory
- Integer-only math
- Small runtime state per instance
