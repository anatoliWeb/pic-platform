[English version](./button.md)

# Драйвер button

## Опис

Неблокуючий active-low драйвер кнопки з debounce, click, double-click, hold і hold-repeat.

## API

- `button_init()`
- `button_init_external()`
- `button_set_raw_state()`
- `button_update()`
- `button_pressed()`
- `button_released()`
- `button_clicked()`
- `button_double_clicked()`
- `button_held()`
- `button_hold_repeated()`
- `button_get_click_count()`
- `button_is_clicked()`
- `button_is_held()`

## Приклад

```c
button_t btn;
button_init(&btn, &PORTB, &TRISB, 0u);

while (1)
{
    button_update(&btn);

    if (button_clicked(&btn))
    {
        /* handle click */
    }
}
```

## Режим зовнішнього стану

Використовуйте `button_init_external()`, коли інша бібліотека вже декодує електричний стан і потрібні лише події кнопки.

```c
button_init_external(&btn, 1u);
button_set_raw_state(&btn, 0u);
button_update(&btn);
```

## Примітки

- Використовується active-low логіка: `0 = pressed`, `1 = released`.
- Таймінги базуються на `tick_get()`.
- Режим зовнішнього стану використовується helpers для shared-line input, зокрема `segment_keys`.
- `button` лишається єдиним місцем для debounce і event-логіки навіть тоді, коли електричним скануванням керує інший helper.
- Це дозволяє перевикористовувати ту саму click/hold-поведінку і для прямих GPIO-кнопок, і для shared-line декодерів.

## Пов'язані приклади

- `examples-projects/xc8/input/button.X`
- `examples-projects/xc8/seven_segment/keys_single_line.X`
- `examples-projects/xc8/seven_segment/keys_diode_coded.X`

## Залежності

- `drivers/timers/tick`
