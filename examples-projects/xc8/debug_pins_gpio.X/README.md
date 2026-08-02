# debug_pins_gpio XC8 example

## Purpose

Demonstrate the `PINS` backend of the debug facade (`core/debug.h`) using
direct GPIO output. Text output is not used; diagnostic signals are driven on
PORTC bits 0..3.

## Selected defines

In `project_config.h`:

| Define | Value |
| --- | ---: |
| `DRV_DEBUG_BACKEND_UART` | `0` |
| `DRV_DEBUG_BACKEND_DISPLAY` | `0` |
| `DRV_DEBUG_BACKEND_PINS` | `1` |
| `DRV_DEBUG_PINS_INTERFACE_GPIO` | `1` |
| `DRV_DEBUG_PINS_INTERFACE_I2C` | `0` |
| `DRV_DEBUG_PINS_PORT` | `PORTC` |
| `DRV_DEBUG_PINS_TRIS` | `TRISC` |
| `DRV_DEBUG_PINS_START_BIT` | `0` |
| `DRV_DEBUG_PINS_CHANNEL_COUNT` | `4` |
| `DRV_DEBUG_PINS_PULSE_US` | `10000` |

## Channel mapping

Debug channels are contiguous bits on PORTC starting at bit 0.

| Debug channel | PIC port bit | PIC18F452 pin | Expected signal |
| --- | --- | ---: | --- |
| 0 | RC0 | 15 | 10 ms pulse; error-code blinks |
| 1 | RC1 | 16 | set high / low toggle (500 ms) |
| 2 | RC2 | 17 | held per `debug_pin_code()` |
| 3 | RC3 | 18 | held per `debug_pin_code()` |

Signals are active-high: a high level is the "on" state.

## LED connection

Attach one LED per observed channel through a current-limiting resistor of
330-1000 ohm. The PIC drives the LED anode via the resistor to ground.

```
PORTC bit  --[330..1000 ohm]--|>|-- GND
```

Never connect an LED without a current-limiting resistor.

## Observing signals

- LED: connect as above; a lit LED means the channel is high.
- Logic analyzer / oscilloscope: probe PORTC bit for the expected high level.
- Pulse duration is `DRV_DEBUG_PINS_PULSE_US` (default 10000 us = 10 ms).
- Error-code pattern (`debug_pin_error(code)`): channel 0 pulses
  `(code & 0x0F) + 1` times, 50 ms apart, then a 200 ms gap.

## Source isolation

This project compiles only the GPIO pins backend. It does not compile
`uart.c`, the UART backend, `i2c.c`, `lcd.c`, or the display backend. The exact
source list is in `nbproject/configurations.xml`.

## Build

```cmd
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .clean-conf
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .build-conf
```

## Expected result

- Channel 0 pulses every 500 ms.
- Channel 1 goes high then low, every 500 ms.
- `debug_pin_code(0x05)` holds channels 0 and 2 high for one second.
- `debug_pin_error(2)` blinks channel 0 three times, then the loop repeats.
