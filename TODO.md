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
- [x] Driver documentation exists under docs/drivers/.
- [x] Phase 1 examples validation audit created under docs/audits/.
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
- [x] OneWire is working for PIC18F452 + XC8 + Proteus + 10 MHz.
- [x] DS18B20 is working for PIC18F452 + XC8 + Proteus + 10 MHz.
- [x] Validate flows: SEARCH_ROM, MATCH_ROM temperature read, SKIP_ROM temperature read, CRC validation.
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
- [x] Add reusable PIC18F452 `templates/config/.../config_bits.c` reference templates.
- [x] Add central place for final CONFIG bit values selected by the project.
- [x] Add optional library requirements/recommendations convention.
- [x] Add configuration checker for required project options.
- [x] Add `core/config/library_requirements.md` for CONFIG conventions.
- [x] Add `core/config/example.c` reference configuration example.
- [x] Document that libraries must not directly emit `#pragma config`.
- [x] Document that libraries may declare CONFIG requirements or recommendations.
- [x] Document that the project has final priority over recommended CONFIG values.
- [x] Document oscillator mode versus clock frequency.
- [x] Document `_XTAL_FREQ`, `DRV_XTAL_FREQ`, and `PIC_PLATFORM_CLOCK_HZ` relationship.
- [x] Document recommended PIC18F452 defaults: HS oscillator, WDT OFF, LVP OFF, BOR ON, STVR ON.
- [x] Add notes for special CONFIG bits affected by modules, such as WDT and CCP2MUX.
- [x] Update XC8 `blink.X` to use `project_config.h` and `config_bits.c`.
- [x] Update C18 `blink.X` to use `project_config.h` and `config_bits.c`.
- [x] Validate whether XC8 accepts macros inside `#pragma config`.
- [ ] Validate whether C18 accepts macros inside `#pragma config`.
- [x] Use direct literal CONFIG bits in XC8 example projects after macro pragma validation failed.
- [x] Use direct literal CONFIG bits in C18 example projects.
- [x] Keep `project_config.h` limited to clock/timing values and helper flags.
- [x] Keep `config_bits.c` as the only source file that emits CONFIG bits.
- [x] Remove macro-based CONFIG pragmas from PIC18F452 XC8 example projects.
- [x] Remove macro-based CONFIG pragmas from PIC18F452 template files.
- [ ] If C18 does not support config macros reliably, keep C18 direct CONFIG bit template.

## Examples

- [x] Common `.c` examples exist for many core, driver, and library modules.
- [x] C18 example `.c` files exist under `C18/examples/`.
- [x] XC8 example `.c` files exist under `XC8/examples/`.
- [x] MPLAB `.X` example project exists for `examples-projects/xc8/blink.X`.
- [x] MPLAB `.X` example project exists for `examples-projects/c18/blink.X`.
- [x] Create wrapper `.X` projects for the common and compiler-specific example sources.
- [x] Create full `.X` example projects for UART debug.
- [x] Create full `.X` example projects for ADC/I2C/SPI.
- [x] Create full `.X` example projects for DS18B20.
- [x] Create DS18B20 multi-sensor `.X` example projects.
- [x] Implement real 1-Wire Search ROM algorithm.
- [x] Support DS18B20 multi-sensor discovery by ROM search.
- [x] Validate `examples-projects/xc8/ds18b20.X` on XC8 + Proteus + 10 MHz.
- [x] Create diagnostic `.X` example projects for `onewire_bus_test.X` and `ds18b20_search_rom.X`.
- [x] Validate `examples-projects/xc8/onewire_bus_test.X` on XC8 + Proteus + 10 MHz.
- [x] Validate `examples-projects/xc8/ds18b20_search_rom.X` on XC8 + Proteus + 10 MHz.
- [x] Split the DS18B20 example into a lean temperature example plus separate diagnostics.
- [ ] Create full `.X` example projects for display/UI demos.
- [x] Verify `.X` project source references use external `pic-platform` paths.
- [x] Validate the new `.X` projects in MPLAB.
- [ ] Validate DS18B20 multi-sensor example in MPLAB XC8.
- [ ] Validate DS18B20 multi-sensor example in MPLAB C18.
- [ ] Add Proteus simulations for the new examples.

### Example Status Snapshot

- [x] Verified in Proteus: blink, uart_debug, adc_read, ds18b20_read, ring_buffer, rs485_basic, rs485_echo, spi, spi_loopback, tick, timer0, timer1, timer2, timer3, uart.
- [x] Verified in Proteus: wdt.
- [ ] Deferred/postponed for now: export XC8 HEX files for `ds18b20_search_rom.X` and `onewire_bus_test.X`.
- [ ] Needs review / Proteus and C18 validation: `ds18b20_multi_read.X`.
- [x] Proteus README coverage is complete for the current example folders.
- [x] Source Files cleanup audit found no missing external `pic-platform` paths.
## Input examples documentation

