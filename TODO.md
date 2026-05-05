# TODO

## Мета репозиторію

Цей репозиторій призначений тільки для драйверів і бібліотек для PIC-мікроконтролерів, з основним фокусом на PIC18/PIC18F458.

Це не готовий application-проєкт.  
У майбутніх проєктах потрібні драйвери будуть копіюватися або підключатися звідси.

## Основні правила

- [x] Писати драйвери максимально універсально
- [x] Підтримувати MPLAB C18
- [x] Підтримувати MPLAB XC8
- [x] Усі відмінності компіляторів винести в `core/compiler.h`
- [x] Кожен драйвер має мати `.h`, `.c` та `example.c`
- [x] У кожному `example.c` має бути мінімальний приклад підключення
- [x] Не змішувати драйвери з логікою конкретного проєкту
- [x] Не хардкодити піни без потреби
- [x] Коментарі в коді писати англійською
- [x] Документацію писати зрозумілою українською мовою

---

## Core / Common

- [x] `core/compiler.h` — абстракція компілятора C18/XC8
- [x] `core/types.h` — спільні типи, статуси, помилки
- [x] `core/bit_utils.h` — макроси для роботи з бітами
- [x] `core/delay.h` — універсальні затримки
- [x] `core/device.h` — базові налаштування MCU
- [x] `core/interrupts.h` — допоміжні макроси для interrupt-логіки
- [x] `core/config.h` — загальні feature flags

---

## Driver List And Status

### GPIO

- [x] GPIO driver (`drivers/gpio`)
- [x] Digital input
- [x] Digital output
- [x] Pin direction control
- [x] Pin read/write/toggle
- [x] Example: LED blink
- [x] Example: button read

### UART / USART

- [x] UART driver (`drivers/uart`)
- [x] UART init
- [x] Send byte
- [x] Send string
- [x] Receive byte
- [x] RX interrupt support
- [x] TX interrupt support
- [x] Ring buffer support
- [x] Example: echo received byte

---

### UART Debug (lightweight, optional)

- [x] Debug module (`drivers/uart_debug`)
- [x] Compile-time enable/disable via macro (e.g. `DRV_DEBUG_ENABLE`)
- [x] Zero-cost when disabled (no code, no memory usage)
- [x] Macro-based API:
  - [x] `DBG_PRINT()`
  - [x] `DBG_PRINTLN()`
  - [x] `DBG_WRITE_BYTE()`
- [x] Optional formatted output (lightweight, без важкого printf)
- [x] Redirect to UART driver
- [x] No dynamic memory usage
- [x] Minimal stack usage
- [x] Example: print debug message
- [x] Example: debug values (int/hex)

### RS485

- [x] RS485 driver (`drivers/rs485`)
- [x] Direction pin control DE/RE
- [x] Send frame
- [x] Receive frame
- [x] Timeout handling
- [x] CRC support
- [x] Example: master send command
- [x] Example: slave receive command

### ADC

- [x] ADC driver (`drivers/adc`)
- [x] ADC init
- [x] Read single channel
- [x] Read multiple channels
- [x] Averaging support
- [x] Sensor helper functions
- [x] Example: read potentiometer
- [x] Example: read thermistor
- [x] Example: read ADC buttons

### PWM / CCP

- [x] PWM driver (`drivers/pwm`)
- [x] CCP1 PWM support
- [x] CCP2 PWM support
- [x] Set duty cycle
- [x] Set frequency where possible
- [x] Start/stop PWM
- [x] Example: LED dimming
- [x] Example: fan speed control

### TIMER

- [x] Timer0 driver (`drivers/timer0`)
- [x] Timer1 driver (`drivers/timer1`)
- [x] Timer2 driver (`drivers/timer2`)
- [x] Timer3 driver (`drivers/timer3`)
- [x] Overflow interrupt support
- [x] Periodic tick support
- [x] Millis/tick counter helper
- [x] Example: periodic LED toggle
- [x] Example: software scheduler tick

### EEPROM

- [x] EEPROM driver (`drivers/eeprom`)
- [x] Read byte
- [x] Write byte
- [x] Read block
- [x] Write block
- [x] Basic wear-safe helper where possible
- [x] Example: save setting
- [x] Example: restore setting after reset

### SPI / MSSP

- [x] SPI driver (`drivers/spi`)
- [x] SPI master mode
- [x] SPI slave mode if needed
- [x] Transfer byte
- [x] Transfer buffer
- [x] Chip select helper
- [x] Example: SPI sensor read
- [x] Example: external EEPROM/flash connection

