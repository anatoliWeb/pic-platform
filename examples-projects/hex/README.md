# PIC18F452 Reset cause test

This example tests the `reset` driver and prints why the PIC18F452 started or restarted.

The idea is simple: after every startup, the firmware reads the MCU reset flags, prints the reset cause over UART, clears the flags, and waits for the next reset.

## Purpose

The reset driver is useful for understanding what happened before the firmware started:

- power was applied;
- the `MCLR` reset button was pressed;
- the Watchdog Timer reset the MCU;
- a Brown-out Reset occurred;
- a software reset instruction was executed.

This is useful for real-device diagnostics. If a board restarts unexpectedly, the reset cause can help determine whether the issue is power, firmware lockup, watchdog, or an external reset signal.

## What this example does

At startup, `main.c` does the following:

1. initializes UART at `9600 baud`;
2. initializes the reset driver with `reset_init()`;
3. reads the reset cause with `reset_get_cause()`;
4. prints the numeric and text reset cause to the Virtual Terminal;
5. clears reset flags with `reset_clear_flags()`;
6. waits for the next reset.

After pressing the `MCLR` button, the MCU restarts and the example prints the reset cause again.

## Important note for PIC18F452

PIC18F452 does not provide one simple dedicated `MCLR_RESET_FLAG`. The driver uses this practical detection order:

```text
POR == 0 -> POWER_ON
BOR == 0 -> BROWN_OUT
TO  == 0 -> WATCHDOG
RI  == 0 -> SOFTWARE
otherwise -> EXTERNAL_MCLR
```

So `EXTERNAL_MCLR` is used as a fallback when there is no Power-on, Brown-out, Watchdog, or Software reset indication.

## Reset causes

| Code | Name | Meaning |
|---:|---|---|
| 0 | `RESET_CAUSE_UNKNOWN` | unknown cause |
| 1 | `RESET_CAUSE_POWER_ON` | startup after power was applied |
| 2 | `RESET_CAUSE_BROWN_OUT` | reset caused by voltage drop |
| 3 | `RESET_CAUSE_WATCHDOG` | reset caused by Watchdog Timer |
| 4 | `RESET_CAUSE_EXTERNAL` | external reset, `MCLR` in this example |
| 5 | `RESET_CAUSE_SOFTWARE` | software reset instruction |

## Proteus wiring

| Element | PIC18F452 | DIP-40 pin | Purpose |
|---|---|---:|---|
| MCLR pull-up | `MCLR` -> `10k` -> `+5V` | 1 | normal MCU operation |
| Reset button | `MCLR` -> button -> `GND` | 1 | manual reset |
| UART TX | `RC6/TX` -> Virtual Terminal `RXD` | 25 | debug output |
| UART RX | `RC7/RX` <- Virtual Terminal `TXD` | 26 | optional, full UART wiring |
| VDD | `+5V` | 11, 32 | power |
| VSS | `GND` | 12, 31 | ground |
| Crystal | `OSC1/OSC2` | 13, 14 | clock, for example `10 MHz` |

Reset button wiring:

```text
+5V
 |
10k
 |
MCLR pin 1 ---- button ---- GND
```

## Expected UART output

On first simulation start:

```text
PIC18F452 Reset cause test
MCLR: pin 1 -> 10k -> +5V
MCLR button: pin 1 -> button -> GND

Reset cause code=1 POWER_ON

Reset flags cleared.
Press MCLR button in Proteus to test external reset.
```

After pressing the `MCLR` button:

```text
PIC18F452 Reset cause test
MCLR: pin 1 -> 10k -> +5V
MCLR button: pin 1 -> button -> GND

Reset cause code=4 EXTERNAL_MCLR

Reset flags cleared.
Press MCLR button in Proteus to test external reset.
```

## Source files

The `reset.X` project needs:

```text
config_bits.c
main.c
../../../core/delay.c
../../../drivers/communication/uart/uart.c
../../../libraries/system/uart_debug/uart_debug.c
../../../drivers/system/reset/reset.c
```

## Troubleshooting

1. `MCLR` must have a `10k` pull-up to `+5V`.
2. The reset button must pull `MCLR` to `GND`.
3. UART `RC6/TX` must be connected to Virtual Terminal `RXD`.
4. The PIC clock in Proteus must match `PIC_PLATFORM_CLOCK_HZ`, for example `10 MHz`.
5. Call `reset_clear_flags()` only after `reset_get_cause()`.

## Watchdog note

This example tests Power-on reset and external reset through `MCLR`. For Watchdog reset, use a separate example such as `reset_wdt.X`, where WDT is enabled in config bits and the firmware intentionally does not call `CLRWDT`.
