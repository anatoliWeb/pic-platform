[🇺🇦 Ukrainian version](./pwm.ua.md)

# PWM Driver

## Description
PWM driver based on CCP modules and Timer2 time base.

## API
- `pwm_init()`
- `pwm_start()`
- `pwm_stop()`
- `pwm_set_duty()`

## Example
```c
pwm_init(1000u);
pwm_start(PWM_CHANNEL_1);
pwm_set_duty(PWM_CHANNEL_1, 512u);
```

## Notes
- Controls PWM frequency and duty cycle.
- Uses CCP channel selection (`PWM_CHANNEL_1/2`).

## Dependencies
- `core/compiler.h`
- `core/types.h`
- Timer2/CCP hardware path