### I2C / MSSP

- [x] I2C driver (`drivers/i2c`)
- [x] I2C master mode
- [x] Start/stop/restart
- [x] Read/write byte
- [x] ACK/NACK handling
- [x] Device scan helper
- [x] Example: I2C LCD
- [x] Example: I2C sensor read

### External Interrupts

- [x] External interrupt driver (`drivers/ext_interrupt`)
- [x] INT0 support
- [x] INT1 support
- [x] INT2 support
- [x] Rising/falling edge config
- [x] Callback-style handler where possible
- [x] Example: button interrupt

### Port Change Interrupt

- [x] PORTB change interrupt driver (`drivers/portb_change`)
- [x] Enable/disable change interrupt
- [x] Read changed pins
- [x] Example: keypad/buttons

### Watchdog Timer

- [x] WDT helper (`drivers/wdt`)
- [x] Enable/disable notes
- [x] Clear watchdog macro/function
- [x] Example: safe main loop with watchdog clear

### Comparator

- [x] Comparator driver (`drivers/comparator`)
- [x] Enable/disable comparator
- [x] Configure input mode
- [x] Read comparator output
- [x] Example: threshold detection

### Capture / Compare

- [x] CCP capture driver (`drivers/ccp_capture`)
- [x] CCP compare driver (`drivers/ccp_compare`)
- [x] Capture event support
- [x] Compare event support
- [x] Example: measure pulse width
- [x] Example: generate timed event

### Reset / Power

- [x] Reset helper (`drivers/reset`)
- [x] Detect reset source where possible
- [x] Brown-out reset notes
- [x] Power-on reset notes
- [x] Example: print reset reason via UART

### Oscillator / Clock

- [x] Clock helper (`drivers/clock`)
- [x] Define `_XTAL_FREQ` / oscillator frequency strategy
- [x] Notes for HS/XT/LP/RC oscillator modes
- [x] Example: clock config note for C18
- [x] Example: clock config note for XC8

### 1-Wire

- [x] 1-Wire driver (`drivers/onewire`)
- [x] Single-wire GPIO bit-banging implementation
- [x] Reset pulse
- [x] Presence detect

---

### Core Operations

- [x] Write bit
- [x] Read bit
- [x] Write byte
- [x] Read byte

---

### ROM Commands

- [x] Skip ROM (0xCC)
- [x] Match ROM (0x55)
- [x] Read ROM (0x33)
- [x] Search ROM (basic support or placeholder)

---

### Device Support (DS18B20)

- [x] Start temperature conversion
- [x] Read scratchpad
- [x] Convert raw data to temperature
- [x] Basic CRC check

---

### Timing Handling

- [ ] Delay-based timing (using core/delay.h)
- [ ] Ensure compatibility with C18 and XC8
- [ ] Minimal blocking implementation
- [ ] No hardware timers required (bit-banging only)

---

### API

- [ ] onewire_init(volatile uint8_t* port, volatile uint8_t* tris, uint8_t pin)
- [ ] onewire_reset(void)
- [ ] onewire_write_bit(uint8_t bit)
- [ ] onewire_read_bit(void)
- [ ] onewire_write_byte(uint8_t byte)
- [ ] onewire_read_byte(void)

---

### Higher-Level Helpers

- [ ] onewire_skip_rom()
- [ ] onewire_read_rom(uint8_t* rom)
- [ ] onewire_match_rom(uint8_t* rom)

---

### DS18B20 Helpers

- [ ] ds18b20_start_conversion()
- [ ] ds18b20_read_temperature_raw()
- [ ] ds18b20_read_temperature_celsius()

---

### CRC

- [ ] Implement CRC8 (Dallas/Maxim)
- [ ] Validate scratchpad data

---

### Examples

- [ ] Example: detect device (presence)
- [ ] Example: read ROM code
- [ ] Example: read temperature (DS18B20)
- [ ] Example: multiple sensors (optional)

---

### Compiler-Specific

- [ ] C18 implementation (`C18/drivers/onewire/onewire.c`)
- [ ] XC8 implementation (`XC8/drivers/onewire/onewire.c`)
- [ ] Universal fallback implementation

---

### Integration

- [ ] Use GPIO driver for pin control
- [ ] Use delay wrapper from core/delay.h
- [ ] No malloc
- [ ] Minimal stack usage

---

### Documentation

- [ ] Add README section: 1-Wire driver
- [ ] Explain timing constraints
- [ ] Explain DS18B20 usage



### LCD / Display Helpers

