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

## Rules

- Keep the library external to the example project.
- Add only required `.c` files to `Source Files`.
- Do not add headers manually to `Source Files`.
- Keep configuration bits in `config_bits.c`.
- Keep relative include paths and source paths portable.

## Supported Compilers

- MPLAB XC8
- MPLAB C18

## Example Families

| Family | Examples | Purpose |
|---|---|---|
| Core validation | `blink.X`, `uart_debug.X`, `adc_read.X`, `i2c_scan.X`, `spi_loopback.X`, `rs485_echo.X`, `ds18b20_read.X` | Baseline integration checks |
| Input | `button.X`, `encoder.X`, `adc_buttons.X` | Input helpers and debounce-related examples |
| Display | `lcd_hd44780.X`, `xc8/seven_segment/basic_manual.X`, `xc8/seven_segment/multiplex_manual.X`, `xc8/seven_segment/multiplex_timer.X`, `xc8/seven_segment/keys_single_line.X`, `xc8/seven_segment/keys_diode_coded.X` | Display refresh, timer-owned multiplexing, fixed-point formatting, and shared-line input examples |
| Actuator | `xc8/actuator/position_drive_adc.X` | Closed-loop position drive with an ADC position sensor; encoder backend is still a placeholder |
| Timing and system | `tick.X`, `timer0.X`, `timer1.X`, `timer2.X`, `timer3.X`, `wdt.X`, `software_pwm.X`, `ac_phase_control.X` | Timers, watchdog, and platform extensions |
| 1-Wire and sensors | `ds18b20.X`, `ds18b20_search_rom.X`, `onewire_bus_test.X`, `ds18b20_multi_read.X` | DS18B20 and bus diagnostics |

## Validation Snapshot

| Example | Status |
|---|---|
| `button.X` | Verified in Proteus |
| `encoder.X` | Verified in Proteus |
| `adc_buttons.X` | Verified in Proteus |
| `software_pwm.X` | Verified in Proteus |
| `wdt.X` | Verified in Proteus |
| `ac_phase_control.X` | Ready for Proteus validation |
| `xc8/seven_segment/basic_manual.X` | Ready for Proteus validation |
| `xc8/seven_segment/multiplex_manual.X` | Ready for Proteus validation |
| `xc8/seven_segment/multiplex_timer.X` | Ready for Proteus validation |
| `xc8/seven_segment/keys_single_line.X` | Ready for Proteus validation |
| `xc8/seven_segment/keys_diode_coded.X` | Ready for Proteus validation |
| `xc8/actuator/position_drive_adc.X` | Ready for Proteus validation |
| `ds18b20_search_rom.X` | Verified in Proteus; XC8 HEX export pending |
| `onewire_bus_test.X` | Verified in Proteus; XC8 HEX export pending |
| `ds18b20_multi_read.X` | Needs review / Proteus and C18 validation |

## Notes

- Proteus notes live under `examples-projects/proteus/`.
- Reusable HEX exports, when generated, live under `examples-projects/hex/`.
- Per-project wiring and expected behavior are documented in each example folder README.
- Console build instructions are documented in [docs/build/console-build.md](../docs/build/console-build.md).

