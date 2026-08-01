# Library Catalog

This is an AI index, not a full human reference. Use it to find the reusable component before writing project-local code.

## Key reusable libraries

### `position_drive`

- Path: `libraries/actuator/position_drive/`
- Purpose: non-blocking closed-loop position control for a DC gear motor with ADC potentiometer feedback.
- Use when: you need target-angle motion, stop-on-error behavior, timeouts, stuck detection, and callback-based hardware binding.
- Do not use when: you need raw motor GPIO only or a different actuator model without extending the reusable library.
- API summary: `position_drive_init`, `position_drive_process`, `position_drive_move_to_deg`, `position_drive_stop`, `position_drive_emergency_stop`, getters and error helpers.
- Dependencies: core types/config, millisecond tick callback, raw sensor callback, motor callback, optional PWM callback, optional debug callback.
- Configuration defines: `POSITION_DRIVE_SENSOR_TYPE`, `POSITION_DRIVE_ENABLE_PWM`, `POSITION_DRIVE_ENABLE_TIMEOUT`, `POSITION_DRIVE_ENABLE_STUCK_DETECTION`, `POSITION_DRIVE_ENABLE_DIRECTION_CHECK`, `POSITION_DRIVE_ENABLE_UART_DEBUG`.
- Supported compilers: XC8/C18 via wrappers.
- Examples: `examples-projects/xc8/actuator/position_drive_adc.X`, `examples-projects/proteus/actuator/position_drive_adc/README.md`.
- Human docs: `docs/libraries/actuator/position_drive.md`, `docs/libraries/actuator/position_drive.ua.md`.
- Known limitations: ADC backend implemented; encoder backend is a placeholder and returns `DRV_STATUS_UNSUPPORTED`; PWM is optional and compiled out by default.
- Extension points: callbacks for tick, sensor read, motor control, optional speed/PWM, debug sink.
- Status: example-tested; Proteus docs exist; tracked HEX exists.

### `seven_segment`

- Path: `libraries/display/seven_segment/`
- Purpose: buffered seven-segment display with manual or timer-owned refresh.
- Use when: you need single-digit or multiplexed display output, formatted numbers, brightness control, blink control, or timer-backed refresh ownership.
- Do not use when: you need a different display technology or you want to hide timer ownership inside project code.
- API summary: `seven_segment_init`, `seven_segment_process`, `seven_segment_refresh`, `seven_segment_irq_handler`, setters for number/fixed/time/brightness/blink.
- Dependencies: GPIO, timer backends, core headers.
- Configuration defines: `SEVEN_SEGMENT_REFRESH_MANUAL`, `SEVEN_SEGMENT_REFRESH_TIMER`, `SEVEN_SEGMENT_TIMER0..3`, `SEVEN_SEGMENT_ENABLE_TIMER0..3`.
- Supported compilers: XC8/C18 via wrappers.
- Examples: `examples-projects/xc8/seven_segment/basic_manual.X`, `multiplex_manual.X`, `multiplex_timer.X`, `keys_single_line.X`, `keys_diode_coded.X`.
- Human docs: `docs/libraries/display/seven_segment.md`, `docs/libraries/display/seven_segment.ua.md`.
- Known limitations: one timer backend can own one display instance at a time.
- Extension points: manual refresh path, timer-owned path, formatting helpers, diagnostics.
- Status: example-tested; timer-backed mode documented.

### `button`

- Path: `libraries/input/button/`
- Purpose: non-blocking active-low button events with debounce, click, double-click, hold, and repeat support.
- Use when: you need direct GPIO buttons or an external raw-state source that still wants the standard button event logic.
- Do not use when: you only need a raw pin read without debounce or events.
- API summary: `button_init`, `button_init_external`, `button_set_raw_state`, `button_update`, event/query helpers.
- Dependencies: `drivers/timers/tick`, bit utilities, core types.
- Configuration defines: none beyond the shared platform debug/config rules.
- Supported compilers: XC8/C18 via wrappers.
- Examples: `examples-projects/xc8/button.X`, `examples-projects/xc8/seven_segment/keys_single_line.X`, `examples-projects/xc8/seven_segment/keys_diode_coded.X`.
- Human docs: `docs/libraries/input/button.md`, `docs/libraries/input/button.ua.md`.
- Known limitations: active-low logic is used; timing is driven by `tick_get()`.
- Extension points: external-state mode for shared-line decoders.
- Status: example-tested.

