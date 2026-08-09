[Ukrainian version](configuration.ua.md)

# Configuration Contract

## Source of truth

Library `.c` files are compiled separately, so they do not see `project_config.h`
unless that header is explicitly included by the same translation unit. Project-wide
values must therefore come from compiler macros (`-D` or MPLAB `define-macros`) or
from platform/module defaults that are already visible to every translation unit.

## Categories

### Category A: project-wide build defines

These values must be visible to all translation units:

- `PIC_PLATFORM_CLOCK_HZ`
- `_XTAL_FREQ` / `DRV_XTAL_FREQ` (derived aliases; prefer `PIC_PLATFORM_CLOCK_HZ`)
- `DRV_DEBUG_ENABLE`
- `DRV_DEBUG_BACKEND_UART`
- `DRV_DEBUG_BACKEND_DISPLAY`
- `DRV_DEBUG_BACKEND_PINS`
- `DRV_DEBUG_LEVEL`
- `DRV_DEBUG_DISPLAY_TYPE_LCD_2X16`
- `DRV_DEBUG_DISPLAY_INTERFACE_I2C`
- `DRV_DEBUG_DISPLAY_INTERFACE_PARALLEL`
- `DRV_DEBUG_DISPLAY_AUTO_INIT`
- `DRV_DEBUG_DISPLAY_I2C_ADDR`
- `DRV_DEBUG_DISPLAY_I2C_FREQ`
- `DRV_DEBUG_PINS_INTERFACE_GPIO`
- `DRV_DEBUG_PINS_INTERFACE_I2C`
- `DRV_DEBUG_PINS_PORT`
- `DRV_DEBUG_PINS_TRIS`
- `DRV_DEBUG_PINS_START_BIT`
- `DRV_DEBUG_PINS_CHANNEL_COUNT`
- `DRV_DEBUG_PINS_PULSE_US`
- `LCD_I2C_PIN_RS`
- `LCD_I2C_PIN_RW`
- `LCD_I2C_PIN_EN`
- `LCD_I2C_PIN_BL`
- `LCD_I2C_DATA_SHIFT`
- `LCD_I2C_MINIMAL` (selects the LCD MINIMAL profile; removes `lcd_i2c_is_ready()`, `lcd_i2c_home()`, `lcd_i2c_write_string()` and more, so it changes the public API surface and must be identical in every TU)
- `SEVEN_SEGMENT_ENABLE_TIMER0`
- `SEVEN_SEGMENT_ENABLE_TIMER1`
- `SEVEN_SEGMENT_ENABLE_TIMER2`
- `SEVEN_SEGMENT_ENABLE_TIMER3`
- `TACHOMETER_LIGHTWEIGHT` (selects the tachometer LIGHTWEIGHT profile; changes `tachometer_t` layout, so it must be identical in every TU)
- `POSITION_DRIVE_SENSOR_TYPE`
- `POSITION_DRIVE_ENABLE_PWM`
- `POSITION_DRIVE_ENABLE_TIMEOUT`
- `POSITION_DRIVE_ENABLE_STUCK_DETECTION`
- `POSITION_DRIVE_ENABLE_DIRECTION_CHECK`
- `POSITION_DRIVE_ENABLE_UART_DEBUG`
- `RB_OVERWRITE` (ring buffer push policy: overwrite vs drop)
- `DRV_USE_FREERTOS` (RTOS abstraction backend selection)
- `DRV_USE_UART` (UART debug gating, combined with `DRV_DEBUG_ENABLE`)
- `DRV_DEBUG_UART_BAUD` (UART debug baud rate)
- `SCHEDULER_MAX_TASKS` (scheduler slot pool size)
- `CRON_MAX_TASKS` (embeds `cron_task_t tasks[CRON_MAX_TASKS]` in `cron_scheduler_t`, so it changes caller-visible struct layout)
- `LED_ANIM_ENGINE_MAX_ANIMATIONS` (embeds `led_animation_t pool[...]` in `led_animation_engine_t`, so it changes caller-visible struct layout)
- `AC_PHASE_CONTROL_MAX_CHANNELS` (channel bounds validation used by `ac_phase_control.c`)

### Category B: runtime arguments

These values are passed into functions and do not need global defines:

- `lcd_i2c_init(0x27u, 100000u)`
- `uart_init(9600u)`

### Category C: CONFIG-bit helper flags

These flags document literal `#pragma config` values in `config_bits.c`:

- `PIC_PLATFORM_WDT_ENABLED`
- `PIC_PLATFORM_LVP_ENABLED`
- `PIC_PLATFORM_BOR_ENABLED`
- `PIC_PLATFORM_CCP2MUX_ENABLED`

## Rules

1. Put safe defaults in `core/pic_platform_config.h` or in the module header.
2. Override project-wide values with compiler `-D` or MPLAB `define-macros`.
3. Do not rely on `project_config.h` as the only source for values needed by library `.c` files.
4. Keep CONFIG bits literal in `config_bits.c`.
5. Keep one source of truth for every category A value.
6. A library `.c` and the application must never see different values of the same define. If the library `.c` uses a macro in `#if` or embeds it in a caller-visible struct, that macro belongs in Category A.

