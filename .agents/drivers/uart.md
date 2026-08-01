# uart

## Purpose

UART peripheral driver used by debug and serial examples.

## Location

```text
drivers/communication/uart/uart.h
drivers/communication/uart/uart.c
XC8/drivers/communication/uart/uart.c
C18/drivers/communication/uart/uart.c
```

## Public API

| Function/type | Purpose | What to pass | Returns / output | Notes |
| --- | --- | --- | --- | --- |
| `uart_init()` | initialize UART | baud rate | none | project selects pins in hardware |
| `uart_write_byte()` | transmit one byte | byte | none | used by debug layer |
| `uart_write_string()` | transmit a string | zero-terminated string | none | used by debug layer |
| `uart_read_byte()` | receive one byte | nothing | byte | no blocking contract stated in header |
| `uart_is_data_ready()` | check RX ready | nothing | `0`/`1` | pair with read |

## Resource ownership

- UART hardware pins are project-owned.
- Examples commonly use `RC6/RC7`.
