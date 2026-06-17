# wdt Proteus Simulation

## Status
Ready for Proteus validation.

## Firmware

XC8 HEX path:

`../../hex/xc8/wdt/wdt.hex`

C18 HEX is not available yet for this project.

## MPLAB Projects

- `../../xc8/wdt.X`
- `../../c18/wdt.X`

## Proteus Notes

Keep the MCU, power, and required peripheral wiring consistent with the example.

## Notes
- Do not copy HEX into this folder.
- Rebuild the MPLAB project to refresh HEX.
- Proteus should load HEX from the shared `hex/` folder.
- Update `proteus-version.txt` after creating or saving the Proteus project.
