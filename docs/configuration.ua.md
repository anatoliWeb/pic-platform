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
- `_XTAL_FREQ` / `DRV_XTAL_FREQ` (похідні аліаси; краще використовувати `PIC_PLATFORM_CLOCK_HZ`)
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
- `LCD_I2C_MINIMAL` (вибір MINIMAL-профілю LCD; прибирає `lcd_i2c_is_ready()`, `lcd_i2c_home()`, `lcd_i2c_write_string()` тощо, тому змінює публічний API і має бути однаковим у кожному TU)
- `SEVEN_SEGMENT_ENABLE_TIMER0`
- `SEVEN_SEGMENT_ENABLE_TIMER1`
- `SEVEN_SEGMENT_ENABLE_TIMER2`
- `SEVEN_SEGMENT_ENABLE_TIMER3`
- `TACHOMETER_LIGHTWEIGHT` (вибір LIGHTWEIGHT-профілю тахометра; змінює layout `tachometer_t`, тому має бути однаковим у кожному TU)
- `POSITION_DRIVE_SENSOR_TYPE`
- `POSITION_DRIVE_ENABLE_PWM`
- `POSITION_DRIVE_ENABLE_TIMEOUT`
- `POSITION_DRIVE_ENABLE_STUCK_DETECTION`
- `POSITION_DRIVE_ENABLE_DIRECTION_CHECK`
- `POSITION_DRIVE_ENABLE_UART_DEBUG`
- `RB_OVERWRITE` (політика запису ring buffer: перезапис vs відкидання)
- `DRV_USE_FREERTOS` (вибір backend абстракції RTOS)
- `DRV_USE_UART` (вмикання UART debug, разом з `DRV_DEBUG_ENABLE`)
- `DRV_DEBUG_UART_BAUD` (швидкість UART debug)
- `SCHEDULER_MAX_TASKS` (розмір пулу слотів планувальника)
- `CRON_MAX_TASKS` (вбудовує `cron_task_t tasks[CRON_MAX_TASKS]` у `cron_scheduler_t`, тому змінює layout структури, яку бачить користувач)
- `LED_ANIM_ENGINE_MAX_ANIMATIONS` (вбудовує `led_animation_t pool[...]` у `led_animation_engine_t`, тому змінює layout структури, яку бачить користувач)
- `AC_PHASE_CONTROL_MAX_CHANNELS` (межа каналів, яку використовує `ac_phase_control.c`)

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
6. Library `.c` і application ніколи не повинні бачити різні значення одного define. Якщо library `.c` використовує макрос у `#if` або вбудовує його у структуру, яку бачить користувач, цей макрос належить до Category A.

## Офіційна таблиця макросів

| Макрос | Default | Файли | Вплив на layout/API | Project-wide? |
|---|---|---|---|---|
| `PIC_PLATFORM_CLOCK_HZ` | 8000000UL | `core/device.h` | похідні для тактування (`_XTAL_FREQ`, `DRV_XTAL_FREQ`) | так |
| `_XTAL_FREQ`, `DRV_XTAL_FREQ` | похідні | `core/device.h` | тактування компілятора, UART/I2C/PWM/tick | так |
| `DRV_DEBUG_ENABLE` | 1 | `core/config.h` | вмикає весь debug facade | так |
| `DRV_USE_UART` | 1 | `core/config.h` | вмикання UART debug | так |
| `DRV_DEBUG_LEVEL` | 1 | `libraries/system/debug/debug.h` | макро-розкриття debug API | так |
| `DRV_DEBUG_UART_BAUD` | 9600UL | `debug.h`, `debug_backend_uart.c` | значення для `uart_init()` | так |
| `DRV_DEBUG_BACKEND_*`, `DRV_DEBUG_DISPLAY_*`, `DRV_DEBUG_PINS_*` | 0 | `debug.h` | compile-out / dispatch backend-ів | так |
| `LCD_I2C_PIN_*`, `LCD_I2C_DATA_SHIFT` | PCF8574 defaults | `lcd_i2c.h`, `lcd_i2c.c` | розкладка nibble | так |
| `LCD_I2C_MINIMAL` | 0 | `lcd_i2c.h`, `lcd_i2c.c` | **прибирає публічний API** (`lcd_i2c_is_ready`, `lcd_i2c_home`, `lcd_i2c_write_string`, ...) | так |
| `TACHOMETER_LIGHTWEIGHT` | 0 | `tachometer.h`, `tachometer.c` | **змінює layout `tachometer_t`** | так |
| `SEVEN_SEGMENT_ENABLE_TIMER0..3` | 0 | `seven_segment.c` | compile-out таймерного backend | так |
| `POSITION_DRIVE_*` | див. config | `position_drive.c` | backend та поведінкові gate-и | так |
| `RB_OVERWRITE` | 0 | `ring_buffer.h`, `ring_buffer.c` | політика запису ring buffer | так |
| `DRV_USE_FREERTOS` | 0 | `rtos_port.h`, `rtos.c` | режим абстракції RTOS | так |
| `SCHEDULER_MAX_TASKS` | 10u | `scheduler.h`, `scheduler.c` | розмір пулу слотів | так |
| `CRON_MAX_TASKS` | 8u | `cron_scheduler.h` | **вбудовує масив у `cron_scheduler_t`** | так |
| `LED_ANIM_ENGINE_MAX_ANIMATIONS` | 8u | `led_animation_engine.h` | **вбудовує масив у `led_animation_engine_t`** | так |
| `AC_PHASE_CONTROL_MAX_CHANNELS` | 4U | `ac_phase_control.h`, `.c` | перевірка меж каналів | так |

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

## Профілі

Profile-макрос вибирає інший скомпільований набір функцій. Його треба задавати
project-wide, щоб кожен translation unit компілював однаковий набір функцій:

- `TACHOMETER_LIGHTWEIGHT=1` вибирає LIGHTWEIGHT-профіль тахометра (змінює layout `tachometer_t`).
- `LCD_I2C_MINIMAL=1` вибирає MINIMAL-профіль LCD (прибирає кілька публічних функцій).
- `DRV_USE_FREERTOS=1` вибирає backend абстракції RTOS на FreeRTOS.

```text
-DTACHOMETER_LIGHTWEIGHT=1
-DLCD_I2C_MINIMAL=1
-DDRV_USE_FREERTOS=1
```

Не визначай profile-макрос у `project_config.h` або в library header як локальне
перевизначення одного TU: `project_config.h` не видно окремо скомпільованим library
`.c`, а локальне визначення в header видно лише одному TU.

## MPLAB X

Project-wide значення задавай у:

```text
XC8 Global Options
-> xc8-cc
-> Preprocessing and messages
-> Preprocessor macros