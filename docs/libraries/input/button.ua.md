[🇬🇧 English version](./button.md)

# Button Driver

## Опис

Неблокуючий драйвер кнопки з debounce та Gyver-style подіями.
Підходить для UI-дій: press, release, click, double click, hold і hold repeat.

## API

- `button_init()`
- `button_update()`
- `button_pressed()`
- `button_released()`
- `button_clicked()`
- `button_double_clicked()`
- `button_held()`
- `button_hold_repeated()`
- `button_get_click_count()`
- `button_is_clicked()` - wrapper для сумісності
- `button_is_held()` - wrapper для сумісності

## Приклад

```c
button_t btn;
button_init(&btn, &PORTB, &TRISB, 0u);

while (1)
{
    button_update(&btn);

    if (button_pressed(&btn)) { /* press action */ }
    if (button_released(&btn)) { /* release action */ }
    if (button_clicked(&btn)) { /* click action */ }
    if (button_double_clicked(&btn)) { /* double click action */ }
    if (button_held(&btn)) { /* hold action */ }
    if (button_hold_repeated(&btn)) { /* repeat action */ }
}
```

## Особливості

- Очікується active-low кнопка.
- Debounce працює через polling і `tick_get()`.
- Hold repeat і click count зроблені компактно, без динамічної пам'яті.
- `button_is_clicked()` та `button_is_held()` лишаються для старого коду.

## Залежності

- `core/compiler.h`
- `core/types.h`
- `drivers/timers/tick`
