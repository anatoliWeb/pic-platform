[🇺🇦 Ukrainian version](./reset.ua.md)

# Reset Helper

## Description
Helper for reset cause detection and reset flag maintenance.

## API
- `reset_init()`
- `reset_get_cause()`
- `reset_clear_flags()`

## Example
```c
reset_init();
cause = reset_get_cause();
reset_clear_flags();
```

## Notes
- Provides system-level reset reason reporting.
- Useful for diagnostics after restart.

## Dependencies
- `core/compiler.h`
- `core/types.h`
