# button

## Purpose

Non-blocking active-low button driver with debounce, click, double-click, hold, and hold-repeat support.

## Location

```text
libraries/input/button/button.h
libraries/input/button/button.c
XC8/libraries/input/button/button.c
C18/libraries/input/button/button.c
docs/libraries/input/button.md
docs/libraries/input/button.ua.md
examples-projects/xc8/input/button.X
examples-projects/xc8/seven_segment/keys_single_line.X
examples-projects/xc8/seven_segment/keys_diode_coded.X
```

## Use when

- you need direct GPIO buttons;
- you need external raw-state button events;
- you want debounce and higher-level click/hold semantics in reusable code.

## Do not use when

- you only need raw port reads;
- you want to encode button semantics in a project-local scanner;
- you do not have a tick source.

## Public API

| Function/type | Purpose | What to pass | Returns / output | Notes |
| --- | --- | --- | --- | --- |
| `button_t` | button state storage | nothing | struct | stores timing and event flags |
| `button_init()` | direct GPIO-backed button init | button, port, tris, pin | none | reads pin state from hardware |
| `button_init_external()` | init with external raw state | button, initial raw state | none | used by shared-line decoders |
| `button_set_raw_state()` | update external raw state | button, raw state | none | `0`/`1` raw input |
| `button_update()` | advance state machine | button | none | call regularly |
| `button_pressed()` / `released()` | edge events | button | `uint8_t` | consume flags |
| `button_clicked()` / `double_clicked()` | click events | button | `uint8_t` | consume flags |
| `button_held()` / `hold_repeated()` | hold events | button | `uint8_t` | consume flags |
| `button_get_click_count()` | click counter | button | `uint8_t` | inspect count |
| `button_is_clicked()` / `is_held()` | state query helpers | button | `uint8_t` | non-consuming helpers |

## Required files

### Core/library files

```text
libraries/input/button/button.h
libraries/input/button/button.c
```

### XC8 wrapper files

```text
XC8/libraries/input/button/button.c
```

### C18 wrapper files

```text
C18/libraries/input/button/button.c
```

## Source inclusion strategy

### Shared implementation

```text
libraries/input/button/button.c
```

### XC8 implementation

```text
XC8/libraries/input/button/button.c
```

### C18 implementation

```text
C18/libraries/input/button/button.c
```

### Select exactly one route

```text
For XC8 add:
XC8/libraries/input/button/button.c

For C18 add:
C18/libraries/input/button/button.c

Shared route:
libraries/input/button/button.c

Do not add together:
shared button.c and the selected compiler-specific .c

Reason:
the compiler-specific sources are independent implementations of the same API.
```

## Dependencies

```text
drivers/timers/tick/tick.h
core/types.h
core/compiler.h
```

## Configuration defines

| Define | Default | Where | Effect | Code size impact |
| --- | ---: | --- | --- | --- |
| none module-specific | n/a | n/a | timing comes from `tick` | none |

## Runtime model

- `button_update()` advances debounce and event state.
- Active-low logic is used: `0 = pressed`, `1 = released`.
- External-state mode lets another library own the electrical scan while `button` keeps event logic.

## Integration recipe

1. Add the button files and the tick dependency.
2. Initialize with `button_init()` for direct pins or `button_init_external()` for decoded input.
3. Call `button_update()` from the main loop.
4. Read click/hold helpers when needed.
5. Build a targeted example.

## Minimal usage example

```c
button_t btn;

button_init(&btn, &PORTB, &TRISB, 0u);

while (1)
{
    button_update(&btn);

    if (button_clicked(&btn))
    {
    }
}
```

## Resource ownership

- Direct GPIO button pins are project-owned.
- External-state mode is used by shared-line decoders.
- Timing depends on `tick` and therefore Timer1.

## Examples

```text
examples-projects/xc8/input/button.X
examples-projects/xc8/seven_segment/keys_single_line.X
examples-projects/xc8/seven_segment/keys_diode_coded.X
```

## Human documentation

```text
docs/libraries/input/button.md
docs/libraries/input/button.ua.md
```

## Known limitations

- Active-low behavior is fixed in current code.

## Extension points

- Reuse `button_init_external()` for any new electrical scanner rather than duplicating debounce logic.

## AI decision rule

If a task needs button semantics, keep the logic in `button` or feed decoded raw state into it.
