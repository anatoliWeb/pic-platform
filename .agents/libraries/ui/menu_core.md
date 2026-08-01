# menu_core

## Purpose

Core menu model.

## Location

```text
libraries/ui/menu_core/menu_core.h
libraries/ui/menu_core/menu_core.c
```

## Use when

- you need reusable menu tree state and navigation.

## Do not use when

- the project does not use menus.

## Public API

| Function/type/macro | Exact signature/type | Purpose | Arguments | Returns/output | Side effects | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `menu_action_cb_t` | function pointer typedef | action callback | menu event | none | callback |  |
| `menu_item_type_t` | enum | item type | `MENU_ITEM_ACTION`, `MENU_ITEM_SUBMENU`, `MENU_ITEM_TOGGLE`, `MENU_ITEM_VALUE` | type | none |  |
| `menu_item_t` | struct | menu item | label/action/children | state | none |  |
| `menu_t` | struct | menu state | root/current/index | state | none |  |
| `menu_init` | `void menu_init(...);` | initialize menu | menu, items | none | stores root | exact args in header |
| `menu_set_root` | `void menu_set_root(...);` | set root | menu, item | none | updates root | exact args in header |
| `menu_get_current` | `menu_item_t* menu_get_current(...);` | current item | menu | item ptr | none | exact args in header |
| `menu_next` | `void menu_next(...);` | next item | menu | none | updates selection | exact args in header |
| `menu_prev` | `void menu_prev(...);` | previous item | menu | none | updates selection | exact args in header |
| `menu_enter` | `void menu_enter(...);` | enter submenu | menu | none | updates current path | exact args in header |
| `menu_back` | `void menu_back(...);` | back | menu | none | updates current path | exact args in header |
| `menu_select` | `void menu_select(...);` | select item | menu | none | triggers action | exact args in header |
| `menu_reset` | `void menu_reset(...);` | reset menu | menu | none | resets state | exact args in header |

## Configuration structures

Public fields are the menu state structs described in the header.

## Error/status model

Not applicable for this module.

## Source inclusion strategy

### Shared implementation

```text
libraries/ui/menu_core/menu_core.c
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
libraries/ui/menu_core/menu_core.c

For C18 add:
libraries/ui/menu_core/menu_core.c

Do not add together:
not applicable

Reason:
this module is independent.
```

## Required files

```text
libraries/ui/menu_core/menu_core.h
libraries/ui/menu_core/menu_core.c
```

## Dependency classification

### Direct compile-time dependencies

```text
core/compiler.h
core/types.h
```

### Callback-provided dependencies

```text
menu_action_cb_t
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

Menu state updates synchronously on navigation calls.

## ISR requirements

Not applicable for this module.

## Resource ownership

Menu tree ownership stays with the caller.

## Integration recipe

1. Add the shared menu core source.
2. Define items and root.
3. Navigate and select.

## Minimal usable example

```c
menu_t menu;
menu_init(&menu, items);
```

## Validation status

Implemented in source; validation evidence not tracked in `.agents/`.

## Known limitations

Tree structure is caller-owned.

## Common mistakes

- Passing invalid menu nodes.

## AI decision rule

Use this module for reusable menu tree state.
