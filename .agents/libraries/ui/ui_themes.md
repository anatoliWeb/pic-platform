# ui_themes

## Purpose

UI theme helper.

## Location

```text
libraries/ui/ui_themes/ui_themes.h
libraries/ui/ui_themes/ui_themes.c
```

## Use when

- you need reusable UI color/theme selection.

## Do not use when

- the project does not use themes.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `ui_color_role_t` | enum | color role | background/foreground/accent/warning/error/success/disabled/border/count | role | none |  |
| `ui_icon_style_t` | enum | icon style | default/outline/filled | style | none |  |
| `ui_theme_t` | struct | theme state | colors/icon style | state | none |  |
| `ui_theme_get_default` | `const ui_theme_t* ui_theme_get_default(void);` | default theme | none | theme ptr | none |  |
| `ui_theme_get_dark` | `const ui_theme_t* ui_theme_get_dark(void);` | dark theme | none | theme ptr | none |  |
| `ui_theme_get_light` | `const ui_theme_t* ui_theme_get_light(void);` | light theme | none | theme ptr | none |  |
| `ui_theme_apply` | `void ui_theme_apply(...);` | apply theme | target, theme | none | updates target | exact args in header |
| `ui_theme_get_color` | `uint16_t ui_theme_get_color(...);` | get color | theme, role | color | none | exact args in header |
| `ui_theme_set_color` | `void ui_theme_set_color(...);` | set color | theme, role, color | none | mutates theme | exact args in header |
| `ui_theme_get_icon_style` | `ui_icon_style_t ui_theme_get_icon_style(...);` | get icon style | theme | style | none | exact args in header |

## Configuration structures

Public fields are the theme struct described in the header.

## Error/status model

Not applicable for this module.

## Source inclusion strategy

### Shared implementation

```text
libraries/ui/ui_themes/ui_themes.c
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
libraries/ui/ui_themes/ui_themes.c

For C18 add:
libraries/ui/ui_themes/ui_themes.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/ui/ui_themes/ui_themes.h
libraries/ui/ui_themes/ui_themes.c
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

Theme access is synchronous and caller-owned.

## ISR requirements

Not applicable for this module.

## Resource ownership

Theme state is caller-owned.

## Integration recipe

1. Add the shared UI themes source.
2. Pick or apply a theme.
3. Read or override colors as needed.

## Minimal usable example

```c
const ui_theme_t* theme = ui_theme_get_default();
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Theme values are static data unless mutated by the caller.

## Common mistakes

- Treating theme data as global mutable state.

## AI decision rule

Use this module for reusable UI themes.
