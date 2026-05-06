[🇺🇦 Ukrainian version](./wdt.ua.md)

# WDT Helper

## Description
Watchdog helper for periodic clear operation.

## API
- `wdt_clear()`
- `WDT_CLEAR()`

## Example
```c
while (1)
{
    // application logic
    wdt_clear();
}
```

## Notes
- Runtime enable/disable is generally MCU-dependent.
- WDT behavior is primarily defined by config bits.

## Dependencies
- `core/compiler.h`
