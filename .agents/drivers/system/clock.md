# clock

## Purpose

Clock frequency accessors.

## Location

```text
drivers/system/clock/clock.h
drivers/system/clock/clock.c
XC8/drivers/system/clock/clock.c
C18/drivers/system/clock/clock.c
```

## Use when

- you need runtime access to MCU clock frequencies.

## Do not use when

- compile-time clock macros are sufficient.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `CLOCK_FREQUENCY` | macro | oscillator frequency | none | constant | none |  |
| `CLOCK_INSTRUCTION_FREQUENCY` | macro | instruction clock | none | constant | none |  |
| `clock_get_frequency` | `uint32_t clock_get_frequency(void);` | get clock | none | Hz | none |  |
| `clock_get_frequency_hz` | `uint32_t clock_get_frequency_hz(void);` | get clock | none | Hz | none |  |
| `clock_get_frequency_khz` | `uint32_t clock_get_frequency_khz(void);` | get clock | none | kHz | none |  |
| `clock_get_frequency_mhz` | `uint32_t clock_get_frequency_mhz(void);` | get clock | none | MHz | none |  |
| `clock_get_instruction_frequency_hz` | `uint32_t clock_get_instruction_frequency_hz(void);` | get instruction clock | none | Hz | none |  |
| `clock_get_instruction_frequency_khz` | `uint32_t clock_get_instruction_frequency_khz(void);` | get instruction clock | none | kHz | none |  |
| `clock_get_instruction_frequency_mhz` | `uint32_t clock_get_instruction_frequency_mhz(void);` | get instruction clock | none | MHz | none |  |

## Configuration structures

Not applicable for this module.

## Error/status model

Not applicable for this module.

## Source inclusion strategy

### Shared implementation

```text
drivers/system/clock/clock.c
```

### XC8 implementation/wrapper

```text
XC8/drivers/system/clock/clock.c
```

### C18 implementation/wrapper

```text
C18/drivers/system/clock/clock.c
```

### Select exactly one route

```text
For XC8 add:
XC8/drivers/system/clock/clock.c

For C18 add:
C18/drivers/system/clock/clock.c

Do not add together:
shared clock.c and the selected wrapper .c

Reason:
the wrapper includes the shared implementation.
```

## Required files

```text
drivers/system/clock/clock.h
drivers/system/clock/clock.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
```

### Callback-provided dependencies

Not applicable for this module.

### Optional feature dependencies

Not applicable for this module.

### Example-only dependencies

Not applicable for this module.

### Transitive dependencies

Not applicable for this module.

## Compile-time defines

| Define | Default | Where | Effect | Required files | Resource impact | Code-size impact |
| --- | ---: | --- | --- | --- | --- | --- |
| `CLOCK_FREQUENCY` | project-specific | header/build | oscillator frequency | shared/wrapper source | compile-time constant | none |
| `CLOCK_INSTRUCTION_FREQUENCY` | derived | header/build | instruction clock | shared/wrapper source | compile-time constant | none |

## Runtime model

Read-only clock accessors.

## ISR requirements

Not applicable for this module.

## Resource ownership

Clock configuration belongs to the project.

## Integration recipe

1. Add the shared clock source or the selected wrapper.
2. Include the header where clock access is needed.
3. Use the accessors for derived timing.

## Minimal usable example

```c
uint32_t hz = clock_get_frequency_hz();
```

## Working examples

Not applicable for this module.

## Proteus integration

Not applicable for this module.

## HEX artifacts

Not applicable for this module.

## Human documentation

Not applicable for this module.

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Depends on project clock configuration.

## Extension points

Keep derived timing logic in reusable helpers that consume these accessors.

## Common mistakes

- Treating the driver as mutable runtime clock control.
- Compiling wrapper and shared source together.

## AI decision rule

Use this driver for read-only clock frequency access.
