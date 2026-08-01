# position_drive

## Purpose

Closed-loop DC gear motor position drive.

## Location

```text
libraries/actuator/position_drive/position_drive.h
libraries/actuator/position_drive/position_drive.c
libraries/actuator/position_drive/example.c
XC8/libraries/actuator/position_drive/position_drive.c
C18/libraries/actuator/position_drive/position_drive.c
docs/libraries/actuator/position_drive.md
docs/libraries/actuator/position_drive.ua.md
examples-projects/xc8/actuator/position_drive_adc.X
examples-projects/hex/xc8/actuator/position_drive_adc.X.production.hex
```

## Use when

- you need a DC motor closed-loop controller;
- the feedback sensor is a potentiometer on ADC;
- you need non-blocking motion with timeouts and stuck detection;
- you want hardware access through callbacks instead of direct pin access.

## Do not use when

- you only need raw motor pin toggling;
- you need a different actuator model and do not plan to extend the library;
- you want a PID controller that is not present in the code.

## Public API

| Function/type | Purpose | What to pass | Returns / output | Notes |
| --- | --- | --- | --- | --- |
| `position_drive_state_t` | drive state enum | nothing | `IDLE`, `MOVING`, `TARGET_REACHED`, `ERROR` | state machine output |
| `position_drive_direction_t` | motor direction enum | nothing | `STOP`, `FORWARD`, `REVERSE` | map to H-bridge pins |
| `position_drive_error_t` | error enum | nothing | init/runtime error codes | use for diagnostics |
| `position_drive_read_raw_fn_t` | sensor callback type | context, raw pointer | `drv_status_t` | must fill raw ADC value |
| `position_drive_get_tick_fn_t` | time callback type | context | `uint32_t` ms | used for timeout/stuck detection |
| `position_drive_motor_cb_t` | motor direction callback type | context, direction | none | H-bridge control |
| `position_drive_set_speed_cb_t` | optional speed callback type | context, 0..100 | none | only when PWM is enabled |
| `position_drive_debug_cb_t` | optional debug output callback | context, string | none | callback sink, not direct UART |
| `position_drive_config_t` | runtime/config parameters | callbacks, ranges, limits | struct | validation happens in init |
| `position_drive_t` | runtime state | nothing | struct | stores config and state |
| `position_drive_init()` | validate config, stop motor, read sensor, enter idle | drive, config | `drv_status_t` | required first call |
| `position_drive_process()` | non-blocking control step | drive | none | call regularly from main loop |
| `position_drive_move_to_deg()` | schedule asynchronous move | drive, target degrees | `drv_status_t` | target must be within limits |
| `position_drive_stop()` | manual stop and clear error | drive | `drv_status_t` | returns to idle |
| `position_drive_emergency_stop()` | immediate stop without clearing error | drive | `drv_status_t` | keeps error latched |
| `position_drive_set_speed_percent()` | set speed for moves | drive, percent | `drv_status_t` | only meaningful with PWM |
| `position_drive_get_current_deg()` | current angle | drive | `int16_t` | getter |
| `position_drive_get_target_deg()` | target angle | drive | `int16_t` | getter |
| `position_drive_get_current_raw()` | last raw sensor value | drive | `uint16_t` | getter |
| `position_drive_get_state()` | current state | drive | `position_drive_state_t` | getter |
| `position_drive_get_error()` | latched error | drive | `position_drive_error_t` | getter |
| `position_drive_clear_error()` | clear error and return idle | drive | none | does not restart motor |

## Required files

### Core/library files

```text
libraries/actuator/position_drive/position_drive.h
libraries/actuator/position_drive/position_drive.c
libraries/actuator/position_drive/example.c
```

### XC8 implementation/wrapper

```text
XC8/libraries/actuator/position_drive/position_drive.c
```

### C18 implementation/wrapper

```text
C18/libraries/actuator/position_drive/position_drive.c
```

## Dependencies

```text
core/compiler.h
core/types.h
core/pic_platform_config.h
drivers/timers/tick/tick.h
drivers/analog/adc/adc.h
drivers/gpio/gpio.h
libraries/system/uart_debug/uart_debug.h
```

## Configuration defines

