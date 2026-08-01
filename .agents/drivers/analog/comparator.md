# comparator

## Purpose

Analog comparator driver for PIC18 comparator peripherals.

## Location

```text
drivers/analog/comparator/comparator.h
drivers/analog/comparator/comparator.c
XC8/drivers/analog/comparator/comparator.c
C18/drivers/analog/comparator/comparator.c
```

## Use when

- you need comparator enable/disable control;
- you need to read the comparator output in reusable code.

## Do not use when

- the project does not use the comparator peripheral.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `comparator_mode_t` | enum | comparator mode | `CMP_MODE_OFF`, `CMP_MODE_1`, `CMP_MODE_2`, `CMP_MODE_3` | mode value | none |  |
| `comparator_init` | `void comparator_init(comparator_mode_t mode);` | configure comparator mode | mode | none | configures comparator hardware |  |
| `comparator_enable` | `void comparator_enable(void);` | enable comparator | none | none | turns comparator on |  |
| `comparator_disable` | `void comparator_disable(void);` | disable comparator | none | none | turns comparator off |  |
| `comparator_get_output` | `uint8_t comparator_get_output(void);` | read comparator output | none | `0` or `1` | reads hardware state |  |

## Configuration structures

Not applicable for this module.

## Error/status model

| Status/error | Meaning | Cause | Required handling |
| --- | --- | --- | --- |
| `0/1` output | comparator output | hardware input state | branch on the sampled output |

## Source inclusion strategy

### Shared implementation

```text
drivers/analog/comparator/comparator.c
```

### XC8 implementation/wrapper

```text
XC8/drivers/analog/comparator/comparator.c
```

### C18 implementation/wrapper

```text
C18/drivers/analog/comparator/comparator.c
```

### Select exactly one route

```text
For XC8 add:
XC8/drivers/analog/comparator/comparator.c

For C18 add:
C18/drivers/analog/comparator/comparator.c

Do not add together:
drivers/analog/comparator/comparator.c and the selected compiler-specific .c

Reason:
the shared source dispatches to the compiler-specific implementation.
```

## Required files

```text
drivers/analog/comparator/comparator.h
drivers/analog/comparator/comparator.c
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
| none module-specific | n/a | n/a | comparator mode is selected at init | shared and wrapper sources | comparator peripheral | small |

## Runtime model

Comparator mode is set once and the output is sampled on demand.

## ISR requirements

Not applicable for this module.

## Resource ownership

Comparator inputs and output belong to the project configuration.

## Integration recipe

1. Add the shared comparator source or the selected compiler wrapper.
2. Initialize the desired mode.
3. Enable the peripheral.
4. Sample the output when needed.

## Minimal usable example

```c
comparator_init(CMP_MODE_1);
comparator_enable();
if (comparator_get_output() != 0u)
{
}
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

Comparator input mapping is project-specific.

## Extension points

If more comparator modes are needed, extend the reusable driver instead of duplicating local logic.

## Common mistakes

- Adding both shared and wrapper source files.
- Treating the comparator output as a GPIO input.

## AI decision rule

Use this driver for shared comparator control and output reads.