- [x] Create and synchronize bilingual README files for `button.X`, `encoder.X`, and `adc_buttons.X`.
- [x] Document phase 1 input behavior without introducing menu logic.
- [ ] Add Proteus verification notes when manual validation is recorded.

## Phase 2 menu libraries

- [ ] Encoder menu demo (Planned for Phase 2)
- [ ] Button/encoder navigation layer (Planned for Phase 2)
- [ ] LCD menu library (Planned for Phase 2)
- [ ] OLED menu library (Planned for Phase 2)
- [ ] Display-specific menu examples (Planned for Phase 2)
- [ ] Keep menu libraries separate from phase 1 input example docs.
## Proteus Simulation

- [x] Organize Proteus example folder structure for all example projects.
- [x] Document shared HEX usage for Proteus examples.
- [x] Add Proteus README/proteus-version placeholders for all example projects.
- [x] Add Proteus projects for basic GPIO blink.
- [ ] Add Proteus projects for UART debug.
- [ ] Add Proteus projects for LCD/DS18B20 and DS18B20 multi-sensor where practical.
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
- [x] Existing `.X` projects now include the original validation set plus expanded wrapper projects for common and compiler-specific examples.
- [x] Proteus blink project files were detected under `examples-projects/proteus/blink/`.
- [x] New `.X` example wrapper projects were created for the example sources under `examples/common`, `XC8/examples`, and `C18/examples`.
- [x] Run MPLAB XC8 build validation.
- [x] Run MPLAB C18 build validation pass for all C18 example projects.
- [ ] Run MPLAB C18 build validation.
  - Attempted for all 40 C18 example projects after resolving the local Windows/Application Control permission issue that previously blocked `_mplink.exe`; 13 passed and 27 failed with project-specific compile/link issues.
- [x] Export successful C18 HEX files to `examples-projects/hex/c18/<project>/<project>.hex`.
- [ ] Validate the new `.X` projects in MPLAB.
- [x] Validate DS18B20 multi-sensor example in MPLAB XC8.
- [x] Validate include paths and source-file lists in `.X` projects.
- [x] Export successful XC8 HEX files to `examples-projects/hex/xc8/<project>/<project>.hex`.
- [x] Normalize MPLAB `.X` project logical source tree structure.
- [x] Ensure example `main.c` files include headers only and not `.c` files.
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
- [ ] Refactor OneWire timing model to separate real microseconds from write tuning ticks.
- [ ] Test C18 build later.
- [ ] Test real hardware later.

### Gyver-style Library Porting

- [ ] Create and maintain `docs/planning/gyverlibs-port.ua.md`.
- [ ] Port button, encoder, and input helpers first.
- [ ] Validate each port with an XC8 example project.
- [ ] Validate C18 compatibility where practical.
## Platform Extensions

### Software PWM

* [x] Create initial `libraries/output/software_pwm/` library for arbitrary GPIO channels driven by one timer-based PWM group.
* [x] Create `docs/libraries/output/software_pwm.md`.
* [x] Create `docs/libraries/output/software_pwm.ua.md`.
* [x] Create `examples-projects/xc8/software_pwm.X` as the initial XC8 example project.
* [x] Create `examples-projects/proteus/software_pwm/README.md`.
* [x] Create `examples-projects/proteus/software_pwm/README.ua.md`.
* [x] Validate `software_pwm.X` in Proteus.
* [x] Confirm software PWM works correctly in Proteus simulation.
* [ ] Extend timer-source validation for Timer0, Timer1, and Timer3.
* [ ] Add more software PWM examples for common use cases:

  * [ ] LED brightness control.
  * [ ] RGB LED control.
  * [ ] Multiple GPIO PWM outputs on PORTD.
  * [ ] Servo-style pulse output example.
  * [ ] Slow heater / relay / SSR control example.

### Gyver-style / Arduino-like Library Expansion

Rule: if a similar module already exists in `pic-platform`, extend and clean it first instead of duplicating logic in a new folder.

#### Input / Buttons / Navigation

* [ ] Add button helper library.

  * [ ] Debounce.
  * [ ] Click.
  * [ ] Hold.
  * [ ] Long press.
  * [ ] Multi-click.
  * [ ] Non-blocking update model.
  * [ ] Minimal XC8 example: `button_click_hold.X`.
  * [ ] Proteus validation.
  * [ ] C18 compatibility check where practical.