- [ ] HD44780 LCD driver (`drivers/lcd_hd44780`)
- [ ] 4-bit mode
- [ ] 8-bit mode if needed
- [ ] Print char/string
- [ ] Set cursor
- [ ] Example: print debug text

### Buttons

- [ ] Button driver (`drivers/button`)
- [ ] Debounce
- [ ] Click
- [ ] Hold
- [ ] Double click where possible
- [ ] Example: one digital button
- [ ] Example: menu button

### ADC Buttons

- [ ] ADC button driver (`drivers/adc_buttons`)
- [ ] Multiple buttons on one ADC channel
- [ ] Threshold table
- [ ] Debounce
- [ ] Click/hold support
- [ ] Example: resistor ladder buttons

### Software Scheduler

- [ ] Lightweight scheduler (`drivers/scheduler`)
- [ ] Periodic task tick
- [ ] Non-blocking timers
- [ ] Example: run tasks every 10 ms / 100 ms / 1 s

### CRC / Checksum

- [ ] CRC helper (`drivers/crc`)
- [ ] CRC8
- [ ] CRC16
- [ ] Simple checksum
- [ ] Example: RS485 frame validation

### Ring Buffer

- [ ] Ring buffer helper (`drivers/ring_buffer`)
- [ ] Byte buffer
- [ ] Push/pop
- [ ] Overflow handling
- [ ] Example: UART RX buffer

---

## Common Requirements For Every Driver

- [ ] Define public API in `.h`
- [ ] Implement source in `.c`
- [ ] Add required `example.c`
- [ ] Add include guard in every `.h`
- [ ] Use clear function names
- [ ] Avoid project-specific logic
- [ ] Avoid unnecessary global variables
- [ ] Add short English comments in code
- [ ] Add Ukrainian notes in `docs/` where useful
- [ ] Validate build with MPLAB C18
- [ ] Validate build with MPLAB XC8
- [ ] Keep compiler-specific code only in `core/compiler.h`

---

## Documentation

- [ ] `README.md` — загальний опис бібліотеки
- [ ] `TODO.md` — список задач і статусів
- [ ] `docs/compiler.md` — різниця між C18 та XC8
- [ ] `docs/driver-convention.md` — правила створення драйвера
- [ ] `docs/naming.md` — правила іменування
- [ ] `docs/examples.md` — як користуватися прикладами
- [ ] `docs/porting.md` — як переносити драйвер у новий проєкт

---

## Folder Structure To Keep

```text
/C18
/XC8
/core
/drivers
/examples
/docs

README.md
TODO.md
```

---

## Future Improvements

- [ ] Додати optional config headers для кожного драйвера
- [ ] Додати спільний тип помилок/status для всіх драйверів
- [ ] Додати host/simulation тестування там, де можливо
- [ ] Додати CI build matrix для C18/XC8, якщо буде доступна автоматизація toolchain
- [ ] Додати версіонування драйверів
- [ ] Додати changelog
- [ ] Додати шаблон нового драйвера
- [ ] Додати приклад інтеграції драйверів у зовнішній PIC-проєкт


## Future: Driver Builder / Integration System

> Not implemented now. Planned for future.

### Dependency Management

- [ ] Define driver dependencies (e.g. RS485 → UART)
- [ ] Add dependency description inside each driver (metadata file or header)
- [ ] Ensure driver can declare required modules

### Driver Packaging

- [ ] Define standard driver structure for export/import
- [ ] Ensure each driver is self-contained
- [ ] Support copying driver with all required files

### Builder Concept

- [ ] Design driver builder tool (script or CLI)
- [ ] Input:
  - [ ] selected drivers
  - [ ] target compiler (C18 / XC8)
- [ ] Output:
  - [ ] ready-to-use project structure
  - [ ] only required drivers
  - [ ] resolved dependencies

### Auto Dependency Resolution

- [ ] Automatically include required drivers
- [ ] Avoid duplicate modules
- [ ] Validate compatibility between drivers

### Configuration System

- [ ] Add per-driver config headers (e.g. `uart_config.h`)
- [ ] Allow enabling/disabling features
- [ ] Central config file for project

### Build Integration

- [ ] Prepare example builds for:
  - [ ] C18
  - [ ] XC8
- [ ] Generate ready MPLAB project (optional)
- [ ] Add simple build scripts (bat/sh)

### Validation

- [ ] Validate driver compatibility before build
- [ ] Detect missing dependencies
- [ ] Detect conflicting configurations

### Documentation

- [ ] Document how builder works
- [ ] Document driver dependency system
- [ ] Provide usage examples
