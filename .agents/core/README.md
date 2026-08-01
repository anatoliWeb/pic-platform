# Core

Core maps point to shared helper cards used by drivers and libraries.

| Need | Open | Decision note |
|---|---|---|
| Compiler abstraction | `.agents/core/compiler.md` | use for XC8/C18 branching and interrupt helpers |
| Shared types | `.agents/core/types.md` | use for `drv_status_t` / `drv_bool_t` |
| Compile-time config | `.agents/core/config.md` | use for platform flags and project config template |
| Clock and timebase glue | `.agents/core/timebase.md` | use when modules need `tick` or Timer1 ownership |
| Delay helpers | `.agents/core/delay.md` | use for `delay_ms/us` and protocol-level micro delays |
| Device clock glue | `.agents/core/device.md` | use for `_XTAL_FREQ` / `PIC_PLATFORM_CLOCK_HZ` setup |
| Bit helpers | `.agents/core/bit_utils.md` | use for simple GPIO bit macros |
| Interrupt facade | `.agents/core/interrupts.md` | use for global interrupt control notes |
| CRC helpers | `.agents/core/crc.md` | use for Dallas and Modbus CRC helpers |
| Ring buffer | `.agents/core/ring_buffer.md` | use for fixed-size byte buffers |
| Scheduler | `.agents/core/scheduler.md` | use for cooperative task scheduling |
| RTOS facade | `.agents/core/rtos.md` | use for bare-metal / future RTOS abstraction |

## Status

Core docs are mapper-first. They point to the real code and explain what belongs in `core/` versus a driver or library.
