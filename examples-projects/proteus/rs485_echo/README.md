# rs485_echo Proteus Simulation

## Status
Manual Proteus setup.

## Firmware

XC8 HEX:

`../../hex/xc8/rs485_echo/rs485_echo.hex`

C18 HEX is not available yet for this project.

## MPLAB Projects

- `../../xc8/rs485_echo.X`
- `../../c18/rs485_echo.X`

## PIC18F452 Connections

| Signal | PIC18F452 pin | Proteus connection |
|---|---:|---|
| UART TX | RC6 / pin 25 | RS485 transceiver DI |
| UART RX | RC7 / pin 26 | RS485 transceiver RO |
| RS485 DE/RE | RB2 / pin 35 | Transceiver direction control |
| VDD | pins 11, 32 | +5V |
| VSS | pins 12, 31 | GND |
| MCLR | pin 1 | Reset pull-up as required |

## Expected Result

Incoming RS485 bytes are printed to UART and echoed back through the transceiver.

## Notes

- Do not copy HEX into this folder.
- Rebuild the MPLAB project to refresh HEX.
- Proteus should load HEX from the shared `hex/` folder.
- Update `proteus-version.txt` after creating or saving the Proteus project.
