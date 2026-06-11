# TODO

## Structure

- [x] Keep repository split into `core/`, `drivers/`, `libraries/`, `C18/`, `XC8/`, `docs/`, and `examples-projects/`.
- [x] Keep low-level hardware/protocol modules under `drivers/`.
- [x] Keep high-level reusable components under `libraries/`.
- [x] Keep software-only utilities under `core/`.
- [x] Keep reusable AI prompt context files under `prompts/`.
- [ ] Review legacy duplicate headers in `libraries/display/lcd_hd44780/` (`lcd.h` and `lcd_hd44780.h`).
- [ ] Review generic/legacy timer folder `drivers/timers/timer/` for overlap with `timer0..timer3`.



## Core

- [x] Compiler abstraction exists in `core/compiler.h`.
- [x] Common types and device defaults exist in `core/types.h` and `core/device.h`.
- [x] Delay layer exists in `core/delay.*`.
- [x] CRC module exists in `core/crc/`.
- [x] Ring buffer module exists in `core/ring_buffer/`.
- [x] Cooperative scheduler exists in `core/scheduler/`.
- [x] RTOS abstraction exists in `core/rtos/`.
- [ ] Validate all core modules with MPLAB XC8.
- [ ] Validate all core modules with MPLAB C18.

## Drivers

- [x] GPIO driver exists.
- [x] Communication drivers exist: UART, I2C, SPI, RS485, 1-Wire.
- [x] Analog drivers exist: ADC, comparator.
- [x] Timer drivers exist: timer0, timer1, timer2, timer3, tick, PWM, CCP capture, CCP compare.
- [x] Interrupt drivers exist: external interrupt and PORTB change interrupt.
- [x] Memory drivers exist: EEPROM and I2C EEPROM.
- [x] System drivers/helpers exist: clock, reset, WDT.
- [x] Driver documentation exists under `docs/drivers/`.
- [ ] Validate driver builds with MPLAB XC8.
- [ ] Validate driver builds with MPLAB C18.
- [ ] Review compiler-specific coverage for newer modules and decide whether universal implementations are enough.

## Libraries

- [x] Input libraries exist: button, ADC buttons, encoder, keypad matrix, IR receiver, shared segment buttons.
- [x] System libraries exist: buzzer, relay, servo, UART debug.
- [x] LED libraries exist: RGB utils, WS2812, WS2812 matrix, APA102, effects, segments, animation engine.
- [x] Library documentation exists under `docs/libraries/`.
- [ ] Validate library builds with MPLAB XC8.
- [ ] Validate library builds with MPLAB C18.
- [ ] Review optional C18/XC8-specific implementations for high-level libraries that need timing-critical code.

## Display / Graphics / UI

- [x] Display libraries exist: LCD HD44780, seven segment, seven segment mux, SSD1306, SH1106, ST7735, ILI9341.
- [x] Graphics core exists.
- [x] Bitmap renderer exists.
- [x] Display framework exists.
- [x] UI libraries exist: icon pack, themes, menu core, input adapter, menu navigation, menu renderer, menu widgets.
- [x] UI runtime helpers exist: events, screen manager, popup manager, overlay system, screen transitions, sprite buffer.
- [ ] Validate display/UI examples on real hardware or simulator.
- [ ] Review RAM usage for graphics, OLED, TFT, sprite, and animation modules on PIC18F452.

## Sensors

- [x] DS18B20 library exists.
- [x] DHT library exists.
- [x] BMP library exists.
- [x] Ultrasonic library exists.
- [x] Sensor documentation exists.
- [ ] Validate timing-sensitive sensors on hardware or Proteus where possible.
- [ ] Add more sensor examples only where real `.X` projects are created.

## RTC / Time

- [x] RTC framework exists.
- [x] Software RTC exists.
- [x] RTC backends exist: DS1302, DS1307, DS3231, DS12C887, PCF8563.
- [x] Cron scheduler exists.
- [x] Timezone utilities exist.
- [x] NTP sync library exists.
- [ ] Validate RTC backends against real hardware or known-good simulation.
- [ ] Add alarm/timer features only after backend validation.

## RTOS / Tasking

