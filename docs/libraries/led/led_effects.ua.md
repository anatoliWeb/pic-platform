[English version](./led_effects.md)

# Бібліотека LED Effects

## Опис
Перевикористовуваний неблокуючий рушій ефектів для адресних LED.
Побудований поверх `ws2812` і `rgb_utils`.

## Публічний API
- `led_effects_init()`
- `led_effects_set_mode()`
- `led_effects_update()`
- `led_effects_set_speed()`
- `led_effects_set_color()`
- `led_effects_is_finished()`

## Реалізовані ефекти
- blink
- breathing
- rainbow cycle
- color wipe
- scanner
- pulse

## Неблокуюча модель анімації
- Усередині рушія немає викликів `delay()`.
- Кожен `led_effects_update()` робить один невеликий крок стану.
- Швидкість керується внутрішнім дільником оновлень (`speed`).
- Працює з main loop, scheduler або RTOS task.

## Використання в циклі
Типовий цикл:
1. часто викликати `led_effects_update()`
2. обробляти події/ввід
3. через API змінювати mode/color/speed

## Продуктивність
- `ws2812_show()` є таймінг-критичною і блокуючою операцією на час виводу кадру.
- Підбирайте кількість LED і частоту оновлення під ресурс MCU.
- Зменшення яскравості знижує струм і навантаження.

## RAM
- У шарі ефектів динамічна пам'ять не використовується.
- Стан ефектів зберігається у компактній структурі `led_effects_t`.
- Буфер пікселів зберігається у шарі `ws2812`.

## Залежності
- `core/*`
- `libraries/led/ws2812/*`
- `libraries/led/rgb_utils/*`