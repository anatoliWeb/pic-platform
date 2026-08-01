# Board Integration

Use the real example wiring and Proteus notes.

## Board rules seen in this repo

- Pin mapping belongs in the project.
- Set analog-capable pins to digital when the example uses them as GPIO.
- Disable `PORTD` PSP mode before using `PORTD` as GPIO.
- Provide the MCLR pull-up.
- Keep oscillator and config bits in the project config and `config_bits.c`.
- Connect VDD/VSS correctly and share ground across the board and peripherals.
- Use `RC6/RC7` for UART when the example expects standard UART pins.
- Track timer ownership; do not let two modules own the same timer slot.
- Put Proteus wiring details in the example or Proteus README.

## Common patterns from examples

- Seven-segment examples use `RD0..RD7` for segments and `RC0..RC3` for digits.
- `position_drive_adc` uses `RA0/AN0` for potentiometer feedback, `RD0/RD1` for H-bridge direction, and `RC6` for UART TX.
