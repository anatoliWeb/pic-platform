# HEX Artifacts

## When to regenerate HEX

- When source code for an example changes.
- When `project_config.h` or `config_bits.c` changes.
- When a library dependency changes and the example is expected to reflect that change.

## Where build output goes

- MPLAB produces the production HEX in the example project `dist/default/production/` tree.
- The tracked artifact lives under `examples-projects/hex/`.

## What to copy

- Copy only the production HEX artifact that matches the example.
- Do not copy debug output, ELF, MAP, object files, or caches.

## Example mappings

```text
examples-projects/xc8/actuator/position_drive_adc.X/dist/default/production/position_drive_adc.X.production.hex
-> examples-projects/hex/xc8/actuator/position_drive_adc.X.production.hex
```

```text
examples-projects/xc8/seven_segment/basic_manual.X/dist/default/production/basic_manual.X.production.hex
-> examples-projects/hex/xc8/seven_segment/basic_manual.X.production.hex
```

```text
examples-projects/xc8/seven_segment/multiplex_manual.X/dist/default/production/multiplex_manual.X.production.hex
-> examples-projects/hex/xc8/seven_segment/multiplex_manual.X.production.hex
```

```text
examples-projects/xc8/seven_segment/multiplex_timer.X/dist/default/production/multiplex_timer.X.production.hex
-> examples-projects/hex/xc8/seven_segment/multiplex_timer.X.production.hex
```

```text
examples-projects/xc8/seven_segment/keys_single_line.X/dist/default/production/keys_single_line.X.production.hex
-> examples-projects/hex/xc8/seven_segment/keys_single_line.X.production.hex
```

```text
examples-projects/xc8/seven_segment/keys_diode_coded.X/dist/default/production/keys_diode_coded.X.production.hex
-> examples-projects/hex/xc8/seven_segment/keys_diode_coded.X.production.hex
```

## Verification

- Compare the tracked artifact with the freshly built production HEX.
- If byte-identical verification is needed, compare hashes or use a binary diff.
- Keep HEX refreshes in a build-focused commit, separate from code changes when practical.
