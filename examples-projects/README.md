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
│   └── ...
│
├── c18/
│   ├── blink.X/
│   ├── uart_debug.X/
│   └── ...
```

## Folder Structure Explanation

- `xc8/` contains MPLAB XC8 example projects.
- `c18/` contains MPLAB C18 example projects.
- Each `.X` folder is a standalone MPLAB X project.

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
| uart_debug | `uart_debug.X` | `uart_debug.X` | UART + debug macro validation |
| adc | `adc.X` | `adc.X` | Analog input and conversion checks |
| i2c | `i2c.X` | `i2c.X` | I2C master communication checks |
| spi | `spi.X` | `spi.X` | SPI transfer validation |
| rs485 | `rs485.X` | `rs485.X` | UART-based RS485 frame flow |
| ds18b20 | `ds18b20.X` | `ds18b20.X` | 1-Wire temperature sensor integration |

## Build Instructions (MPLAB X)

1. Open the selected `.X` project in MPLAB X.
2. Select target device and tool.
3. Select toolchain (`XC8` or `C18`) according to project folder.
4. Verify include paths point to `../pic-platform`, `../pic-platform/core`, `../pic-platform/drivers`.
5. Verify required driver `.c` files are present under **Source Files**.
6. Build the project.

## Recommended Workflow

- Keep application projects separate from the reusable library.
- Keep `pic-platform` external and shared across projects.
- Do not patch drivers per project; configure projects and paths instead.