* [ ] Add rotary encoder helper library.

  * [ ] Direction detection.
  * [ ] Step counting.
  * [ ] Optional acceleration.
  * [ ] Button + encoder combined navigation.
  * [ ] Minimal XC8 example: `encoder_basic.X`.
  * [ ] Minimal XC8 example: `encoder_menu.X`.
  * [ ] Proteus validation.
  * [ ] C18 compatibility check where practical.

* [ ] Add ADC buttons helper library.

  * [ ] Resistor ladder buttons on one ADC input.
  * [ ] Threshold table.
  * [ ] Noise tolerance / hysteresis.
  * [ ] Optional calibration notes.
  * [ ] Minimal XC8 example: `adc_buttons_basic.X`.
  * [ ] Minimal XC8 example: `adc_buttons_menu.X`.
  * [ ] Proteus validation.
  * [ ] C18 compatibility check where practical.

* [ ] Add matrix keypad helper library.

  * [ ] 2x2 keypad scan.
  * [ ] 3x4 keypad scan.
  * [ ] 4x4 keypad scan.
  * [ ] Debounce.
  * [ ] Non-blocking scan model.
  * [ ] Minimal XC8 example: `keypad_matrix.X`.
  * [ ] Proteus validation.
  * [ ] C18 compatibility check where practical.

#### Timing / Scheduler / Control Helpers

* [ ] Add millis-like timing helpers.

  * [ ] Shared system tick.
  * [ ] `millis()` style counter.
  * [ ] `elapsed` helper.
  * [ ] Non-blocking timer helper.
  * [ ] Minimal XC8 example: `millis_demo.X`.
  * [ ] Proteus validation.
  * [ ] C18 compatibility check where practical.

* [ ] Add relay helper library.

  * [ ] Simple ON/OFF.
  * [ ] Pulse mode.
  * [ ] Delayed OFF.
  * [ ] Delayed ON.
  * [ ] Non-blocking timer-based control.
  * [ ] Minimal XC8 example: `relay_pulse.X`.
  * [ ] Proteus validation.
  * [ ] C18 compatibility check where practical.

* [ ] Add buzzer / beeper helper library.

  * [ ] Single beep.
  * [ ] Beep pattern.
  * [ ] Short melody / tone pattern.
  * [ ] Non-blocking playback.
  * [ ] Minimal XC8 example: `buzzer_pattern.X`.
  * [ ] Proteus validation.
  * [ ] C18 compatibility check where practical.

#### Output Helpers

* [ ] Add hardware PWM / CCP helper examples.

  * [ ] CCP1 PWM example on RC2.
  * [ ] CCP2 PWM example on RC1.
  * [ ] CCP2 alternative output note for RB3 / CCP2MUX.
  * [ ] Comparison note: hardware PWM vs software PWM.
  * [ ] Minimal XC8 example: `ccp_pwm_basic.X`.
  * [ ] Proteus validation.
  * [ ] C18 compatibility check where practical.

* [ ] Add servo output helper.

  * [ ] 1 servo output.
  * [ ] Multiple servo outputs if timing allows.
  * [ ] 1000-2000 us pulse control.
  * [ ] 20 ms frame documentation.
  * [ ] Minimal XC8 example: `servo_basic.X`.
  * [ ] Proteus validation.
  * [ ] C18 compatibility check where practical.

* [ ] Add LED effects helper.

  * [ ] Blink pattern.
  * [ ] Fade using software PWM.
  * [ ] RGB status LED.
  * [ ] Smooth transition helper.
  * [ ] Minimal XC8 example: `led_effects.X`.
  * [ ] Proteus validation.
  * [ ] C18 compatibility check where practical.

* [ ] Add MOSFET output usage notes.

  * [ ] Low-voltage DC load examples only.
  * [ ] Gate resistor note.
  * [ ] Gate pulldown note.
  * [ ] Flyback diode note for inductive loads.
  * [ ] Separate real AC/mains usage from Phase 1.

#### Filters / Regulators

* [ ] Add filter helper library.

  * [ ] Average filter.
  * [ ] Running average filter.
  * [ ] Median filter.
  * [ ] Exponential / IIR filter.
  * [ ] Integer math only by default.
  * [ ] Minimal XC8 example: `filters_demo.X`.
  * [ ] Proteus or UART-output validation.
  * [ ] C18 compatibility check where practical.

* [ ] Add PID / regulator helper library.

  * [ ] P regulator.
  * [ ] PI regulator.
  * [ ] PID regulator.
  * [ ] Output limits.
  * [ ] Integral clamp / anti-windup.
  * [ ] Integer or fixed-point math by default.
  * [ ] Minimal XC8 example: `pid_regulator.X`.
  * [ ] Proteus or UART-output validation.
  * [ ] C18 compatibility check where practical.

