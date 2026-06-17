[🇬🇧 English version](./button.md)

# Button Driver

## Опис

Фаза 1: неблокуючий драйвер кнопки з debounce. Він призначений для окремих input-прикладів. Шари menu navigation для Phase 2 будуть побудовані поверх цієї бібліотеки, але не входять до її складу.

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
- Hold repeat і click count реалізовані компактно, без динамічної пам’яті.
- `button_is_clicked()` та `button_is_held()` залишаються для старого коду.
- Шари Phase 2 menu/navigation не входять до цієї бібліотеки.

## Залежності

- `core/compiler.h`
- `core/types.h`
- `drivers/timers/tick`