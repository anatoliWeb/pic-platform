[🇺🇦 Ukrainian version](./README.ua.md)

# examples-projects

## What is examples-projects

`examples-projects` contains standalone MPLAB X wrapper projects for the external `pic-platform` repository.

- Each `.X` folder is a self-contained project.
- The projects are used for integration checks, not as the library source tree.
- They validate the same codebase under MPLAB XC8 and MPLAB C18.
- Example folders are split by compiler.

## Project Structure

```text
examples-projects/
├── xc8/
│   ├── blink.X/
│   ├── uart_debug.X/
│   ├── adc_read.X/
│   └── ...
├── c18/
│   ├── blink.X/
│   ├── uart_debug.X/
│   ├── adc_read.X/
│   └── ...
├── hex/
│   ├── xc8/
│   └── c18/
└── proteus/
```

## Folder Structure Explanation

- `xc8/` contains MPLAB XC8 wrapper projects.
- `c18/` contains MPLAB C18 wrapper projects.
- Every `.X` folder is a separate MPLAB X project.
- Generated folders such as `build/`, `dist/`, `debug/`, and `nbproject/private/` are not tracked.

## Project Rules

- Do not copy drivers into example folders.
- Keep all paths relative.
- Keep `pic-platform` external to the example projects.
- Add required `.c` files manually through MPLAB X project metadata.
- Keep application setup in `main.c`, `project_config.h`, and `config_bits.c`.
- Keep CONFIG bits in `config_bits.c` only.

## Integration Notes

- The project wrappers depend on `../pic-platform`.
- Include paths must allow access to:
  - repository root
  - `core/`
  - `drivers/`
  - `libraries/`
- Source files are referenced in `nbproject/configurations.xml`.
- The durable source of truth is the MPLAB project metadata, not the generated `Makefile-default.mk` file.

## Supported Compilers

- MPLAB XC8
- MPLAB C18

## Example Project Families

| Family | Example wrappers | Purpose |
|---|---|---|
| Validation set | `blink.X`, `uart_debug.X`, `adc_read.X`, `i2c_scan.X`, `spi_loopback.X`, `rs485_echo.X`, `ds18b20_read.X` | Core integration and legacy validation projects |
| Common wrappers | `delay.X`, `template.X` | Simple baseline examples |
| GPIO / UART | `gpio.X`, `uart.X` | Basic peripheral access wrappers |
| Analog / input | `adc.X`, `adc_buttons.X`, `button.X`, `encoder.X` | Input and sensor-style examples |
| Displays | `lcd_hd44780.X` | Text LCD example |
| Core utilities | `ring_buffer.X`, `crc.X`, `clock.X` | Lightweight core utility wrappers |
| System / timing | `comparator.X`, `eeprom.X`, `ext_interrupt.X`, `i2c.X`, `onewire.X`, `portb_change.X`, `pwm.X`, `reset.X`, `rs485_basic.X`, `spi.X`, `tick.X`, `timer.X`, `timer0.X`, `timer1.X`, `timer2.X`, `timer3.X`, `wdt.X` | System, timing, and communication examples |
| 1-Wire / sensors | `ds18b20.X`, `ds18b20_multi_read.X`, `ds18b20_search_rom.X`, `onewire_bus_test.X` | Standalone DS18B20 wrapper and 1-Wire diagnostic examples |

The DS18B20 multi-sensor wrapper assumes one shared 1-Wire bus with a 4.7 kΩ pull-up resistor. ROM search and Match ROM support are required for real multi-drop use. Proteus validation for the multi-drop setup is still pending.

## Validation Snapshot

- Proteus README coverage is complete for the current `examples-projects/proteus/` folders.
- Static source-file audit for the XC8 `.X` projects found no missing external `pic-platform` paths.
- Pending XC8 HEX exports: `ds18b20_search_rom.X` and `onewire_bus_test.X`.
- `ds18b20_multi_read.X` still needs further Proteus and C18 follow-up.

## Example Validation Snapshot

