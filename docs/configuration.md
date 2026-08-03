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
- `SEVEN_SEGMENT_ENABLE_TIMER0`
- `SEVEN_SEGMENT_ENABLE_TIMER1`
- `SEVEN_SEGMENT_ENABLE_TIMER2`
- `SEVEN_SEGMENT_ENABLE_TIMER3`
- `POSITION_DRIVE_SENSOR_TYPE`
- `POSITION_DRIVE_ENABLE_PWM`
- `POSITION_DRIVE_ENABLE_TIMEOUT`
- `POSITION_DRIVE_ENABLE_STUCK_DETECTION`
- `POSITION_DRIVE_ENABLE_DIRECTION_CHECK`
- `POSITION_DRIVE_ENABLE_UART_DEBUG`

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

## Clock

`PIC_PLATFORM_CLOCK_HZ` is the project clock contract. `core/device.h` derives
`DRV_XTAL_FREQ` and `_XTAL_FREQ` from it.

## Debug backends

`DRV_DEBUG_ENABLE` gates the whole debug facade. Backend selection is controlled
by `DRV_DEBUG_BACKEND_UART`, `DRV_DEBUG_BACKEND_DISPLAY`, and
`DRV_DEBUG_BACKEND_PINS`.

## LCD mapping

`lcd_i2c.h` allows backpack pin remapping through `LCD_I2C_PIN_*` and
`LCD_I2C_DATA_SHIFT`. The defaults match the common PCF8574 board.

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
