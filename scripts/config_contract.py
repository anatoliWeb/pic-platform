"""Machine-readable configuration contract for the PIC18F452 / XC8 platform.

Single source of truth for the compile-time configuration categories defined
in docs/configuration.md (Category A / B / C). Consumers import these
constants instead of re-declaring them.

Category A: project-wide build defines (must be visible to all units).
            Includes profile, backend, layout, capacity and timing macros that
            a library .c consumes directly; a per-unit value mismatch silently
            breaks behavior, struct layout, or the public API surface.
Category B: runtime function arguments (no global defines needed).
Category C: CONFIG-bit helper flags documenting literal #pragma config values.
"""

from __future__ import annotations

CATEGORY_A = frozenset({
    "PIC_PLATFORM_CLOCK_HZ",
    "DRV_DEBUG_ENABLE",
    "DRV_DEBUG_BACKEND_UART",
    "DRV_DEBUG_BACKEND_DISPLAY",
    "DRV_DEBUG_BACKEND_PINS",
    "DRV_DEBUG_LEVEL",
    "DRV_DEBUG_DISPLAY_TYPE_LCD_2X16",
    "DRV_DEBUG_DISPLAY_INTERFACE_I2C",
    "DRV_DEBUG_DISPLAY_INTERFACE_PARALLEL",
    "DRV_DEBUG_DISPLAY_AUTO_INIT",
    "DRV_DEBUG_DISPLAY_I2C_ADDR",
    "DRV_DEBUG_DISPLAY_I2C_FREQ",
    "DRV_DEBUG_PINS_INTERFACE_GPIO",
    "DRV_DEBUG_PINS_INTERFACE_I2C",
    "DRV_DEBUG_PINS_PORT",
    "DRV_DEBUG_PINS_TRIS",
    "DRV_DEBUG_PINS_START_BIT",
    "DRV_DEBUG_PINS_CHANNEL_COUNT",
    "DRV_DEBUG_PINS_PULSE_US",
    "LCD_I2C_PIN_RS",
    "LCD_I2C_PIN_RW",
    "LCD_I2C_PIN_EN",
    "LCD_I2C_PIN_BL",
    "LCD_I2C_DATA_SHIFT",
    "LCD_I2C_MINIMAL",
    "SEVEN_SEGMENT_ENABLE_TIMER0",
    "SEVEN_SEGMENT_ENABLE_TIMER1",
    "SEVEN_SEGMENT_ENABLE_TIMER2",
    "SEVEN_SEGMENT_ENABLE_TIMER3",
    "TACHOMETER_LIGHTWEIGHT",
    "POSITION_DRIVE_SENSOR_TYPE",
    "POSITION_DRIVE_ENABLE_PWM",
    "POSITION_DRIVE_ENABLE_TIMEOUT",
    "POSITION_DRIVE_ENABLE_STUCK_DETECTION",
    "POSITION_DRIVE_ENABLE_DIRECTION_CHECK",
    "POSITION_DRIVE_ENABLE_UART_DEBUG",
    "RB_OVERWRITE",
    "DRV_USE_FREERTOS",
    "DRV_USE_UART",
    "DRV_DEBUG_UART_BAUD",
    "SCHEDULER_MAX_TASKS",
    "CRON_MAX_TASKS",
    "LED_ANIM_ENGINE_MAX_ANIMATIONS",
    "AC_PHASE_CONTROL_MAX_CHANNELS",
    "_XTAL_FREQ",
    "DRV_XTAL_FREQ",
})

CATEGORY_B = [
    ("lcd_i2c_init",
     "lcd_i2c_init(0x27u, 100000u)",
     "Initialize LCD on the PCF8574 backpack"),
    ("uart_init",
     "uart_init(9600u)",
     "Initialize UART at 9600 baud"),
]

CATEGORY_C = frozenset({
    "PIC_PLATFORM_WDT_ENABLED",
    "PIC_PLATFORM_LVP_ENABLED",
    "PIC_PLATFORM_BOR_ENABLED",
    "PIC_PLATFORM_CCP2MUX_ENABLED",
})