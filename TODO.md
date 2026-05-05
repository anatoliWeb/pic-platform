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

- [ ] RS485 driver (`drivers/rs485`)
- [ ] Direction pin control DE/RE
- [ ] Send frame
- [ ] Receive frame
- [ ] Timeout handling
- [ ] CRC support
- [ ] Example: master send command
- [ ] Example: slave receive command

### ADC

- [ ] ADC driver (`drivers/adc`)
- [ ] ADC init
- [ ] Read single channel
- [ ] Read multiple channels
- [ ] Averaging support
- [ ] Sensor helper functions
- [ ] Example: read potentiometer
- [ ] Example: read thermistor
- [ ] Example: read ADC buttons

### PWM / CCP

- [ ] PWM driver (`drivers/pwm`)
- [ ] CCP1 PWM support
- [ ] CCP2 PWM support
- [ ] Set duty cycle
- [ ] Set frequency where possible
- [ ] Start/stop PWM
- [ ] Example: LED dimming
- [ ] Example: fan speed control

### TIMER

- [ ] Timer0 driver (`drivers/timer0`)
- [ ] Timer1 driver (`drivers/timer1`)
- [ ] Timer2 driver (`drivers/timer2`)
- [ ] Timer3 driver (`drivers/timer3`)
- [ ] Overflow interrupt support
- [ ] Periodic tick support
- [ ] Millis/tick counter helper
- [ ] Example: periodic LED toggle
- [ ] Example: software scheduler tick

### EEPROM

- [ ] EEPROM driver (`drivers/eeprom`)
- [ ] Read byte
- [ ] Write byte
- [ ] Read block
- [ ] Write block
- [ ] Basic wear-safe helper where possible
- [ ] Example: save setting
- [ ] Example: restore setting after reset

### SPI / MSSP

- [ ] SPI driver (`drivers/spi`)
- [ ] SPI master mode
- [ ] SPI slave mode if needed
- [ ] Transfer byte
- [ ] Transfer buffer
- [ ] Chip select helper
- [ ] Example: SPI sensor read
- [ ] Example: external EEPROM/flash connection

### I2C / MSSP

- [ ] I2C driver (`drivers/i2c`)
- [ ] I2C master mode
- [ ] Start/stop/restart
- [ ] Read/write byte
- [ ] ACK/NACK handling
- [ ] Device scan helper
- [ ] Example: I2C LCD
- [ ] Example: I2C sensor read

### External Interrupts

- [ ] External interrupt driver (`drivers/ext_interrupt`)
- [ ] INT0 support
- [ ] INT1 support
- [ ] INT2 support
- [ ] Rising/falling edge config
- [ ] Callback-style handler where possible
- [ ] Example: button interrupt

### Port Change Interrupt

- [ ] PORTB change interrupt driver (`drivers/portb_change`)
- [ ] Enable/disable change interrupt
- [ ] Read changed pins
- [ ] Example: keypad/buttons

### Watchdog Timer

- [ ] WDT helper (`drivers/wdt`)
- [ ] Enable/disable notes
- [ ] Clear watchdog macro/function
- [ ] Example: safe main loop with watchdog clear

### Comparator

- [ ] Comparator driver (`drivers/comparator`)
- [ ] Enable/disable comparator
- [ ] Configure input mode
- [ ] Read comparator output
- [ ] Example: threshold detection

### Capture / Compare

- [ ] CCP capture driver (`drivers/ccp_capture`)
- [ ] CCP compare driver (`drivers/ccp_compare`)
- [ ] Capture event support
- [ ] Compare event support
- [ ] Example: measure pulse width
- [ ] Example: generate timed event

### Reset / Power

- [ ] Reset helper (`drivers/reset`)
- [ ] Detect reset source where possible
- [ ] Brown-out reset notes
- [ ] Power-on reset notes
- [ ] Example: print reset reason via UART

### Oscillator / Clock

- [ ] Clock helper (`drivers/clock`)
- [ ] Define `_XTAL_FREQ` / oscillator frequency strategy
- [ ] Notes for HS/XT/LP/RC oscillator modes
- [ ] Example: clock config note for C18
- [ ] Example: clock config note for XC8

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
