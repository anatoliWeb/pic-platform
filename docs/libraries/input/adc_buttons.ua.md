[🇬🇧 English version](./adc_buttons.md)

# ADC Buttons Driver

## Опис

Драйвер кнопок через resistor ladder на одному ADC вході. Він перетворює діапазони ADC на логічні ID кнопок і додає легкий debounce та event flags для input-прикладів фази 1.

## API

- `adc_buttons_init()`
- `adc_buttons_update()`
- `adc_buttons_get()`
- `adc_buttons_is_clicked()`
- `adc_buttons_is_held()`

## Приклад

```c
adc_buttons_t btns;
adc_buttons_init(&btns, 0u, table, table_size);

while (1)
{
    adc_buttons_update(&btns);

    switch (adc_buttons_get(&btns))
    {
        case 1u: /* button 1 */ break;
        case 2u: /* button 2 */ break;
        case 3u: /* button 3 */ break;
        case 4u: /* button 4 */ break;
        default:  /* no button */ break;
    }
}
```

## Особливості

- Один ADC input використовується для кількох кнопок через resistor ladder.
- Діапазони порогів налаштовує викликаючий код.
- Бібліотека підходить для простих клавіатур і панельних кнопок на PIC18F452.
- Click і hold flags компактні та статичні.
- Menu logic не входить до цієї бібліотеки; це задача Phase 2.

## Залежності

- `core/compiler.h`
- `core/types.h`
- `drivers/analog/adc`
- `drivers/timers/tick`