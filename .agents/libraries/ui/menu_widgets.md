# menu_widgets

## Purpose

Menu widget helpers.

## Location

```text
libraries/ui/menu_widgets/menu_widgets.h
libraries/ui/menu_widgets/menu_widgets.c
```

## Use when

- you need reusable menu widget rendering and callbacks.

## Do not use when

- the project does not use menu widgets.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `menu_widget_type_t` | enum | widget type | checkbox/toggle/progress/numeric/option/slider/text/graph | type | none |  |
| `menu_widget_t` | struct | widget state | value/limits/callbacks | state | none |  |
| `menu_widget_draw_cb_t` | function pointer typedef | draw callback | widget | none | callback |  |
| `menu_widget_update_cb_t` | function pointer typedef | update callback | widget | none | callback |  |
| `menu_widget_init` | `void menu_widget_init(...);` | initialize widget | widget, type, value | none | stores state | exact args in header |
| `menu_widget_draw` | `void menu_widget_draw(...);` | draw widget | widget, display | none | draws UI | exact args in header |
| `menu_widget_update` | `void menu_widget_update(...);` | update widget | widget | none | updates state | exact args in header |
| `menu_widget_bind_menu_item` | `void menu_widget_bind_menu_item(...);` | bind menu item | widget, item | none | stores binding | exact args in header |

## Configuration structures

Public fields are the widget struct described in the header.

## Error/status model

Not applicable for this module.

## Source inclusion strategy

### Shared implementation

```text
libraries/ui/menu_widgets/menu_widgets.c
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
libraries/ui/menu_widgets/menu_widgets.c

For C18 add:
libraries/ui/menu_widgets/menu_widgets.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/ui/menu_widgets/menu_widgets.h
libraries/ui/menu_widgets/menu_widgets.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
libraries/ui/menu_core/menu_core.h
```

### Callback-provided dependencies

```text
menu_widget_draw_cb_t, menu_widget_update_cb_t
```

### Optional feature dependencies

Not applicable for this module.

### Example-only dependencies

Not applicable for this module.

### Transitive dependencies

Not applicable for this module.

## Compile-time defines

Not applicable for this module.

## Runtime model

Widget updates and draws are synchronous.

## ISR requirements

Not applicable for this module.

## Resource ownership

Widget state is caller-owned.

## Integration recipe

1. Add the shared widget source.
2. Bind menu items.
3. Draw and update widgets from the UI loop.

## Minimal usable example

```c
menu_widget_t widget;
menu_widget_init(&widget, MENU_WIDGET_CHECKBOX, 0u);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Widget behavior depends on the bound menu item.

## Common mistakes

- Drawing without binding a menu item.

## AI decision rule

Use this module for reusable menu widgets.
