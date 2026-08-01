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

| Module | Shared source | XC8 route | C18 route | Wrapper type | Forbidden combination |
|---|---|---|---|---|---|
| `comparator`, `i2c`, `rs485`, `spi`, `ext_interrupt`, `portb_change`, `eeprom`, `i2c_eeprom`, `timer0`, `timer1`, `timer2`, `timer3`, `ccp_compare`, `ccp_capture`, `pwm`, `position_drive`, `button`, `segment_keys`, `uart_debug`, `onewire`, `ds18b20`, `seven_segment`, `seven_segment_mux` | shared `.c` + compiler wrapper | `XC8/.../*.c` | `C18/.../*.c` | include-wrapper or mixed | do not compile shared and wrapper `.c` together when wrapper includes shared `.c` |
| `clock`, `reset`, `wdt`, `graphics`, `bitmap`, `ili9341`, `sh1106`, `ssd1306`, `st7735`, `shared_segment_buttons`, `keypad_matrix`, `ir_receiver`, `ws2812`, `ws2812_matrix`, `rgb_utils`, `led_segments`, `led_effects`, `led_animation_engine`, `apa102`, `bmp`, `dht`, `ultrasonic`, `buzzer`, `relay`, `servo`, `rtc_framework`, `software_rtc`, `timezone`, `cron_scheduler`, `ntp_sync`, `ds1302`, `ds1307`, `ds3231`, `ds12c887`, `pcf8563`, `sprite_buffer`, `icon_pack`, `menu_core`, `menu_navigation`, `menu_renderer`, `menu_widgets`, `input_adapter`, `screen_manager`, `screen_transitions`, `overlay_system`, `popup_manager`, `ui_events`, `ui_themes`, `display_framework` | shared `.c` only or no wrapper split | shared `.c` in `libraries/...` | same shared `.c` | independent | no wrapper/shared duplication because there is no wrapper split |

## Known limitations

- Keep compiler-specific syntax in XC8 paths, not in shared code.

## Do

- Use shared headers from `core/`, `drivers/`, and `libraries/`.

## Don't

- Claim non-existent compiler support.
