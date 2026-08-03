[Ukrainian version](./README.ua.md)

# examples-projects

## What is examples-projects

`examples-projects` contains standalone MPLAB X wrapper projects for the external `pic-platform` codebase.

## Structure

```text
examples-projects/
|-- xc8/
|-- c18/
|-- hex/
`-- proteus/
```

XC8 projects are grouped into families under `xc8/`; standalone smoke-test projects stay at the `xc8/` root.

## Rules

- Keep the library external to the example project.
- Add only required `.c` files to `Source Files`.
- Do not add headers manually to `Source Files`.
- Keep configuration bits in `config_bits.c`.
- Keep relative include paths and source paths portable.
- Do not track MPLAB `build/`, `dist/`, or `nbproject/private/` output alongside example sources.

## Supported Compilers

- MPLAB XC8
- MPLAB C18

## Example Families

| Family | Examples | Purpose |
|---|---|---|
| Core validation (root) | `blink.X`, `gpio.X`, `adc.X`, `adc_read.X`, `timer.X`, `pwm.X`, `ccp_capture.X`, `ccp_compare.X`, `clock.X`, `comparator.X`, `crc.X`, `eeprom.X`, `ext_interrupt.X`, `portb_change.X`, `reset.X`, `ring_buffer.X`, `template.X` | Baseline integration checks and canonical smoke tests |
| Debug | `xc8/debug/default_uart.X`, `xc8/debug/display_i2c.X`, `xc8/debug/display_parallel.X`, `xc8/debug/pins_gpio.X`, `xc8/debug/multi_backend.X`, `xc8/debug/disabled.X` | Switchable debug backend demos (UART / display / GPIO / disabled) |
| Communication | `xc8/communication/uart/{basic,debug}.X`, `xc8/communication/i2c/{basic,scan}.X`, `xc8/communication/spi/{basic,loopback}.X`, `xc8/communication/rs485/{basic,echo}.X`, `xc8/communication/onewire/{basic,bus_test}.X` | UART, I2C, SPI, RS485, and OneWire protocol examples |
| Sensors | `xc8/sensors/ds18b20/{basic,read,multi_read,search_rom}.X` | DS18B20 read, multi-drop, and ROM search examples |
| Input | `xc8/input/button.X`, `xc8/input/encoder.X`, `xc8/input/adc_buttons.X` | Input helpers and debounce-related examples |
| Display | `xc8/display/lcd_hd44780/i2c_pcf8574.X`, `lcd_hd44780.X`, `xc8/seven_segment/basic_manual.X`, `xc8/seven_segment/multiplex_manual.X`, `xc8/seven_segment/multiplex_timer.X`, `xc8/seven_segment/keys_single_line.X`, `xc8/seven_segment/keys_diode_coded.X` | Display refresh, timer-owned multiplexing, and shared-line input examples |
| Actuator | `xc8/actuator/position_drive_adc.X` | Closed-loop position drive with an ADC position sensor; encoder backend is still a placeholder |
| Output / control | `xc8/output/software_pwm.X`, `xc8/output/ac_phase_control.X` | Software PWM and AC phase control output examples |
| Timers | `xc8/timers/tick.X`, `xc8/timers/timer0.X`, `xc8/timers/timer1.X`, `xc8/timers/timer2.X`, `xc8/timers/timer3.X` | Timebase and timer peripheral examples |
| System | `xc8/system/wdt.X` | Watchdog example |

## Validation Snapshot

| Example | Status |
|---|---|
| `xc8/input/button.X` | Verified in Proteus |
| `xc8/input/encoder.X` | Verified in Proteus |
| `xc8/input/adc_buttons.X` | Verified in Proteus |
| `xc8/output/software_pwm.X` | Verified in Proteus |
| `xc8/system/wdt.X` | Verified in Proteus |
| `xc8/output/ac_phase_control.X` | Ready for Proteus validation |
| `xc8/seven_segment/basic_manual.X` | Ready for Proteus validation |
| `xc8/seven_segment/multiplex_manual.X` | Ready for Proteus validation |
| `xc8/seven_segment/multiplex_timer.X` | Ready for Proteus validation |
| `xc8/seven_segment/keys_single_line.X` | Ready for Proteus validation |
| `xc8/seven_segment/keys_diode_coded.X` | Ready for Proteus validation |
| `xc8/actuator/position_drive_adc.X` | Ready for Proteus validation |
| `xc8/display/lcd_hd44780/i2c_pcf8574.X` | Ready for Proteus validation |
| `xc8/sensors/ds18b20/search_rom.X` | Verified in Proteus; XC8 HEX export pending |
| `xc8/communication/onewire/bus_test.X` | Verified in Proteus; XC8 HEX export pending |
| `xc8/sensors/ds18b20/multi_read.X` | Needs review / Proteus and C18 validation |

## Notes

- Proteus notes live under `examples-projects/proteus/`.
- Reusable HEX exports, when generated, live under `examples-projects/hex/`.
- Per-project wiring and expected behavior are documented in each example folder README.
- Console build instructions are documented in [docs/build/console-build.md](../docs/build/console-build.md).