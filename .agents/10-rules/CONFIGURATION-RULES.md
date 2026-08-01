# Configuration Rules

## Project configuration

- `project_config.h` is project-local and should hold board-specific settings, clock values, feature switches, and pin binding.
- `core/config/project_config_template.h` is the template source of truth for new projects.
- `config_bits.c` holds literal configuration bits only.
- `pic_platform_config.h` provides platform defaults and shared feature flags.

## Feature defines

- Use `SEVEN_SEGMENT_ENABLE_TIMER0..3` for seven-segment timer ownership.
- Use `POSITION_DRIVE_SENSOR_TYPE` for the position drive backend selection.
- Use `POSITION_DRIVE_ENABLE_PWM`, `POSITION_DRIVE_ENABLE_TIMEOUT`, `POSITION_DRIVE_ENABLE_STUCK_DETECTION`, `POSITION_DRIVE_ENABLE_DIRECTION_CHECK`, and `POSITION_DRIVE_ENABLE_UART_DEBUG` for position drive behavior.
- Use `DRV_DEBUG_ENABLE` and `DRV_USE_UART` for debug layer gating.

## Placement rules

- Defaults belong in `core/pic_platform_config.h` or `core/config.h`.
- Project overrides belong in `project_config.h` or compiler `-D` flags.
- If a library translation unit does not include `project_config.h`, the value must be visible through build flags or platform defaults.

## Pin binding

- Reusable libraries MUST NOT hardcode project pins.
- Project-specific pin binding belongs in `project_config.h` or application-level config.

## Backend ownership

- Timer ownership must be explicit.
- Do not enable two components that need the same timer slot unless the design proves they do not conflict.
- Keep debug, sensor, and actuator backend selection visible in project configuration.
