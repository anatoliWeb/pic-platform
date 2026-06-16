# Exported HEX Files

This folder stores HEX files exported from successful XC8 and C18 example builds.

- Path: `examples-projects/hex/xc8/<project>/<project>.hex`
- Path: `examples-projects/hex/c18/<project>/<project>.hex`
- These files are intended for Proteus and other PIC18 simulation setups.
- Regenerate them after rebuilding the matching `.X` project.
- Only successful builds should produce files here.

The current XC8 validation pass refreshed the HEX exports for `adc`, `adc_buttons`, `adc_read`, `blink`, `button`, `ccp_capture`, and `ccp_compare`.

See also:
- [examples-projects README](../README.md)
- [examples-projects README (UA)](../README.ua.md)
