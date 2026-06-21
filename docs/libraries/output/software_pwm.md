[🇺🇦 Ukrainian version](./software_pwm.ua.md)

# Software PWM Library

## Description

Software PWM library for multiple GPIO channels driven by a shared timer tick.
It is separate from hardware PWM/CCP and is intended for flexible low-cost output control where one timer drives one PWM group.

## API

- `software_pwm_init_group()`
- `software_pwm_set_duty()`
- `software_pwm_enable_channel()`
- `software_pwm_tick()`

## Architecture

- One `software_pwm_group_t` represents one PWM frequency group.
- One timer source drives one group tick.
- Multiple channels can belong to the same group.
- All channels inside one group share the same base PWM frequency.
- Each channel controls only its duty cycle and enable state.

## Example

```c
static software_pwm_channel_t channels[4] =
{
    { &LATB, &TRISB, (uint8_t)(1u << 0u), 1u, 1u },
    { &LATB, &TRISB, (uint8_t)(1u << 1u), 3u, 1u },
    { &LATB, &TRISB, (uint8_t)(1u << 2u), 6u, 1u },
    { &LATB, &TRISB, (uint8_t)(1u << 3u), 9u, 1u }
};

static software_pwm_group_t group;

software_pwm_init_group(&group, SOFTWARE_PWM_TIMER2, 10u, channels, 4u);
```

## Notes

- `duty = 0` means always OFF.
- `duty >= resolution` means always ON.
- Timer selection is part of the group metadata.
- Initial practical example uses `Timer2` as the tick source.
- `Timer0`, `Timer1`, and `Timer3` remain planned timer-source options and need dedicated example validation.
- Static channel arrays are expected; no dynamic memory is used.
- Channels can be placed on different ports as long as the register pointers are provided.

## Hardware PWM vs Software PWM

- Hardware PWM uses dedicated CCP/PWM peripherals.
- Software PWM toggles GPIO in software from a timer-driven tick.
- Hardware PWM is better for higher frequency and low CPU overhead.
- Software PWM is more flexible for many arbitrary GPIO outputs but is limited by ISR cost and timer rate.

## Performance Notes

- One timer equals one frequency group.
- Higher resolution lowers the effective PWM output frequency for a fixed timer interrupt rate.
- More channels increase ISR workload.
- Very high frequency plus high resolution plus many channels is not suitable for PIC18F452.
- Keep the timer callback short and avoid UART prints inside the PWM tick path.
- On PIC18F452 XC8, route software PWM outputs through `LATx` with the matching `TRISx` bit set as output.
  Do not use `PORTx` as the software PWM output latch because read-modify-write behavior can disturb GPIO updates.

## AC Mains Power Control Note

- Do not treat AC 220V power control as ordinary GPIO PWM.
- Phase-angle control needs a zero-cross detector and a timed gate pulse after zero-cross.
- Zero-cross SSR loads usually use burst-fire or time-proportional control, not ordinary PWM.
- Random-fire triac control is a separate and more safety-critical mode.
- Proper isolation, optocouplers, protection parts, creepage, and mains safety rules are required.
- This repository keeps AC mains control as a separate future module from software PWM.

## Status

Ready for Proteus validation.
