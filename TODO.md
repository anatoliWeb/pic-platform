# TODO

## Current priorities

- [x] LCD HD44780 I2C transport finalized: hardened `lcd_i2c.c`/`lcd_i2c.h` (status API, 7-bit address validation including `0x00`, configurable PCF8574 pin mapping with single-bit mask validation, NACK short-circuit + ready gate, `attach` = probe-only bind, `controller_init(void)` without address), direct example `xc8/display/lcd_hd44780/i2c_pcf8574.X` (init status check + fail state), debug display adapter ready route, full MPLAB `define-macros` for the debug display projects; verified with XC8 builds and XC8 compile/preprocess behavior tests.
- [ ] Re-run MPLAB XC8 clean/build for every moved XC8 project after the family regrouping (paths changed by one level).
- [ ] Re-run `python -m unittest scripts.tests.test_examples_hierarchy` and `scripts/tests/test_debug_examples.py` after each build batch.
- [ ] Review legacy duplicate headers in `libraries/display/lcd_hd44780/` (`lcd.h` vs `lcd_hd44780.h`).
- [ ] Review generic/legacy timer folder `drivers/timers/timer/` for overlap with `timer0..timer3`.

## Build and compiler validation

- [ ] MPLAB XC8: build all XC8 `.X` projects and export HEX to `examples-projects/hex/xc8/<family>/<project>/`.
- [ ] MPLAB C18: build validation was attempted for 40 C18 example projects; 13 passed and 27 failed with project-specific compile/link issues. Not resolved; do not mark complete until the failures are triaged.
- [ ] Validate whether C18 accepts macros inside `#pragma config`; keep direct literal CONFIG bits in C18 templates until confirmed.
- [ ] Validate memory usage for PIC18F452 (graphics, OLED, TFT, sprite, animation modules).

## Examples and simulations

- [ ] Regenerate/export XC8 HEX for `xc8/sensors/ds18b20/search_rom.X` and `xc8/communication/onewire/bus_test.X` (deferred).
- [ ] Validate `xc8/sensors/ds18b20/multi_read.X` in Proteus and C18 (needs review).
- [ ] Proteus: add UART debug, LCD, and DS18B20 multi-sensor projects where practical; document HEX loading steps.
- [ ] Add Proteus validation for AC phase control channels, timer sources (Timer0..3), and slow brightness fade.
- [ ] Validate timing-sensitive modules on hardware or Proteus: 1-Wire, DHT, WS2812, IR receiver.
- [ ] Create `.X` example projects for display/UI demos after RAM review.

## Libraries and drivers

- [ ] Validate all `core/`, `drivers/`, and `libraries/` modules with MPLAB XC8 and C18.
- [ ] Extend software PWM timer-source validation for Timer0, Timer1, and Timer3.
- [ ] Implement optional FreeRTOS backend hooks; add mutex and queue abstractions once a real backend is selected.
- [ ] Migrate library delay calls to `rtos_sleep_ms()` where scheduler safety matters.
- [ ] Implement real 1-Wire Search ROM timing model split (real microseconds vs write tuning ticks).
- [ ] Add queued button events for timer-driven shared-line segment key scanning.

## Documentation

- [ ] Keep documentation links synchronized after module moves.
- [ ] Add per-module "tested with" status once builds/hardware checks are done.
- [ ] Add safety notes before any real AC/mains examples or documentation.
- [ ] Document Proteus setup and expected HEX loading steps.

## Future / optional

- [ ] Gyver-style helper expansion: button/encoder/ADC-buttons/keypad helpers, millis/elapsed helpers, relay, buzzer, servo, LED effects, filters, PID, NTC, joystick, 74HC595/74HC165 shift registers, menu core + input adapter.
- [ ] Phase 2 menu libraries: encoder menu demo, button/encoder navigation layer, LCD/OLED menu libraries, display-specific menu examples.
- [ ] Burst-fire heater control as a separate future module.
- [ ] Board/profile configuration templates for more PIC18 devices.
- [ ] More complete MPLAB `.X` example projects and Proteus coverage.

## Completed snapshot

History is captured in audit and validation docs:

- [x] `docs/audits/phase-1-examples-validation.md` — Proteus-verified set: blink, uart debug, adc_read, ds18b20_read, ring_buffer, rs485_basic, rs485_echo, spi, spi_loopback, tick, timer0..3, uart, wdt, button, encoder, adc_buttons, software_pwm; XC8 HEX exports recorded under `examples-projects/hex/`.
- [x] XC8 example projects regrouped into families: `debug/`, `communication/{uart,i2c,spi,rs485,onewire}/`, `sensors/ds18b20/`, `input/`, `output/`, `timers/`, `system/`; root keeps standalone validation projects.
- [x] MPLAB `.X` projects use external `pic-platform` paths only; `build/`, `dist/`, and `nbproject/private/` outputs are not tracked.
- [x] Project configuration layer: `project_config.h` + `config_bits.c` templates for XC8 and C18, config checker, library CONFIG requirement conventions.
- [x] Reusable libraries documented in `docs/` (EN/UA) and catalogued in `.agents/`.
- [x] Configuration contract split into Category A/B/C, with MPLAB `define-macros` and target tests covering debug/LCD/build config drift.