### `segment_keys`

- Path: `libraries/input/segment_keys/`
- Purpose: exact-mask key decoder for buttons connected to a seven-segment segment bus.
- Use when: you need shared-line keys, diode-coded keys, or exact-mask decoding on a display bus.
- Do not use when: you need debounce or hold logic without the `button` library.
- API summary: `segment_keys_init`, `segment_keys_update`, key/state accessors.
- Dependencies: `seven_segment`, `button`, `gpio`.
- Configuration defines: key table and active level live in the config struct.
- Supported compilers: XC8/C18 via wrappers.
- Examples: `examples-projects/xc8/seven_segment/keys_single_line.X`, `examples-projects/xc8/seven_segment/keys_diode_coded.X`.
- Human docs: `docs/libraries/input/segment_keys.md`, `docs/libraries/input/segment_keys.ua.md`.
- Known limitations: exact-mask decoding only; no built-in debounce.
- Extension points: button forwarding, alternative key maps.
- Status: example-tested; manual Proteus validation still noted in docs.

### `uart_debug`

- Path: `libraries/system/uart_debug/`
- Purpose: macro-based UART debug layer with compile-time enable/disable.
- Use when: you need low-overhead debug prints that can disappear at compile time.
- Do not use when: you need a full logging framework or non-UART transport without an adapter.
- API summary: `DBG_PRINT`, `DBG_PRINTLN`, `DBG_WRITE_BYTE`, `DBG_PRINT_INT`, `DBG_PRINT_HEX`, and helper functions.
- Dependencies: `core/config.h`, `core/compiler.h`, UART driver.
- Configuration defines: `DRV_DEBUG_ENABLE`, `DRV_USE_UART`.
- Supported compilers: XC8/C18 via wrappers.
- Examples: `examples-projects/xc8/seven_segment/keys_single_line.X`, `examples-projects/xc8/seven_segment/keys_diode_coded.X`, `examples-projects/xc8/actuator/position_drive_adc.X`.
- Human docs: `docs/libraries/system/uart_debug.md`, `docs/libraries/system/uart_debug.ua.md`.
- Known limitations: when disabled, macros expand away; transport is UART-centric.
- Extension points: adapter callback at application level.
- Status: example-tested.

## Timing helpers used by libraries

`tick` is a driver, not a library, but it is part of the selection flow because `button` and `position_drive` depend on it.

### `tick`

- Path: `drivers/timers/tick/`
- Purpose: millisecond time base built on `timer1`.
- API summary: `tick_init`, `tick_get`, `tick_delay`.
- Dependencies: `timer1`, `core/device.h`.
- Use when: a library needs a non-blocking millisecond clock.
- Do not use when: the project already owns Timer1 for something else.
- Status: example-tested.

## Additional library folders present in the tree

The following folders are real and should be searched before adding new reusable code:

| Category | Present folders |
|---|---|
| `libraries/display/` | `bitmap`, `display_framework`, `graphics`, `ili9341`, `lcd_hd44780`, `seven_segment_mux`, `sh1106`, `ssd1306`, `st7735` |
| `libraries/input/` | `adc_buttons`, `encoder`, `ir_receiver`, `keypad_matrix`, `shared_segment_buttons` |
| `libraries/led/` | `apa102`, `led_animation_engine`, `led_effects`, `led_segments`, `rgb_utils`, `ws2812`, `ws2812_matrix` |
| `libraries/output/` | `ac_phase_control`, `software_pwm` |
| `libraries/sensors/` | `bmp`, `dht`, `ds18b20`, `ultrasonic` |
| `libraries/system/` | `buzzer`, `relay`, `servo` |
| `libraries/time/` | `cron_scheduler`, `ds12c887`, `ds1302`, `ds1307`, `ds3231`, `ntp_sync`, `pcf8563`, `rtc_framework`, `software_rtc`, `timezone` |
| `libraries/ui/` | `icon_pack`, `input_adapter`, `menu_core`, `menu_navigation`, `menu_renderer`, `menu_widgets`, `overlay_system`, `popup_manager`, `screen_manager`, `screen_transitions`, `sprite_buffer`, `ui_events`, `ui_themes` |

## Search rule

If the requested behavior is close to one of these folders, search that folder before creating project-local code.