- [x] Cooperative scheduler exists and uses external tick input.
- [x] RTOS abstraction exists with bare-metal sleep/delay support.
- [x] Future RTOS switch exists through `DRV_USE_FREERTOS`.
- [ ] Implement optional FreeRTOS backend hooks.
- [ ] Implement mutex abstraction when a real backend is selected.
- [ ] Implement queue abstraction when a real backend is selected.
- [ ] Migrate remaining library delay calls from `delay_ms()` or `DRV_DELAY_MS()` to `rtos_sleep_ms()` where scheduler safety matters.

## Project Configuration Layer

- [x] Add project-level configuration concept for PIC18F452 example projects.
- [x] Add `project_config.h` template for application-level options.
- [x] Add `config_bits.c` template for PIC18F452 XC8 projects.
- [x] Add `config_bits.c` template for PIC18F452 C18 projects.
- [x] Add central place for final CONFIG bit values selected by the project.
- [x] Add optional library requirements/recommendations convention.
- [x] Add configuration checker for required project options.
- [x] Document that libraries must not directly emit `#pragma config`.
- [x] Document that libraries may declare CONFIG requirements or recommendations.
- [x] Document that the project has final priority over recommended CONFIG values.
- [x] Document oscillator mode versus clock frequency.
- [x] Document `_XTAL_FREQ`, `DRV_XTAL_FREQ`, and `PIC_PLATFORM_CLOCK_HZ` relationship.
- [x] Document recommended PIC18F452 defaults: HS oscillator, WDT OFF, LVP OFF, BOR ON, STVR ON.
- [x] Add notes for special CONFIG bits affected by modules, such as WDT and CCP2MUX.
- [x] Update XC8 `blink.X` to use `project_config.h` and `config_bits.c`.
- [x] Update C18 `blink.X` to use `project_config.h` and `config_bits.c`.
- [ ] Validate whether XC8 accepts macros inside `#pragma config`.
- [ ] Validate whether C18 accepts macros inside `#pragma config`.
- [ ] If C18 does not support config macros reliably, keep C18 direct CONFIG bit template.

## Examples

- [x] Common `.c` examples exist for many core, driver, and library modules.
- [x] C18 example `.c` files exist under `C18/examples/`.
- [x] XC8 example `.c` files exist under `XC8/examples/`.
- [x] MPLAB `.X` example project exists for `examples-projects/xc8/blink.X`.
- [x] MPLAB `.X` example project exists for `examples-projects/c18/blink.X`.
- [ ] Create full `.X` example projects for UART debug.
- [ ] Create full `.X` example projects for ADC/I2C/SPI.
- [ ] Create full `.X` example projects for DS18B20.
- [ ] Create full `.X` example projects for display/UI demos.
- [ ] Verify `.X` project source references use external `pic-platform` paths.

## Proteus Simulation

- [x] Add Proteus projects for basic GPIO blink.
- [ ] Add Proteus projects for UART debug.
- [ ] Add Proteus projects for LCD/DS18B20 where practical.
- [ ] Document Proteus setup and expected HEX loading steps.

## Documentation

- [x] Root English README exists.
- [x] Root Ukrainian README exists.
- [x] Architecture documentation exists.
- [x] MPLAB integration documentation exists.
- [x] Compiler documentation exists.
- [x] Build validation documentation exists.
- [x] Driver and library documentation exists in English and Ukrainian.
- [x] Prompt kit documentation exists.
- [ ] Keep documentation links synchronized after future module moves.
- [ ] Add per-module "tested with" status once builds/hardware checks are done.

## Validation

- [x] Repository structure has been scanned and TODO rebuilt from current files.
- [x] Existing `.X` projects detected only for C18 blink and XC8 blink.
- [x] Proteus blink project files were detected under `examples-projects/proteus/blink/`.
- [ ] Run MPLAB XC8 build validation.
- [ ] Run MPLAB C18 build validation.
- [ ] Validate include paths and source-file lists in `.X` projects.
- [ ] Validate timing-sensitive modules: 1-Wire, DHT, WS2812, IR receiver.
- [ ] Validate memory usage for PIC18F452.

## Future Work

- [ ] Optional FreeRTOS backend integration.
- [ ] Optional mutex and queue abstractions.
- [ ] Advanced communication protocols.
- [ ] Additional RTC alarm/square-wave features.
- [ ] More complete MPLAB `.X` example projects.
- [ ] Proteus simulation coverage.
- [ ] Legacy module cleanup after build validation.
- [ ] Board/profile configuration templates for more PIC18 devices.
