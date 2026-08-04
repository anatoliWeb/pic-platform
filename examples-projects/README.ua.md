[English version](./README.md)

# examples-projects

## Що таке examples-projects

`examples-projects` містить окремі wrapper-проєкти MPLAB X для зовнішньої кодової бази `pic-platform`.

## Структура

```text
examples-projects/
|-- xc8/
|-- c18/
|-- hex/
`-- proteus/
```

XC8-проєкти згруповано у сімейства під `xc8/`; автономні smoke-test проєкти залишаються в корені `xc8/`.

## Правила

- Тримайте бібліотеку окремо від прикладного проєкту.
- Додавайте в `Source Files` лише потрібні `.c` файли.
- Не додавайте `.h` файли вручну в `Source Files`.
- Залишайте configuration bits у `config_bits.c`.
- Використовуйте лише відносні include paths і source paths.
- Не відстежуйте у git виходи MPLAB `build/`, `dist/` та `nbproject/private/` поруч із файлами прикладу.

## Підтримувані компілятори

- MPLAB XC8
- MPLAB C18

## Сімейства прикладів

| Сімейство | Приклади | Призначення |
|---|---|---|
| Базова валідація (root) | `blink.X`, `gpio.X`, `adc.X`, `adc_read.X`, `timer.X`, `pwm.X`, `ccp_capture.X`, `ccp_compare.X`, `clock.X`, `comparator.X`, `crc.X`, `eeprom.X`, `ext_interrupt.X`, `portb_change.X`, `reset.X`, `ring_buffer.X`, `template.X` | Базові інтеграційні перевірки та canonical smoke tests |
| Debug | `xc8/debug/default_uart.X`, `xc8/debug/display_i2c.X`, `xc8/debug/display_parallel.X`, `xc8/debug/pins_gpio.X`, `xc8/debug/multi_backend.X`, `xc8/debug/disabled.X` | Приклади debug-бекендів (UART / display / GPIO / disabled) |
| Communication | `xc8/communication/uart/{basic,debug}.X`, `xc8/communication/i2c/{basic,scan}.X`, `xc8/communication/spi/{basic,loopback}.X`, `xc8/communication/rs485/{basic,echo}.X`, `xc8/communication/onewire/{basic,bus_test}.X` | Приклади протоколів UART, I2C, SPI, RS485 та OneWire |
| Sensors | `xc8/sensors/tachometer.X`, `xc8/sensors/ds18b20/{basic,read,multi_read,search_rom}.X` | Tachometer simulation/demo плюс DS18B20 read, multi-drop і ROM search |
| Input | `xc8/input/button.X`, `xc8/input/encoder.X`, `xc8/input/adc_buttons.X`, `xc8/input/zero_cross.X`, `xc8/input/digital_input.X` | Input-бібліотеки, приклади debounce, generic digital input demo і автономний smoke test zero-cross |
| Actuator | `xc8/actuator/cooldown_output.X`, `xc8/actuator/position_drive_adc.X` | Cooldown output simulation/demo і замкнений позиційний привод з ADC-датчиком положення; encoder-бекенд ще є заглушкою |
| Output / control | `xc8/output/software_pwm.X`, `xc8/output/ac_phase_control.X` | Приклади software PWM і AC phase control |
| Timers | `xc8/timers/tick.X`, `xc8/timers/timer0.X`, `xc8/timers/timer1.X`, `xc8/timers/timer2.X`, `xc8/timers/timer3.X` | Приклади timebase і таймерів |
| System | `xc8/system/wdt.X` | Приклад watchdog |

## Короткий статус

| Приклад | Статус |
|---|---|
| `xc8/input/button.X` | Перевірено в Proteus |
| `xc8/input/encoder.X` | Перевірено в Proteus |
| `xc8/input/adc_buttons.X` | Перевірено в Proteus |
| `xc8/input/zero_cross.X` | Готово до перевірки в Proteus |
| `xc8/input/digital_input.X` | Готово до XC8 validation |
| `xc8/sensors/tachometer.X` | Готово до XC8 validation |
| `xc8/actuator/cooldown_output.X` | Готово до XC8 validation |
| `xc8/output/software_pwm.X` | Перевірено в Proteus |
| `xc8/system/wdt.X` | Перевірено в Proteus |
| `xc8/output/ac_phase_control.X` | Готово до перевірки в Proteus |
| `xc8/seven_segment/basic_manual.X` | Готово до перевірки в Proteus |
| `xc8/seven_segment/multiplex_manual.X` | Готово до перевірки в Proteus |
| `xc8/seven_segment/multiplex_timer.X` | Готово до перевірки в Proteus |
| `xc8/seven_segment/keys_single_line.X` | Готово до перевірки в Proteus |
| `xc8/seven_segment/keys_diode_coded.X` | Готово до перевірки в Proteus |
| `xc8/actuator/position_drive_adc.X` | Готово до перевірки в Proteus |
| `xc8/display/lcd_hd44780/i2c_pcf8574.X` | Готово до перевірки в Proteus |
| `xc8/sensors/ds18b20/search_rom.X` | Перевірено в Proteus; XC8 HEX export pending |
| `xc8/communication/onewire/bus_test.X` | Перевірено в Proteus; XC8 HEX export pending |
| `xc8/sensors/ds18b20/multi_read.X` | Потребує додаткової перевірки в Proteus і C18 |

## Примітки

- Нотатки для Proteus зберігаються в `examples-projects/proteus/`.
- Повторно використовувані HEX-файли, якщо вони згенеровані, лежать у `examples-projects/hex/`.
- Для кожного прикладу схему підключення й очікувану поведінку описано у README всередині папки прикладу.
- Інструкція для консолі: [docs/build/console-build.ua.md](../docs/build/console-build.ua.md).