| Example | What it tests | Proteus components / pins | Status |
|---|---|---|---|
| `blink.X` | GPIO LED blink | RB0 LED | Verified in Proteus (XC8 + 10 MHz) |
| `uart_debug.X` | Debug UART helpers | RC6/TX to Virtual Terminal RXD, RC7/RX optional | Verified in Proteus (XC8 + 10 MHz) |
| `adc_read.X` | ADC readout | Analog input source on AN0/AN1 | Verified in Proteus (XC8 + 10 MHz) |
| `ds18b20_read.X` | DS18B20 temperature read | RB1 1-Wire data line, 4.7 kΩ pull-up | Verified in Proteus (XC8 + 10 MHz) |
| `ring_buffer.X` | UART RX FIFO | UART RX/TX path | Verified in Proteus through UART output (XC8 + 10 MHz) |
| `rs485_basic.X` | RS485 frame TX | MAX487 / UART / DE-RE control | Verified in Proteus (XC8 + 10 MHz) |
| `rs485_echo.X` | Half-duplex RS485 echo | Two MAX487 nodes, UART link | Verified in Proteus (XC8 + 10 MHz) |
| `spi.X` | SPI basic output | 74HC595, SPI pins, LEDs | Verified in Proteus (XC8 + 10 MHz) |
| `spi_loopback.X` | SPI loopback self-test | RC5/SDO to RC4/SDI | Verified in Proteus (XC8 + 10 MHz) |
| `tick.X` | Millisecond tick | Timer1 ISR / Timer1 forwarding | Verified in Proteus (XC8 + 10 MHz) |
| `timer0.X` | Timer0 interrupt counter | RB0..RB3 LEDs | Verified in Proteus (XC8 + 10 MHz) |
| `timer1.X` | Timer1 interrupt counter | RB0..RB3 LEDs | Verified in Proteus (XC8 + 10 MHz) |
| `timer2.X` | Timer2 interrupt counter | RB0..RB3 LEDs | Verified in Proteus (XC8 + 10 MHz) |
| `timer3.X` | Timer3 interrupt counter | RB0..RB3 LEDs | Verified in Proteus (XC8 + 10 MHz) |
| `uart.X` | UART echo | RC6/TX to Virtual Terminal RXD, RC7/RX optional | Verified in Proteus (XC8 + 10 MHz) |
| `wdt.X` | Watchdog reset behavior | RB0 alive LED, RB1 pushbutton | Ready for Proteus validation |
| `ds18b20_search_rom.X` | DS18B20 ROM search | 1-Wire bus diagnostics | Verified in Proteus; XC8 HEX export pending |
| `onewire_bus_test.X` | Low-level OneWire bus test | 1-Wire bus diagnostics | Verified in Proteus; XC8 HEX export pending |
| `ds18b20_multi_read.X` | Multi-drop DS18B20 discovery | Shared 1-Wire bus, 4.7 kΩ pull-up | Needs review / Proteus and C18 validation |

All current Proteus folders already have matching `README.md` and `README.ua.md` files. No README gap was found during this pass.

## HEX Exports

Successful XC8 builds are exported to:

- `examples-projects/hex/xc8/<project>/<project>.hex`

Successful C18 builds are exported to:

- `examples-projects/hex/c18/<project>/<project>.hex`

Use these HEX files for Proteus or other simulator setups. Regenerate them after rebuilding the matching `.X` project.

Proteus simulations live under `examples-projects/proteus/` and use shared HEX files from `examples-projects/hex/`. Each Proteus folder contains README files and a `proteus-version.txt` note; firmware is not duplicated there.
Only successful builds are stored in the HEX folders.

This validation pass refreshed the XC8 HEX exports for `adc`, `adc_buttons`, `adc_read`, `blink`, `button`, `ccp_capture`, and `ccp_compare`.

See also: [HEX exports README](./hex/README.md).

## Build Instructions

1. Open the selected `.X` project in MPLAB X.
2. Select the target device and programmer/debugger.
3. Select the matching toolchain folder:
   - `xc8/` for MPLAB XC8
   - `c18/` for MPLAB C18
4. Verify include paths for the repository root, `core/`, `drivers/`, and `libraries/`.
5. Verify the required `.c` files are added to **Source Files**.
6. Build the project in MPLAB X.
7. Copy the exported HEX into `examples-projects/hex/` if you need a reusable simulator artifact.
8. Add Proteus circuits separately if you need schematic-level testing.

## Recommended Workflow

- Keep application projects separate from the reusable platform library.
- Keep `pic-platform` shared and external.
- Do not patch drivers per project; update project metadata instead.
- Keep source references in `nbproject/configurations.xml`.

