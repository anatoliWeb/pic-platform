# uart_debug

## Purpose

Macro-based UART debug layer with compile-time enable/disable.

## Location

```text
libraries/system/uart_debug/uart_debug.h
libraries/system/uart_debug/uart_debug.c
XC8/libraries/system/uart_debug/uart_debug.c
C18/libraries/system/uart_debug/uart_debug.c
docs/libraries/system/uart_debug.md
docs/libraries/system/uart_debug.ua.md
examples-projects/xc8/seven_segment/keys_single_line.X
examples-projects/xc8/seven_segment/keys_diode_coded.X
examples-projects/xc8/actuator/position_drive_adc.X
```

## Use when

- you need low-overhead debug prints that can be compiled out;
- you already use UART in the project;
- you want a simple debug sink for reusable code.

## Do not use when

- you need a structured logger;
- you need a non-UART transport and do not plan to add an adapter;
- debug code would belong only in the project and not in reusable code.

## Public API

| Function/type | Purpose | What to pass | Returns / output | Notes |
| --- | --- | --- | --- | --- |
| `DBG_PRINT()` | write string | string literal or `const char*` | UART output or nothing | macro |
| `DBG_PRINTLN()` | write string + newline | string literal or `const char*` | UART output or nothing | macro |
| `DBG_WRITE_BYTE()` | write one byte | byte value | UART output or nothing | macro |
| `DBG_PRINT_INT()` | print integer | int value | UART output or nothing | macro |
| `DBG_PRINT_HEX()` | print byte in hex | byte value | UART output or nothing | macro |
| `dbg_print_int()` | integer print helper | int value | none | available when enabled |
| `dbg_print_hex()` | hex print helper | byte value | none | available when enabled |

## Required files

### Core/library files

```text
libraries/system/uart_debug/uart_debug.h
libraries/system/uart_debug/uart_debug.c
```

### XC8 wrapper files

```text
XC8/libraries/system/uart_debug/uart_debug.c
```

### C18 wrapper files

```text
C18/libraries/system/uart_debug/uart_debug.c
```

## Dependencies

```text
core/compiler.h
core/config.h
drivers/communication/uart/uart.h
```

## Configuration defines

| Define | Default | Where | Effect | Code size impact |
| --- | ---: | --- | --- | --- |
| `DRV_DEBUG_ENABLE` | `1` | `core/config.h` | enables macros and helpers | can be compiled out |
| `DRV_USE_UART` | `1` | `core/config.h` | selects UART transport | none when off |

## Runtime model

- When enabled, macros forward to UART helpers.
- When disabled, macros expand to empty statements.
- The library itself does not manage UART pins; the project does.

## Integration recipe

1. Add UART driver files and the debug library files.
2. Initialize UART in the project.
3. Keep `DRV_DEBUG_ENABLE` and `DRV_USE_UART` set as needed.
4. Use macros in reusable code only when the debug sink is genuinely reusable.
5. Build a targeted example.

## Minimal usage example

```c
DBG_PRINT("Start");
DBG_PRINTLN("");
DBG_PRINT_INT(123);
```

## Resource ownership

- UART hardware pins are project-owned.
- Example wiring commonly uses `RC6/TX` and `RC7/RX`.

## Examples

```text
examples-projects/xc8/seven_segment/keys_single_line.X
examples-projects/xc8/seven_segment/keys_diode_coded.X
examples-projects/xc8/actuator/position_drive_adc.X
```

## Human documentation

```text
docs/libraries/system/uart_debug.md
docs/libraries/system/uart_debug.ua.md
```

## Known limitations

- UART is the only built-in transport.

## Extension points

- Route debug callback output through application adapters if a library should not know about UART directly.

## AI decision rule

Use `uart_debug` for reusable UART debug output. Use an application-level callback if the library must stay transport-agnostic.
