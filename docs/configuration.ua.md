[English version](configuration.md)

# Kontseptsiia konfiguratsii

## Dzherelo istyny

Okremi library `.c` kompiliuiutsia yak nezalezhni translation units, tomu vony ne
bachat `project_config.h`, yakshcho tsii header ne pidkliucheno v tomu samomu TU.
Project-wide znachennia maialy prykhodyty z compiler macros (`-D` abo MPLAB
`define-macros`) abo z platform/module defaults, yaki vydni vsim TU.

## Kategorie

### Category A: project-wide build defines

TsI znachennia maibut vydni vsim translation units:

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

TsI znachennia peredaiutsia u funktsii i ne potribuiut global defines:

- `lcd_i2c_init(0x27u, 100000u)`
- `uart_init(9600u)`

### Category C: CONFIG-bit helper flags

TsI praporci dokumentuiut literal `#pragma config` znachennia u `config_bits.c`:

- `PIC_PLATFORM_WDT_ENABLED`
- `PIC_PLATFORM_LVP_ENABLED`
- `PIC_PLATFORM_BOR_ENABLED`
- `PIC_PLATFORM_CCP2MUX_ENABLED`

## Pravyla

1. Safe defaults zberihai u `core/pic_platform_config.h` abo module header.
2. Project-wide znachennia perevyznachai cherez compiler `-D` abo MPLAB `define-macros`.
3. Ne poklaiis na `project_config.h` yak na odyne dzherelo znachen dlia library `.c`.
4. CONFIG bits trymay literal u `config_bits.c`.
5. Dlia kozhnoho category A znachennia maie buty odne dzherelo istyny: one source of truth.

## Clock

`PIC_PLATFORM_CLOCK_HZ` is the project clock contract. `core/device.h` derives
`DRV_XTAL_FREQ` and `_XTAL_FREQ` from it.

## Debug backendy

`DRV_DEBUG_ENABLE` disables the whole debug facade. Backend selection is controlled
by `DRV_DEBUG_BACKEND_UART`, `DRV_DEBUG_BACKEND_DISPLAY`, and `DRV_DEBUG_BACKEND_PINS`.

## LCD mapping

`lcd_i2c.h` allows backpack pin remapping through `LCD_I2C_PIN_*` and
`LCD_I2C_DATA_SHIFT`. The defaults match the common PCF8574 board.

## MPLAB X

Set project-wide values here:

`XC8 Global Options -> xc8-cc -> Preprocessing and messages -> Preprocessor macros`

Example:

```text
PIC_PLATFORM_CLOCK_HZ=8000000UL
```

## `xc8`

Use the wrapper from `PATH`:

```text
xc8
```
