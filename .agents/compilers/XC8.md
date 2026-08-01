# XC8

## Purpose

XC8 compiler compatibility map.

## Location

```text
XC8/
examples-projects/xc8/
```

## Wrapper style

- Use `XC8/drivers/...` and `XC8/libraries/...` for XC8-specific implementation details.

## Route matrix

| Module | Public header | Shared source | XC8 source | C18 source | XC8 pattern | C18 pattern | Correct route | Forbidden combination |
|---|---|---|---|---|---|---|---|---|
| `comparator`, `i2c`, `rs485`, `spi`, `uart`, `ext_interrupt`, `portb_change`, `eeprom`, `i2c_eeprom`, `timer0`, `timer1`, `timer2`, `timer3`, `ccp_compare`, `ccp_capture`, `pwm`, `onewire`, `ds18b20`, `uart_debug`, `seven_segment_mux`, `lcd_hd44780` | matching module header | shared dispatcher `.c` | compiler-specific `.c` | compiler-specific `.c` | independent | independent | compile the shared dispatcher or the compiler-specific source, not both | shared dispatcher and compiler-specific source together |
| `position_drive`, `segment_keys` | matching module header | shared implementation `.c` | compiler wrapper `.c` | compiler wrapper `.c` | include-wrapper | include-wrapper | compile the shared implementation or the wrapper, not both | shared implementation and wrapper together |
| `button`, `seven_segment` | matching module header | shared implementation `.c` | compiler-specific `.c` | compiler-specific `.c` | independent | independent | compile one implementation route only | shared implementation and compiler-specific source together |

## Known limitations

- Keep compiler-specific syntax in XC8 paths, not in shared code.

## Do

- Use shared headers from `core/`, `drivers/`, and `libraries/`.

## Don't

- Claim non-existent compiler support.
