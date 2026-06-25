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

## Правила

- Тримайте бібліотеку окремо від прикладного проєкту.
- Додавайте в `Source Files` лише потрібні `.c` файли.
- Не додавайте `.h` файли вручну в `Source Files`.
- Залишайте configuration bits у `config_bits.c`.
- Використовуйте лише відносні include paths і source paths.

## Підтримувані компілятори

- MPLAB XC8
- MPLAB C18

## Сімейства прикладів

| Сімейство | Приклади | Призначення |
|---|---|---|
| Базова валідація | `blink.X`, `uart_debug.X`, `adc_read.X`, `i2c_scan.X`, `spi_loopback.X`, `rs485_echo.X`, `ds18b20_read.X` | Базові інтеграційні перевірки |
| Input | `button.X`, `encoder.X`, `adc_buttons.X` | Input-бібліотеки та приклади debounce |
| Display | `lcd_hd44780.X`, `xc8/seven_segment/basic_manual.X`, `xc8/seven_segment/multiplex_manual.X`, `xc8/seven_segment/multiplex_timer.X`, `xc8/seven_segment/keys_single_line.X`, `xc8/seven_segment/keys_diode_coded.X` | Приклади оновлення індикаторів, timer-owned multiplexing, fixed-point formatting і shared-line input |
| Timing та system | `tick.X`, `timer0.X`, `timer1.X`, `timer2.X`, `timer3.X`, `wdt.X`, `software_pwm.X`, `ac_phase_control.X` | Таймери, watchdog і platform extensions |
| 1-Wire та sensors | `ds18b20.X`, `ds18b20_search_rom.X`, `onewire_bus_test.X`, `ds18b20_multi_read.X` | DS18B20 і діагностика шини |

## Короткий статус

| Приклад | Статус |
|---|---|
| `button.X` | Перевірено в Proteus |
| `encoder.X` | Перевірено в Proteus |
| `adc_buttons.X` | Перевірено в Proteus |
| `software_pwm.X` | Перевірено в Proteus |
| `wdt.X` | Перевірено в Proteus |
| `ac_phase_control.X` | Готово до перевірки в Proteus |
| `xc8/seven_segment/basic_manual.X` | Готово до перевірки в Proteus |
| `xc8/seven_segment/multiplex_manual.X` | Готово до перевірки в Proteus |
| `xc8/seven_segment/multiplex_timer.X` | Готово до перевірки в Proteus |
| `xc8/seven_segment/keys_single_line.X` | Готово до перевірки в Proteus |
| `xc8/seven_segment/keys_diode_coded.X` | Готово до перевірки в Proteus |
| `ds18b20_search_rom.X` | Перевірено в Proteus; XC8 HEX export pending |
| `onewire_bus_test.X` | Перевірено в Proteus; XC8 HEX export pending |
| `ds18b20_multi_read.X` | Потребує додаткової перевірки в Proteus і C18 |

## Примітки

- Нотатки для Proteus зберігаються в `examples-projects/proteus/`.
- Повторно використовувані HEX-файли, якщо вони згенеровані, лежать у `examples-projects/hex/`.
- Для кожного прикладу схему підключення й очікувану поведінку описано у README всередині папки прикладу.

