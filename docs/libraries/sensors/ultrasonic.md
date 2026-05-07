# Ultrasonic Sensor Library (HC-SR04 Style)

## Description

`ultrasonic` is a lightweight distance measurement library for HC-SR04-style sensors.
It is designed as a reusable sensor layer and can be extended for compatible modules like JSN-SR04T.

## Public API

- `ultrasonic_init()`
- `ultrasonic_trigger()`
- `ultrasonic_read()`
- `ultrasonic_get_distance_mm()`
- `ultrasonic_get_distance_cm()`
- `ultrasonic_is_valid()`

## Timing Model

Typical measurement sequence:

1. Send trigger pulse (`~10us` HIGH).
2. Wait for echo line to become HIGH.
3. Measure echo HIGH pulse width.
4. Convert pulse width to one-way distance.

Distance conversion used:

`distance_mm ~= echo_us * 343 / 2000`

## Timeout Model

Library protects all waiting loops with configurable timeouts:

- timeout waiting for ECHO HIGH
- timeout waiting for ECHO LOW
- pulse width bounds check

This guarantees the API never blocks forever with disconnected or out-of-range sensors.

## Configuration

- trigger port/tris/pin
- echo port/tris/pin
- `timeout_us`
- `min_pulse_us`
- `max_pulse_us`

No hardcoded pin assumptions.

## Error Handling

`ultrasonic_read()` returns status:

- `ULTRASONIC_STATUS_OK`
- `ULTRASONIC_STATUS_TIMEOUT_WAIT_HIGH`
- `ULTRASONIC_STATUS_TIMEOUT_WAIT_LOW`
- `ULTRASONIC_STATUS_INVALID_PULSE`
- `ULTRASONIC_STATUS_OVERFLOW`
- `ULTRASONIC_STATUS_ERROR`

## Wiring Notes

- TRIG -> MCU output pin
- ECHO -> MCU input pin
- GND common with MCU
- VCC according to module requirements

For long wires/noisy environments, use proper grounding and filtering.

## Range and Limitations

- Practical range depends on sensor model and environment.
- HC-SR04 typical max is around 4m in ideal conditions.
- Soft surfaces and angled targets reduce reliable echo returns.

## Dependencies

- `core/*`
- `drivers/gpio`
- `drivers/timers/*` (typically `tick` for periodic polling in application)
