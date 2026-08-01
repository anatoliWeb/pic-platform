# menu_renderer

## Purpose

Menu renderer.

## Location

```text
libraries/ui/menu_renderer/menu_renderer.h
libraries/ui/menu_renderer/menu_renderer.c
```

## Use when

- you need reusable menu drawing logic.

## Do not use when

- the project does not render menus.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `menu_renderer_t` | struct | renderer state | target/theme | state | none |  |
| `menu_renderer_init` | `uint8_t menu_renderer_init(...);` | initialize renderer | state, display | success flag | stores display | exact args in header |
| `menu_renderer_draw` | `void menu_renderer_draw(...);` | draw menu | renderer, menu | none | draws UI | exact args in header |
| `menu_renderer_set_theme` | `void menu_renderer_set_theme(...);` | set theme | renderer, theme | none | updates theme | exact args in header |
| `menu_renderer_set_icons` | `void menu_renderer_set_icons(...);` | set icons | renderer, pack | none | updates icons | exact args in header |
| `menu_renderer_set_layout` | `void menu_renderer_set_layout(...);` | set layout | renderer, layout | none | updates layout | exact args in header |

## Configuration structures

Public fields are the renderer state struct described in the header.

## Error/status model

Init returns a success flag.

## Source inclusion strategy

### Shared implementation

```text
libraries/ui/menu_renderer/menu_renderer.c
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
libraries/ui/menu_renderer/menu_renderer.c

For C18 add:
libraries/ui/menu_renderer/menu_renderer.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/ui/menu_renderer/menu_renderer.h
libraries/ui/menu_renderer/menu_renderer.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
libraries/ui/menu_core/menu_core.h
libraries/ui/icon_pack/icon_pack.h
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

Renderer draws menus synchronously to the configured target.

## ISR requirements

Not applicable for this module.

## Resource ownership

Target display and theme state are caller-owned.

## Integration recipe

1. Add the shared menu renderer source.
2. Provide a display target.
3. Draw menus using the renderer.

## Minimal usable example

```c
menu_renderer_t renderer;
menu_renderer_init(&renderer, &display);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Rendering is target-specific.

## Common mistakes

- Drawing before initializing the renderer.

## AI decision rule

Use this module for reusable menu rendering.
