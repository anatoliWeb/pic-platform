# i2c_scan Proteus Simulation

## Status
Manual Proteus setup.

## Firmware

XC8 HEX:

`../../hex/xc8/i2c_scan/i2c_scan.hex`

C18 HEX is not available yet for this project.

## MPLAB Projects

- `../../xc8/i2c_scan.X`
- `../../c18/i2c_scan.X`

## PIC18F452 Connections

| Signal | PIC18F452 pin | Proteus connection |
|---|---:|---|
| I2C SCL | RC3 / pin 18 | I2C SCL line with pull-up resistor |
| I2C SDA | RC4 / pin 23 | I2C SDA line with pull-up resistor |
| VDD | pins 11, 32 | +5V |
| VSS | pins 12, 31 | GND |
| MCLR | pin 1 | Reset pull-up as required |

## Expected Result

The UART prints the number of detected I2C devices and their addresses.

## Notes

- Do not copy HEX into this folder.
- Rebuild the MPLAB project to refresh HEX.
- Proteus should load HEX from the shared `hex/` folder.
- Update `proteus-version.txt` after creating or saving the Proteus project.