## Official macro table

| Macro | Default | Files | Layout/API effect | Project-wide? |
|---|---|---|---|---|
| `PIC_PLATFORM_CLOCK_HZ` | 8000000UL | `core/device.h` | timing derived (`_XTAL_FREQ`, `DRV_XTAL_FREQ`) | yes |
| `_XTAL_FREQ`, `DRV_XTAL_FREQ` | derived | `core/device.h` | compiler timing, UART/I2C/PWM/tick `#if`/math | yes |
| `DRV_DEBUG_ENABLE` | 1 | `core/config.h` | gates whole debug facade | yes |
| `DRV_USE_UART` | 1 | `core/config.h` | UART debug gating | yes |
| `DRV_DEBUG_LEVEL` | 1 | `libraries/system/debug/debug.h` | debug API macro expansion | yes |
| `DRV_DEBUG_UART_BAUD` | 9600UL | `debug.h`, `debug_backend_uart.c` | init value passed to `uart_init()` | yes |
| `DRV_DEBUG_BACKEND_*`, `DRV_DEBUG_DISPLAY_*`, `DRV_DEBUG_PINS_*` | 0 | `debug.h` | backend compile-out / dispatch | yes |
| `LCD_I2C_PIN_*`, `LCD_I2C_DATA_SHIFT` | PCF8574 defaults | `lcd_i2c.h`, `lcd_i2c.c` | nibble mapping | yes |
| `LCD_I2C_MINIMAL` | 0 | `lcd_i2c.h`, `lcd_i2c.c` | **removes public API** (`lcd_i2c_is_ready`, `lcd_i2c_home`, `lcd_i2c_write_string`, ...) | yes |
| `TACHOMETER_LIGHTWEIGHT` | 0 | `tachometer.h`, `tachometer.c` | **changes `tachometer_t` layout** | yes |
| `SEVEN_SEGMENT_ENABLE_TIMER0..3` | 0 | `seven_segment.c` | timer backend compile-out | yes |
| `POSITION_DRIVE_*` | see config | `position_drive.c` | backend + behavior gates | yes |
| `RB_OVERWRITE` | 0 | `ring_buffer.h`, `ring_buffer.c` | ring buffer push policy | yes |
| `DRV_USE_FREERTOS` | 0 | `rtos_port.h`, `rtos.c` | RTOS abstraction mode | yes |
| `SCHEDULER_MAX_TASKS` | 10u | `scheduler.h`, `scheduler.c` | slot pool size | yes |
| `CRON_MAX_TASKS` | 8u | `cron_scheduler.h` | **embeds array in `cron_scheduler_t`** | yes |
| `LED_ANIM_ENGINE_MAX_ANIMATIONS` | 8u | `led_animation_engine.h` | **embeds array in `led_animation_engine_t`** | yes |
| `AC_PHASE_CONTROL_MAX_CHANNELS` | 4U | `ac_phase_control.h`, `.c` | channel bounds validation | yes |

## Clock

`PIC_PLATFORM_CLOCK_HZ` is the project clock contract. `core/device.h` derives
`DRV_XTAL_FREQ` and `_XTAL_FREQ` from it. Set it project-wide; do not pass
differing `_XTAL_FREQ` values to individual translation units.

```text
-DPIC_PLATFORM_CLOCK_HZ=8000000UL
```

## Debug backends

`DRV_DEBUG_ENABLE` gates the whole debug facade. Backend selection is controlled
by `DRV_DEBUG_BACKEND_UART`, `DRV_DEBUG_BACKEND_DISPLAY`, and
`DRV_DEBUG_BACKEND_PINS`.

## LCD mapping

`lcd_i2c.h` allows backpack pin remapping through `LCD_I2C_PIN_*` and
`LCD_I2C_DATA_SHIFT`. The defaults match the common PCF8574 board.

## Profiles

A profile macro selects a different compiled feature set. It must be set project-wide
so every translation unit compiles the same feature set:

- `TACHOMETER_LIGHTWEIGHT=1` selects the lightweight tachometer profile (changes `tachometer_t` layout).
- `LCD_I2C_MINIMAL=1` selects the LCD MINIMAL profile (removes several public functions).
- `DRV_USE_FREERTOS=1` selects the FreeRTOS RTOS abstraction backend.

```text
-DTACHOMETER_LIGHTWEIGHT=1
-DLCD_I2C_MINIMAL=1
-DDRV_USE_FREERTOS=1
```

Do not define a profile macro inside `project_config.h` or a library header as a
single-TU override: `project_config.h` is not visible to separately compiled
library `.c` files, and a local header define is visible to one TU only.

## MPLAB X

Set project-wide values in `XC8 Global Options -> xc8-cc -> Preprocessing and
messages -> Preprocessor macros`.

Example:

```text
PIC_PLATFORM_CLOCK_HZ=8000000UL
```

## `xc8`

Use the wrapper from `PATH`:

```text
xc8
```
