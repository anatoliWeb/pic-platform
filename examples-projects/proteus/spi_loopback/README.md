# spi_loopback Proteus Simulation

## Status
Manual Proteus setup.

## Firmware

XC8 HEX:

`../../hex/xc8/spi_loopback/spi_loopback.hex`

C18 HEX is not available yet for this project.

## MPLAB Projects

- `../../xc8/spi_loopback.X`
- `../../c18/spi_loopback.X`

## PIC18F452 Connections

| Signal | PIC18F452 pin | Proteus connection |
|---|---:|---|
| SPI SCK | RC3 / pin 18 | SPI clock line |
| SPI SDI | RC4 / pin 23 | Connect to SPI SDO for loopback |
| SPI SDO | RC5 / pin 24 | Connect to SPI SDI for loopback |
| VDD | pins 11, 32 | +5V |
| VSS | pins 12, 31 | GND |
| MCLR | pin 1 | Reset pull-up as required |

## Expected Result

The UART prints the test byte echoed back from the SPI loopback wiring.

## Notes

- Do not copy HEX into this folder.
- Rebuild the MPLAB project to refresh HEX.
- Proteus should load HEX from the shared `hex/` folder.
- Update `proteus-version.txt` after creating or saving the Proteus project.
