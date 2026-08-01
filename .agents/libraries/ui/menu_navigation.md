# menu_navigation

## Purpose

Menu navigation helper.

## Location

```text
libraries/ui/menu_navigation/menu_navigation.h
libraries/ui/menu_navigation/menu_navigation.c
```

## Use when

- you need reusable menu navigation input handling.

## Do not use when

- the project does not use menu navigation.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `menu_navigation_t` | struct | navigation state | current input | state | none |  |
| `menu_navigation_init` | `uint8_t menu_navigation_init(...);` | initialize nav | state, menu | success flag | stores menu | exact args in header |
| `menu_navigation_update` | `void menu_navigation_update(...);` | process input | nav | none | changes menu selection | exact args in header |
| `menu_navigation_set_enabled` | `void menu_navigation_set_enabled(...);` | enable/disable | nav, flag | none | toggles handling | exact args in header |
| `menu_navigation_reset` | `void menu_navigation_reset(...);` | reset nav | nav | none | resets state | exact args in header |

## Configuration structures

Public fields are the navigation state struct described in the header.

## Error/status model

Init returns a success flag.

## Source inclusion strategy

### Shared implementation

```text
libraries/ui/menu_navigation/menu_navigation.c
```

### XC8 implementation/wrapper

```text
Not applicable for this module.
```

### C18 implementation/wrapper

```text
Not applicable for this module.
```

### Select exactly one route

```text
For XC8 add:
libraries/ui/menu_navigation/menu_navigation.c

For C18 add:
libraries/ui/menu_navigation/menu_navigation.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/ui/menu_navigation/menu_navigation.h
libraries/ui/menu_navigation/menu_navigation.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
libraries/ui/menu_core/menu_core.h
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

Not applicable for this module.

## Runtime model

Navigation updates synchronously from input events.

## ISR requirements

Not applicable for this module.

## Resource ownership

Menu navigation state is caller-owned.

## Integration recipe

1. Add the shared menu navigation source.
2. Bind a menu and input source.
3. Update on each loop tick.

## Minimal usable example

```c
menu_navigation_t nav;
menu_navigation_init(&nav, &menu);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Input source is caller-managed.

## Common mistakes

- Updating without a menu tree.

## AI decision rule

Use this module for reusable menu navigation.
