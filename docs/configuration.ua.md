[English version](configuration.md)

# Концепція конфігурації

## Джерело істини

Окремі library `.c` компілюються як незалежні translation units, тому вони не
бачать `project_config.h`, якщо цей header не підключено в тому самому TU.

Project-wide значення повинні надходити з compiler macros (`-D` або MPLAB
`define-macros`) або з platform/module defaults, які доступні всім translation units.

## Категорії

### Category A: project-wide build defines

Ці значення повинні бути доступні всім translation units:

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

Ці значення передаються у функції й не потребують global defines:

- `lcd_i2c_init(0x27u, 100000u)`
- `uart_init(9600u)`

### Category C: CONFIG-bit helper flags

Ці прапорці документують literal значення `#pragma config` у `config_bits.c`:

- `PIC_PLATFORM_WDT_ENABLED`
- `PIC_PLATFORM_LVP_ENABLED`
- `PIC_PLATFORM_BOR_ENABLED`
- `PIC_PLATFORM_CCP2MUX_ENABLED`

## Правила

1. Безпечні defaults зберігай у `core/pic_platform_config.h` або у module header.
2. Project-wide значення перевизначай через compiler `-D` або MPLAB `define-macros`.
3. Не покладайся на `project_config.h` як на єдине джерело значень для library `.c`.
4. CONFIG bits зберігай як literal значення у `config_bits.c`.
5. Для кожного значення Category A має бути одне джерело істини.

## Частота кварца

`PIC_PLATFORM_CLOCK_HZ` є project-wide контрактом частоти кварца.

`core/device.h` на його основі визначає:

- `DRV_XTAL_FREQ`
- `_XTAL_FREQ`

Значення `PIC_PLATFORM_CLOCK_HZ` повинно бути доступне всім translation units через
compiler `-D` або MPLAB `define-macros`.

## Debug backends

`DRV_DEBUG_ENABLE` вмикає або вимикає весь debug facade.

Вибір backend-ів виконується через:

- `DRV_DEBUG_BACKEND_UART`
- `DRV_DEBUG_BACKEND_DISPLAY`
- `DRV_DEBUG_BACKEND_PINS`

Оскільки debug library `.c` компілюються окремо, ці значення мають передаватися
через compiler `-D` або MPLAB `define-macros`.

## LCD mapping

`lcd_i2c.h` дозволяє перевизначити pin mapping PCF8574 через:

- `LCD_I2C_PIN_RS`
- `LCD_I2C_PIN_RW`
- `LCD_I2C_PIN_EN`
- `LCD_I2C_PIN_BL`
- `LCD_I2C_DATA_SHIFT`

Defaults відповідають типовій PCF8574 backpack-платі.

Якщо конкретна плата має інше розведення, значення потрібно перевизначити через
compiler `-D` або MPLAB `define-macros`, щоб вони були однаковими для всіх
translation units.

## MPLAB X

Project-wide значення задавай у:

```text
XC8 Global Options
-> xc8-cc
-> Preprocessing and messages
-> Preprocessor macros