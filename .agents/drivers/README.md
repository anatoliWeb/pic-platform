# Drivers

Driver maps point to low-level reusable modules.

| Need | Next mapper | Path | Decision note |
|---|---|---|---|
| GPIO register helpers | `gpio.md` | `.agents/drivers/gpio.md` | direct register helpers |
| ADC / analog | `analog/README.md` | `.agents/drivers/analog/README.md` | analog peripheral modules |
| Communication | `communication/README.md` | `.agents/drivers/communication/README.md` | serial / bus protocols |
| Interrupt helpers | `interrupts/README.md` | `.agents/drivers/interrupts/README.md` | external interrupt helpers |
| Memory drivers | `memory/README.md` | `.agents/drivers/memory/README.md` | EEPROM-related drivers |
| System drivers | `system/README.md` | `.agents/drivers/system/README.md` | clock/reset/wdt |
| Timers and tick | `timers/README.md` | `.agents/drivers/timers/README.md` | timer backends and timebase |

## Status

Driver docs are short maps with explicit ownership and use cases. They point to library cards when a driver is mainly used as a dependency.
