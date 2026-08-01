# Example Matrix

| Example project | Purpose | Required library files | Required driver files | Timer usage | Pins | Output HEX | Proteus path | Status |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `examples-projects/xc8/seven_segment/basic_manual.X` | Single-digit manual refresh and brightness demo | `libraries/display/seven_segment/seven_segment.c` | `drivers/gpio/gpio.c`, `core/delay.c` | None | `RD0..RD7` segments, `RC0` digit enable, `RE0/RE1` diagnostics | `examples-projects/hex/xc8/seven_segment/basic_manual.X.production.hex` | Not present in repo | example-tested |
| `examples-projects/xc8/seven_segment/multiplex_manual.X` | Four-digit manual multiplex demo | `libraries/display/seven_segment/seven_segment.c` | `drivers/gpio/gpio.c`, `core/delay.c` | None | `RD0..RD7` segments, `RC0..RC3` digits, `RE0/RE1` diagnostics | `examples-projects/hex/xc8/seven_segment/multiplex_manual.X.production.hex` | Not present in repo | example-tested |
| `examples-projects/xc8/seven_segment/multiplex_timer.X` | Four-digit timer-backed multiplex demo | `libraries/display/seven_segment/seven_segment.c` | `drivers/gpio/gpio.c`, `drivers/timers/timer2/timer2.c`, `core/delay.c` | Timer2 owns refresh | `RD0..RD7` segments, `RC0..RC3` digits | `examples-projects/hex/xc8/seven_segment/multiplex_timer.X.production.hex` | Not present in repo | example-tested |
| `examples-projects/xc8/seven_segment/keys_single_line.X` | Shared-line input with one button per segment line | `libraries/display/seven_segment/seven_segment.c`, `libraries/input/button/button.c`, `libraries/input/segment_keys/segment_keys.c`, `libraries/system/uart_debug/uart_debug.c` | `drivers/gpio/gpio.c`, `drivers/communication/uart/uart.c`, `drivers/timers/tick/tick.c`, `drivers/timers/timer1/timer1.c`, `core/delay.c` | Timer1 owned by `tick` | `RD0..RD2` shared keys, `RD0..RD7` segments, `RC0..RC3` digits, `RC6/RC7` UART | `examples-projects/hex/xc8/seven_segment/keys_single_line.X.production.hex` | Not present in repo | example-tested |
| `examples-projects/xc8/seven_segment/keys_diode_coded.X` | Shared-line input with diode-coded keys | `libraries/display/seven_segment/seven_segment.c`, `libraries/input/button/button.c`, `libraries/input/segment_keys/segment_keys.c`, `libraries/system/uart_debug/uart_debug.c` | `drivers/gpio/gpio.c`, `drivers/communication/uart/uart.c`, `drivers/timers/tick/tick.c`, `drivers/timers/timer1/timer1.c`, `core/delay.c` | Timer1 owned by `tick` | `RD0..RD2` shared keys, `RD0..RD7` segments, `RC0..RC3` digits, `RC6/RC7` UART | `examples-projects/hex/xc8/seven_segment/keys_diode_coded.X.production.hex` | Not present in repo | example-tested |
| `examples-projects/xc8/actuator/position_drive_adc.X` | Closed-loop position drive with an ADC potentiometer sensor | `libraries/actuator/position_drive/position_drive.c`, `libraries/system/uart_debug/uart_debug.c` | `drivers/analog/adc/adc.c`, `drivers/gpio/gpio.c`, `drivers/timers/tick/tick.c`, `drivers/timers/timer1/timer1.c`, `core/delay.c` | Timer1 owned by `tick` | `RA0/AN0` potentiometer, `RD0/RD1` H-bridge, `RC6` UART TX | `examples-projects/hex/xc8/actuator/position_drive_adc.X.production.hex` | `examples-projects/proteus/actuator/position_drive_adc/README.md` | example-tested |
| `examples-projects/c18/actuator/position_drive_adc/` | C18 position drive example (planned) | `libraries/actuator/position_drive/position_drive.c` via C18 include stub | `drivers/analog/adc/adc.c`, `drivers/gpio/gpio.c`, `drivers/timers/tick/tick.c`, `drivers/timers/timer1/timer1.c` | Timer1 owned by `tick` | `RA0/AN0` potentiometer, `RD0/RD1` H-bridge, `RC6` UART TX | none yet | not generated | planned |

## Build / Use Recipes

### Manual seven-segment display

- Add `main.c`, `config_bits.c`, `project_config.h`.
- Add `core/delay.c`, `drivers/gpio/gpio.c`, `libraries/display/seven_segment/seven_segment.c`.
- Do not add timer drivers.

### Timer-backed seven-segment display

- Add `drivers/timers/timer2/timer2.c`.
- Define `SEVEN_SEGMENT_ENABLE_TIMER2=1`.
- Forward interrupts to `seven_segment_irq_handler()`.

### Shared-line keys

- Add `libraries/input/button/button.c`.
- Add `libraries/input/segment_keys/segment_keys.c`.
- Keep `seven_segment` refresh active while scanning.

### UART debug

- Add `drivers/communication/uart/uart.c`.
- Add `libraries/system/uart_debug/uart_debug.c`.
- Use `RC6/TX` and `RC7/RX` unless another project constraint says otherwise.

### Position drive (ADC backend)

- Add `libraries/actuator/position_drive/position_drive.c`.
- Add `drivers/analog/adc/adc.c`, `drivers/gpio/gpio.c`, `drivers/timers/tick/tick.c`, `drivers/timers/timer1/timer1.c`.
- Wire the `read_raw` callback to `adc_read`, the `get_tick` callback to `tick_get`, and the `motor` callback to the H-bridge pins.
- Call `position_drive_process()` regularly from the main loop.
