# blink Proteus Simulation

## Status
Real Proteus project file(s) already exist: `blink.pdsprj`, `_blink_.pdsprj`.

## Firmware

XC8 HEX:

`../../hex/xc8/blink/blink.hex`

C18 HEX:

`../../hex/c18/blink/blink.hex`

## MPLAB Projects

- `../../xc8/blink.X`
- `../../c18/blink.X`

## PIC18F452 Connections

| Signal | PIC18F452 pin | Proteus connection |
|---|---:|---|
| LED output | RB1 / pin 34 | LED + series resistor to GND |
| VDD | pins 11, 32 | +5V |
| VSS | pins 12, 31 | GND |
| MCLR | pin 1 | Reset pull-up as required |

## Expected Result

RB1 toggles every 500 ms and the LED blinks steadily.

## Notes

- Do not copy HEX into this folder.
- Rebuild the MPLAB project to refresh HEX.
- Proteus should load HEX from the shared `hex/` folder.
- Update `proteus-version.txt` after creating or saving the Proteus project.
