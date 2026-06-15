# uart_debug Proteus Simulation

## Status
Manual Proteus setup.

## Firmware

XC8 HEX:

`../../hex/xc8/uart_debug/uart_debug.hex`

C18 HEX is not available yet for this project.

## MPLAB Projects

- `../../xc8/uart_debug.X`
- `../../c18/uart_debug.X`

## PIC18F452 Connections

| Signal | PIC18F452 pin | Proteus connection |
|---|---:|---|
| UART TX | RC6 / pin 25 | Virtual Terminal RXD |
| UART RX | RC7 / pin 26 | Virtual Terminal TXD (optional) |
| VDD | pins 11, 32 | +5V |
| VSS | pins 12, 31 | GND |
| MCLR | pin 1 | Reset pull-up as required |

## Expected Result

The Virtual Terminal receives periodic debug lines at 9600 baud, 8N1.

## Notes

- Do not copy HEX into this folder.
- Rebuild the MPLAB project to refresh HEX.
- Proteus should load HEX from the shared `hex/` folder.
- Update `proteus-version.txt` after creating or saving the Proteus project.
