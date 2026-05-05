# PIC PLATFORM - Universal Drivers (C18 + XC8)

Universal PIC drivers base (focus: PIC18).
This repository is a reusable drivers library, not an application project.

## Goals

- Reusable and portable drivers
- Support MPLAB C18 and MPLAB XC8
- Keep compiler-specific details in `core/compiler.h`

## Structure

- `core/`
- `drivers/`
- `examples/`
- `C18/`
- `XC8/`

## Compiler Examples

- `examples/common` - universal examples
- `C18/examples` - MPLAB C18 examples
- `XC8/examples` - MPLAB XC8 examples

## Timer Drivers

Timer module includes drivers for:

- Timer0
- Timer1
- Timer2
- Timer3

Each timer exposes a unified API:

- `timerX_init(uint16_t prescaler)`
- `timerX_start()` / `timerX_stop()`
- `timerX_set(uint16_t value)` / `timerX_get()`
- `timerX_enable_interrupt()` / `timerX_disable_interrupt()`
- `timerX_set_callback(void (*cb)(void))`
- `timerX_irq_handler()` for ISR integration

Overflow handling:

- Overflow interrupt flag is cleared in `timerX_irq_handler()`
- Optional callback is called on overflow when set

### Tick System

Global millisecond tick is implemented in:

- `drivers/tick/tick.h`
- `drivers/tick/tick.c`

API:

- `tick_init()`
- `tick_get()`
- `tick_delay(ms)`

Tick uses Timer1 overflow callback and integer-only counter.

### Timer Architecture

Universal entry + compiler-specific override pattern:

- `drivers/timer0/timer0.c` -> `C18`/`XC8` or fallback
- `drivers/timer1/timer1.c` -> `C18`/`XC8` or fallback
- `drivers/timer2/timer2.c` -> `C18`/`XC8` or fallback
- `drivers/timer3/timer3.c` -> `C18`/`XC8` or fallback

## Notes

- No malloc
- No float
- Timer drivers keep timer-only responsibility

## Supported Compilers

- MPLAB C18
- MPLAB XC8
