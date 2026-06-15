# adc_read Proteus Simulation

## Status
Manual Proteus setup.

## Firmware

XC8 HEX:

`../../hex/xc8/adc_read/adc_read.hex`

C18 HEX is not available yet for this project.

## MPLAB Projects

- `../../xc8/adc_read.X`
- `../../c18/adc_read.X`

## PIC18F452 Connections

| Signal | PIC18F452 pin | Proteus connection |
|---|---:|---|
| ADC input | RA0 / AN0 / pin 2 | Potentiometer wiper or analog voltage source |
| VDD | pins 11, 32 | +5V |
| VSS | pins 12, 31 | GND |
| MCLR | pin 1 | Reset pull-up as required |

## Expected Result

The UART prints raw ADC0 values while the analog input is changed.

## Notes

- Do not copy HEX into this folder.
- Rebuild the MPLAB project to refresh HEX.
- Proteus should load HEX from the shared `hex/` folder.
- Update `proteus-version.txt` after creating or saving the Proteus project.