#### Sensors / Analog Helpers

* [ ] Add NTC thermistor helper.

  * [ ] ADC reading conversion.
  * [ ] Compact lookup-table mode.
  * [ ] Optional beta-formula mode if code size allows.
  * [ ] Integer or fixed-point math by default.
  * [ ] Minimal XC8 example: `ntc_read.X`.
  * [ ] Proteus or UART-output validation.
  * [ ] C18 compatibility check where practical.

* [ ] Add joystick helper.

  * [ ] X axis ADC read.
  * [ ] Y axis ADC read.
  * [ ] Button debounce.
  * [ ] Dead-zone handling.
  * [ ] Direction mapping.
  * [ ] Minimal XC8 example: `joystick_xy.X`.
  * [ ] Proteus validation.
  * [ ] C18 compatibility check where practical.

#### IO Expansion

* [ ] Add 74HC595 output shift register helper.

  * [ ] Bit-bang mode.
  * [ ] Optional SPI-backed mode.
  * [ ] Single chip output.
  * [ ] Multiple chained chips.
  * [ ] Minimal XC8 example: `shift_register_595.X`.
  * [ ] Proteus validation.
  * [ ] C18 compatibility check where practical.

* [ ] Add 74HC165 input shift register helper.

  * [ ] Bit-bang mode.
  * [ ] Optional SPI-backed mode.
  * [ ] Single chip input.
  * [ ] Multiple chained chips.
  * [ ] Minimal XC8 example: `shift_register_165.X`.
  * [ ] Proteus validation.
  * [ ] C18 compatibility check where practical.

#### Menu / UI Helpers

* [ ] Add menu core helper.

  * [ ] Menu item model.
  * [ ] Cursor position.
  * [ ] Enter/back navigation.
  * [ ] Value edit mode.
  * [ ] Keep display rendering separate from menu logic.
  * [ ] Minimal XC8 example: `menu_core_demo.X`.

* [ ] Add input adapter for menu navigation.

  * [ ] Button navigation.
  * [ ] Encoder navigation.
  * [ ] ADC buttons navigation.
  * [ ] Keypad navigation.
  * [ ] Minimal XC8 example: `menu_input_demo.X`.

* [ ] Add display-specific menu examples only after RAM review.

  * [ ] LCD HD44780 menu example.
  * [ ] SSD1306 menu example.
  * [ ] Seven-segment simple menu/status example.
  * [ ] Review RAM usage before adding TFT/graphics-heavy examples.

### AC Phase Control / Heater Control

* [x] Create `libraries/output/ac_phase_control/` module skeleton.
* [x] Create `docs/libraries/output/ac_phase_control.md`.
* [x] Create `docs/libraries/output/ac_phase_control.ua.md`.
* [x] Implement low-voltage zero-cross phase-control state machine.
* [x] Implement configurable gate output pulse.
* [x] Implement percent-to-phase-delay brightness control.
  * [x] Add multi-channel phase-control group support.
  * [x] Add configurable output pins per channel.
  * [x] Add independent power level per channel.
  * [x] Keep one shared zero-cross input for the group.
  * [x] Move timer selection into the phase-control library API.
  * [x] Add `ac_phase_control_init_group()` / group-based API.
  * [x] Add library-owned timer IRQ dispatch helper.
  * [x] Create `examples-projects/xc8/ac_phase_control.X`.
  * [x] Add selectable timer source in the XC8 example.
  * [x] Add non-blocking slow brightness fade example.
* [x] Create Proteus documentation:
  * [x] `docs/libraries/output/ac_phase_control.md`
  * [x] `docs/libraries/output/ac_phase_control.ua.md`
  * [x] `examples-projects/proteus/ac_phase_control/README.md`
  * [x] `examples-projects/proteus/ac_phase_control/README.ua.md`
* [ ] Validate multiple phase-control channels in Proteus.
* [ ] Validate Timer0 source in Proteus.
* [ ] Validate Timer1 source in Proteus.
* [ ] Validate Timer2 source in Proteus.
* [ ] Validate Timer3 source in Proteus.
* [ ] Validate slow brightness fade waveform in Proteus.
* [ ] Implement burst-fire heater control as a separate future module.
* [ ] Keep real mains examples out of Phase 1.
* [ ] Start with low-voltage Proteus-first validation only.
* [ ] Add safety notes before any real AC/mains documentation or examples.

### Future Output Extensions

* [ ] Add more software PWM examples.
* [ ] Add hardware PWM / CCP examples.
* [ ] Add servo output helper.
* [ ] Add relay output helper.
* [ ] Add buzzer / tone output helper.
* [ ] Add LED effects helper.
* [ ] Add MOSFET output usage notes.
