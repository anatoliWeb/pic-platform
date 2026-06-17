# ds18b20_multi_read Proteus Simulation

## Status
Needs review / Proteus and C18 validation.

## Firmware

XC8 HEX path:

`../../hex/xc8/ds18b20_multi_read/ds18b20_multi_read.hex`

C18 HEX is not available yet for this project.

## MPLAB Projects

- `../../xc8/ds18b20_multi_read.X`
- `../../c18/ds18b20_multi_read.X`

## Proteus Notes

Use a 4.7k pull-up resistor on the shared 1-Wire data line.

## Notes
- Do not copy HEX into this folder.
- Rebuild the MPLAB project to refresh HEX.
- Proteus should load HEX from the shared `hex/` folder.
- Update `proteus-version.txt` after creating or saving the Proteus project.
