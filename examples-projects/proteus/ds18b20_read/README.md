# ds18b20_read Proteus Simulation

## Status
Verified in Proteus (XC8 + 10 MHz).

## Firmware

XC8 HEX:

`../../hex/xc8/ds18b20_read/ds18b20_read.hex`

C18 HEX is not available yet for this project.

## MPLAB Projects

- `../../xc8/sensors/ds18b20/read.X`
- `../../c18/ds18b20_read.X`

## PIC18F452 Connections

| Signal | PIC18F452 pin | Proteus connection |
|---|---:|---|
| DS18B20 DQ | RB1 / pin 34 | 1-Wire data line with 4.7k pull-up to +5V |
| VDD | pins 11, 32 | +5V |
| VSS | pins 12, 31 | GND |
| MCLR | pin 1 | Reset pull-up as required |

## Expected Result

The UART prints valid DS18B20 temperatures using the single-sensor read flow.

## Notes

- Do not copy HEX into this folder.
- Rebuild the MPLAB project to refresh HEX.
- Proteus should load HEX from the shared `hex/` folder.
- Update `proteus-version.txt` after creating or saving the Proteus project.
