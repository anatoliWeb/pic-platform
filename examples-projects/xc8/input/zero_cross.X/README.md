# zero_cross.X

Standalone XC8 smoke test for `libraries/input/zero_cross`.

## What it shows

- first edge at timestamp `0`
- `zero_cross_process()` timeout to `LOST`
- first recovery edge after `LOST`
- recovery back to `ALIVE`
- 50 Hz and 60 Hz half-cycle classification

## Files

- `main.c`
- `project_config.h`
- `config_bits.c`
- `nbproject/configurations.xml`
- `nbproject/project.xml`

## Notes

- No triac or relay outputs are used.
- The demo feeds explicit synthetic edges; it does not depend on any hidden zero-cross generator.
