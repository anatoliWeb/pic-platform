[🇬🇧 English version](./button.md)

# Button Драйвер

## Опис

Неблокуючий драйвер кнопки з debounce та подієвими прапорцями.
Використовується для UI-подій, зокрема click і hold.

## API

- `button_init()`
- `button_update()`
- `button_is_clicked()`
- `button_is_held()`

## Приклад

```c
button_t btn;
button_init(&btn, &PORTB, &TRISB, 0u);
button_update(&btn);

if (button_is_clicked(&btn)) { /* дія click */ }
if (button_is_held(&btn))    { /* дія hold */ }
```

## Особливості

- Debounce реалізовано всередині драйвера через часову перевірку стану.
- Розраховано на періодичний виклик зі scheduler/polling loop.

## Залежності

- `core/compiler.h`
- `core/types.h`

