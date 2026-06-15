# Gyver-style Library Porting Roadmap

Цей документ тримає окремий план перенесення Gyver-style бібліотек у `pic-platform`, щоб не роздувати головний `TODO.md`.

Правило: якщо аналог уже існує в репозиторії, спочатку розширюємо або уніфікуємо його, а не дублюємо нову логіку.

## Пріоритет 1: input / navigation / control

| Статус | Що портувати | Куди в структурі | Мінімальний `.X` приклад | XC8/C18 | Обмеження PIC18F452 |
|---|---|---|---|---|---|
| [ ] | Кнопки: debounce, click, hold, multi-click, long-press, аналог EncButton | `libraries/input/button/`, `libraries/ui/input_adapter/` | `button_click_hold.X` | XC8: так, C18: так | Дуже малий RAM, без блокувань |
| [ ] | Rotary encoder, напрямок, кроки, прискорення | `libraries/input/encoder/` | `encoder_menu.X` | XC8: так, C18: так | Частий polling, мінімум стеку |
| [ ] | ADC buttons на одній лінії через resistor ladder | `libraries/input/adc_buttons/` | `adc_buttons_menu.X` | XC8: так, C18: так | Чутливість до шуму ADC |
| [ ] | Matrix keypad 2x2 / 3x4 / 4x4 | `libraries/input/keypad_matrix/` | `keypad_menu.X` | XC8: так, C18: так | Потрібен компактний scan model |
| [ ] | Menu/input helpers для навігації по UI | `libraries/ui/menu_core/`, `libraries/ui/menu_navigation/`, `libraries/ui/input_adapter/` | `menu_input_demo.X` | XC8: так, C18: так | Не тягнути display logic в input layer |

## Пріоритет 2: timing / helpers

| Статус | Що портувати | Куди в структурі | Мінімальний `.X` приклад | XC8/C18 | Обмеження PIC18F452 |
|---|---|---|---|---|---|
| [ ] | Relay control, pulse, delayed off, non-blocking timers | `libraries/system/relay/`, `core/scheduler/`, `core/rtos/` | `relay_pulse.X` | XC8: так, C18: так | Лише cooperative model, без delay-based logic |
| [ ] | Buzzer / beeper / pattern playback | `libraries/system/buzzer/` | `buzzer_pattern.X` | XC8: так, C18: так | Патерни мають бути короткі й non-blocking |
| [ ] | `millis()`-like helpers і спільний tick flow | `core/rtos/`, `core/scheduler/` | `millis_demo.X` | XC8: так, C18: так | Не вводити preemptive RTOS для PIC18F452 |
| [ ] | Software PWM helpers | `libraries/system/soft_pwm/` або `core/` | `soft_pwm_demo.X` | XC8: так, C18: так | Жорсткий контроль часу виконання |

## Пріоритет 3: filters / control

| Статус | Що портувати | Куди в структурі | Мінімальний `.X` приклад | XC8/C18 | Обмеження PIC18F452 |
|---|---|---|---|---|---|
| [ ] | Фільтри: average, running average, median, exponential | `core/filters/` або `libraries/system/filters/` | `filters_demo.X` | XC8: так, C18: так | Тільки integer math, без floating point |
| [ ] | PID / regulator helpers | `libraries/system/pid/` | `pid_regulator.X` | XC8: так, C18: так | Пам'ять і stack дуже обмежені |

## Пріоритет 4: sensors / IO expansion

| Статус | Що портувати | Куди в структурі | Мінімальний `.X` приклад | XC8/C18 | Обмеження PIC18F452 |
|---|---|---|---|---|---|
| [ ] | NTC thermistor helpers | `libraries/sensors/ntc/` | `ntc_read.X` | XC8: так, C18: так | Лінійка/таблиці мають бути компактні |
| [ ] | Joystick reading і обробка осей/кнопки | `libraries/input/joystick/` | `joystick_xy.X` | XC8: так, C18: так | ADC + debounce, без зайвих буферів |
| [ ] | 74HC595 / 74HC165 shift register helpers | `drivers/communication/shift_register/` | `shift_register_595_165.X` | XC8: так, C18: так | Простий bit-bang або SPI-backed path |

## Пріоритет 5: display / UI helpers

| Статус | Що портувати | Куди в структурі | Мінімальний `.X` приклад | XC8/C18 | Обмеження PIC18F452 |
|---|---|---|---|---|---|
| [ ] | Display helpers, коли RAM дозволяє | `libraries/display/graphics/`, `libraries/display/display_framework/`, `libraries/ui/icon_pack/`, `libraries/ui/ui_themes/` | `display_helper_demo.X` | XC8: так, C18: так | Тільки якщо RAM/Flash ще достатньо |

## Рекомендований порядок

1. Кнопки / debouncing / click / hold.
2. Encoder.
3. ADC buttons.
4. Keypad.
5. Relay + millis-like helpers.
6. Buzzer.
7. Filters.
8. PID helpers.
9. NTC та joystick.
10. Shift registers.
11. Display helpers, коли по RAM це безпечно.

## Зауваження для PIC18F452

- RAM дуже обмежена, тому кожен новий helper має бути statically allocated або без буферів.
- Для timing-sensitive логіки потрібні короткі update/callback цикли без `delay_ms()`.
- Під час портування кожну бібліотеку бажано перевіряти мінімальним XC8 `.X` прикладом.
- C18-перевірка потрібна там, де код не використовує C99/складні макроси і де це практично виправдано.
