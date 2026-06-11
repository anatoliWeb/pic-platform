[🇺🇦 Українська версія](./README.ua.md)

# examples-projects

## What is examples-projects

`examples-projects` contains complete MPLAB X example projects for validating and testing the external `pic-platform` driver library.

- Includes full project-level examples, not just source snippets.
- Used to test driver integration in real MPLAB project setups.
- Used to validate compatibility with both XC8 and C18.
- Examples are separated by compiler.

## Project Structure

```text
examples-projects/
├── xc8/
│   ├── blink.X/
│   ├── uart_debug.X/
│   ├── adc_read.X/
│   ├── i2c_scan.X/
│   ├── spi_loopback.X/
│   ├── rs485_echo.X/
│   ├── ds18b20_read.X/
│   └── ...
│
├── c18/
│   ├── blink.X/
│   ├── uart_debug.X/
│   ├── adc_read.X/
│   ├── i2c_scan.X/
│   ├── spi_loopback.X/
│   ├── rs485_echo.X/
│   ├── ds18b20_read.X/
│   └── ...
```

## Folder Structure Explanation

- `xc8/` contains MPLAB XC8 example projects.
- `c18/` contains MPLAB C18 example projects.
- Each `.X` folder is a standalone MPLAB X project.
- Generated MPLAB folders such as `build/`, `dist/`, and `debug/` are
  intentionally not stored in the repository.

## Important Rules

- Do not copy drivers into example project folders.
- Use relative paths only.
- Projects must use the external `../pic-platform` library.
- Add required driver `.c` files manually in MPLAB X.

## Integration Notes

- Projects depend on `../pic-platform`.
- Include paths are configured in MPLAB project properties.
- Driver implementation files (`.c`) must be added to **Source Files**.

## Supported Compilers

- MPLAB XC8
- MPLAB C18

## Example Projects

| Example | XC8 | C18 | Purpose |
|---|---|---|---|
| blink | `blink.X` | `blink.X` | GPIO output sanity check |
| uart_debug | `uart_debug.X` | `uart_debug.X` | UART and debug helper validation |
| adc_read | `adc_read.X` | `adc_read.X` | ADC input read example |
| i2c_scan | `i2c_scan.X` | `i2c_scan.X` | I2C bus scan example |
| spi_loopback | `spi_loopback.X` | `spi_loopback.X` | SPI transfer loopback example |
| rs485_echo | `rs485_echo.X` | `rs485_echo.X` | RS485 byte echo example |
| ds18b20_read | `ds18b20_read.X` | `ds18b20_read.X` | 1-Wire temperature sensor example |

## Build Instructions (MPLAB X)

1. Open the selected `.X` project in MPLAB X.
2. Select target device and tool.
3. Select toolchain (`XC8` or `C18`) according to project folder.
4. Verify include paths point to `../pic-platform`, `../pic-platform/core`, `../pic-platform/drivers`.
5. Verify required driver `.c` files are present under **Source Files**.
6. Build the project.
7. Add Proteus simulations later manually if you need circuit-level testing.

## Recommended Workflow

- Keep application projects separate from the reusable library.
- Keep `pic-platform` external and shared across projects.
- Do not patch drivers per project; configure projects and paths instead.
