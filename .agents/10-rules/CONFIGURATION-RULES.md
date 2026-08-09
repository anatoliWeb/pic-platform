# Configuration Rules

## Project configuration

- `project_config.h` is project-local and should hold board-specific notes and helper flags.
- Project-wide values that must be visible to library `.c` files belong in compiler `-D` flags or MPLAB `define-macros`.
- `core/config/project_config_template.h` is the template source of truth for new projects.
- `config_bits.c` holds literal configuration bits only.
- `pic_platform_config.h` provides platform defaults and shared feature flags.

## Category A classification

- A macro belongs to Category A (project-wide build define) when a library `.c` consumes it in `#if`, or when it changes a caller-visible struct layout, or when it compiles out public API functions.
- Known Category A macros are listed in `scripts/config_contract.py` and `docs/configuration.md` / `docs/configuration.ua.md`.
- Category A values must be passed through compiler `-D` or MPLAB `define-macros` only. Never override them inside `project_config.h` or as a local `#define` in a single translation unit.
- A library `.c` and the application must never see different values of the same define. Different per-unit values silently break behavior, struct layout, or the API surface.
- Layout-affecting macros (`TACHOMETER_LIGHTWEIGHT`, `CRON_MAX_TASKS`, `LED_ANIM_ENGINE_MAX_ANIMATIONS`) and API-compiling-out macros (`LCD_I2C_MINIMAL`) are especially dangerous: every translation unit must compile the identical profile.

## Feature defines

- Use `SEVEN_SEGMENT_ENABLE_TIMER0..3` for seven-segment timer ownership.
- Use `POSITION_DRIVE_SENSOR_TYPE` for the position drive backend selection.
- Use `POSITION_DRIVE_ENABLE_PWM`, `POSITION_DRIVE_ENABLE_TIMEOUT`, `POSITION_DRIVE_ENABLE_STUCK_DETECTION`, `POSITION_DRIVE_ENABLE_DIRECTION_CHECK`, and `POSITION_DRIVE_ENABLE_UART_DEBUG` for position drive behavior.
- Use `DRV_DEBUG_ENABLE` and `DRV_USE_UART` for debug layer gating.

## Placement rules

- Defaults belong in `core/pic_platform_config.h`, `core/config.h`, or a module header.
- Project overrides belong in compiler `-D` flags or MPLAB `define-macros`.
- `project_config.h` is not the only source for values required by library `.c` files.

## Pin binding

- Reusable libraries MUST NOT hardcode project pins.
- Project-specific pin binding belongs in `project_config.h` or application-level config.

## Backend ownership

- Timer ownership must be explicit.
- Do not enable two components that need the same timer slot unless the design proves they do not conflict.
- Keep debug, sensor, and actuator backend selection visible in project configuration.
