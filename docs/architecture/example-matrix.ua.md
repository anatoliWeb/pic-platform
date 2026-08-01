# Матриця прикладів

| Example project | Purpose | Required library files | Required driver files | Timer usage | Pins | Output HEX | Proteus path | Status |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `examples-projects/xc8/seven_segment/basic_manual.X` | Демо одного digit з manual refresh і brightness | `libraries/display/seven_segment/seven_segment.c` | `drivers/gpio/gpio.c`, `core/delay.c` | Немає | `RD0..RD7` segments, `RC0` digit enable, `RE0/RE1` diagnostics | `examples-projects/hex/xc8/seven_segment/basic_manual.X.production.hex` | У репозиторії немає | example-tested |
| `examples-projects/xc8/seven_segment/multiplex_manual.X` | Чотири digit manual multiplex | `libraries/display/seven_segment/seven_segment.c` | `drivers/gpio/gpio.c`, `core/delay.c` | Немає | `RD0..RD7` segments, `RC0..RC3` digits, `RE0/RE1` diagnostics | `examples-projects/hex/xc8/seven_segment/multiplex_manual.X.production.hex` | У репозиторії немає | example-tested |
| `examples-projects/xc8/seven_segment/multiplex_timer.X` | Чотири digit з timer-backed refresh | `libraries/display/seven_segment/seven_segment.c` | `drivers/gpio/gpio.c`, `drivers/timers/timer2/timer2.c`, `core/delay.c` | Timer2 керує refresh | `RD0..RD7` segments, `RC0..RC3` digits | `examples-projects/hex/xc8/seven_segment/multiplex_timer.X.production.hex` | У репозиторії немає | example-tested |
| `examples-projects/xc8/seven_segment/keys_single_line.X` | Shared-line input з однією кнопкою на лінію сегмента | `libraries/display/seven_segment/seven_segment.c`, `libraries/input/button/button.c`, `libraries/input/segment_keys/segment_keys.c`, `libraries/system/uart_debug/uart_debug.c` | `drivers/gpio/gpio.c`, `drivers/communication/uart/uart.c`, `drivers/timers/tick/tick.c`, `drivers/timers/timer1/timer1.c`, `core/delay.c` | Timer1 належить `tick` | `RD0..RD2` shared keys, `RD0..RD7` segments, `RC0..RC3` digits, `RC6/RC7` UART | `examples-projects/hex/xc8/seven_segment/keys_single_line.X.production.hex` | У репозиторії немає | example-tested |
| `examples-projects/xc8/seven_segment/keys_diode_coded.X` | Shared-line input з diode-coded кнопками | `libraries/display/seven_segment/seven_segment.c`, `libraries/input/button/button.c`, `libraries/input/segment_keys/segment_keys.c`, `libraries/system/uart_debug/uart_debug.c` | `drivers/gpio/gpio.c`, `drivers/communication/uart/uart.c`, `drivers/timers/tick/tick.c`, `drivers/timers/timer1/timer1.c`, `core/delay.c` | Timer1 належить `tick` | `RD0..RD2` shared keys, `RD0..RD7` segments, `RC0..RC3` digits, `RC6/RC7` UART | `examples-projects/hex/xc8/seven_segment/keys_diode_coded.X.production.hex` | У репозиторії немає | example-tested |

## Рецепти збірки / використання

### Manual seven-segment display

- Додайте `main.c`, `config_bits.c`, `project_config.h`.
- Додайте `core/delay.c`, `drivers/gpio/gpio.c`, `libraries/display/seven_segment/seven_segment.c`.
- Timer-драйвери не потрібні.

### Timer-backed seven-segment display

- Додайте `drivers/timers/timer2/timer2.c`.
- Встановіть `SEVEN_SEGMENT_ENABLE_TIMER2=1`.
- Forward-ьте interrupts у `seven_segment_irq_handler()`.

### Shared-line keys

- Додайте `libraries/input/button/button.c`.
- Додайте `libraries/input/segment_keys/segment_keys.c`.
- Підтримуйте refresh дисплея під час scan.

### UART debug

- Додайте `drivers/communication/uart/uart.c`.
- Додайте `libraries/system/uart_debug/uart_debug.c`.
- Використовуйте `RC6/TX` і `RC7/RX`, якщо схема не вимагає іншого.
