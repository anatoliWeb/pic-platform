# icon_pack

## Purpose

Icon pack helper.

## Location

```text
libraries/ui/icon_pack/icon_pack.h
libraries/ui/icon_pack/icon_pack.c
```

## Use when

- you need reusable UI icon lookup/drawing helpers.

## Do not use when

- the project does not use UI icons.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `ui_icon_id_t` | enum | icon id | `UI_ICON_SETTINGS ... UI_ICON_COUNT` | id | none |  |
| `ui_icon_t` | struct | icon data | bitmap/size | state | none |  |
| `icon_get` | `const ui_icon_t* icon_get(ui_icon_id_t id);` | lookup icon | id | icon ptr | none | exact args in header |
| `icon_draw` | `void icon_draw(...);` | draw icon | target, icon, position | none | drawing | exact args in header |
| `icon_draw_scaled` | `void icon_draw_scaled(...);` | draw scaled icon | target, icon, position, scale | none | drawing | exact args in header |
| `icon_draw_inverted` | `void icon_draw_inverted(...);` | draw inverted icon | target, icon, position | none | drawing | exact args in header |

## Configuration structures

Public fields are the `ui_icon_t` members described in the header.

## Error/status model

Not applicable for this module.

## Source inclusion strategy

### Shared implementation

```text
libraries/ui/icon_pack/icon_pack.c
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
libraries/ui/icon_pack/icon_pack.c

For C18 add:
libraries/ui/icon_pack/icon_pack.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/ui/icon_pack/icon_pack.h
libraries/ui/icon_pack/icon_pack.c
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

Not applicable for this module.

## Runtime model

Icon lookup and draw helpers are synchronous.

## ISR requirements

Not applicable for this module.

## Resource ownership

Icon data belongs to the package.

## Integration recipe

1. Add the shared icon pack source.
2. Look up icons.
3. Draw them through the selected UI target.

## Minimal usable example

```c
const ui_icon_t* icon = icon_get(UI_ICON_SETTINGS);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Icon set is fixed by the package.

## Common mistakes

- Requesting an out-of-range icon id.

## AI decision rule

Use this module for reusable UI icon access.