| Define | Default | Where | Effect | Code size impact |
| --- | ---: | --- | --- | --- |
| `POSITION_DRIVE_SENSOR_TYPE` | `POSITION_DRIVE_SENSOR_ADC` | `core/pic_platform_config.h` / `-D` flags | selects backend | backend-dependent |
| `POSITION_DRIVE_ENABLE_PWM` | `0` | `core/pic_platform_config.h` / `-D` flags | includes optional PWM path | off by default |
| `POSITION_DRIVE_ENABLE_TIMEOUT` | `1` | `core/pic_platform_config.h` / `-D` flags | enables move timeout checks | small |
| `POSITION_DRIVE_ENABLE_STUCK_DETECTION` | `1` | `core/pic_platform_config.h` / `-D` flags | enables stuck detection | small |
| `POSITION_DRIVE_ENABLE_DIRECTION_CHECK` | `1` | `core/pic_platform_config.h` / `-D` flags | verifies commanded motion direction | small |
| `POSITION_DRIVE_ENABLE_UART_DEBUG` | `0` | `core/pic_platform_config.h` / `-D` flags | enables debug callback usage | extra debug code |
| `POSITION_DRIVE_DEBUG_LEVEL` | info when debug is enabled | header/build define | controls debug verbosity | higher level = larger code |

## Runtime model

- `position_drive_init()` validates the config, stops the motor, reads the initial sensor value, and enters `IDLE`.
- `position_drive_move_to_deg()` schedules an asynchronous move.
- `position_drive_process()` runs in the main loop and never blocks.
- The ADC backend is implemented.
- The encoder backend is a placeholder and returns `DRV_STATUS_UNSUPPORTED`.
- Debug output is not a direct sink; the application routes it through a callback, commonly to UART.

## Integration recipe

1. Add the core files, actuator library files, and the compiler wrapper for the selected compiler.
2. Add `tick`, `adc`, `gpio`, and `uart_debug` if the example needs them.
3. Define `PIC_PLATFORM_CLOCK_HZ` in `project_config.h`.
4. Bind the sensor read callback to `adc_read()`.
5. Bind the tick callback to `tick_get()`.
6. Bind the motor callback to the H-bridge pins.
7. Configure the ADC and motor pins in the project.
8. Initialize, schedule a move, and call `position_drive_process()` from the main loop.
9. Inspect `position_drive_get_state()` and `position_drive_get_error()` on fault.

## Minimal usage example

```c
position_drive_t drive;

position_drive_init(&drive, &config);
position_drive_move_to_deg(&drive, 90);

while (1)
{
    position_drive_process(&drive);

    if (position_drive_get_state(&drive) == POSITION_DRIVE_STATE_ERROR)
    {
        position_drive_emergency_stop(&drive);
    }
}
```

## Resource ownership

- `RA0/AN0` is used for potentiometer feedback in the example.
- `RD0` and `RD1` drive the H-bridge direction inputs.
- `RD2` is optional enable/PWM.
- `RC6/TX` goes to the Virtual Terminal in the example.
- Timer1 is owned by `tick`.

## Examples

```text
examples-projects/xc8/actuator/position_drive_adc.X
examples-projects/hex/xc8/actuator/position_drive_adc.X.production.hex
examples-projects/proteus/actuator/position_drive_adc/README.md
```

## Human documentation

```text
docs/libraries/actuator/position_drive.md
docs/libraries/actuator/position_drive.ua.md
```

## Known limitations

- Encoder backend is unsupported placeholder.
- PWM speed output is optional and compiled out by default.
- The control model is bang-bang with safety checks, not PID.

## Extension points

- Add or extend sensor backends inside the reusable library.
- Keep project-specific pin binding and debug routing outside the library body.

## AI decision rule

When requested functionality overlaps with this library, MUST use or extend this library instead of creating project-local duplicate code.
### Select exactly one route

```text
For XC8 add:
XC8/libraries/actuator/position_drive/position_drive.c

For C18 add:
C18/libraries/actuator/position_drive/position_drive.c

Do not add together:
libraries/actuator/position_drive/position_drive.c and the selected wrapper .c

Reason:
the wrapper already compiles the shared implementation.
```
