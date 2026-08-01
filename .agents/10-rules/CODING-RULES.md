# Coding Rules

These rules come from the existing code, headers, and examples in this repo.

## Style

- Use lowercase `snake_case` for files and public functions.
- Keep header/source pairs together and name them after the module.
- Use short file headers in the form `File: path`.
- Keep comments short and in English.
- Use `static` for internal helpers.

## Types and names

- Use `drv_status_t` for status returns.
- Use `drv_bool_t` or `bool` where the module already does.
- Use `uint8_t`, `uint16_t`, `uint32_t` for safe integer math.
- Use `int32_t` or `uint32_t` for intermediate arithmetic that can overflow 16-bit math.
- Keep enum names and function names module-scoped.

## API shape

- Prefer a minimal public API.
- Keep configuration in structs where hardware binding or backend selection is needed.
- Use callbacks for hardware binding when the library must stay reusable.
- Use a non-blocking `process()` pattern when the component runs from the main loop.
- Return `DRV_STATUS_UNSUPPORTED` for unsupported backends instead of faking success.

## Compiler compatibility

- Use `core/compiler.h` for compiler abstraction.
- Do not branch on `__XC8` or `__18CXX` in driver code.
- Use `DRV_INT_ENABLE()` and `DRV_INT_DISABLE()` instead of direct compiler checks.
- Keep XC8/C18 differences in wrappers or compiler glue.

## Practical patterns seen in the repo

- `button_update()` handles debounce and events.
- `seven_segment_process()` handles one refresh step in manual mode.
- `position_drive_process()` performs asynchronous control and never blocks.
- `tick_get()` provides a millisecond time base.
- `config_bits.c` holds literal config bits, not reusable logic.
