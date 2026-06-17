[🇺🇦 Ukrainian version](./adc_buttons.ua.md)

# ADC Buttons Driver

## Description

ADC ladder button driver for a single analog input. It maps ADC ranges to logical button IDs and adds lightweight debounce and event flags for phase 1 input examples.

## API

- `adc_buttons_init()`
- `adc_buttons_update()`
- `adc_buttons_get()`
- `adc_buttons_is_clicked()`
- `adc_buttons_is_held()`

## Example

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

## Notes

- One ADC input is used for multiple buttons via a resistor ladder.
- Threshold ranges are configured by the caller.
- The library is suitable for simple keyboards and panel buttons on PIC18F452.
- Click and hold flags are lightweight and static.
- Menu logic is not part of this library; that remains a Phase 2 task.

## Dependencies

- `core/compiler.h`
- `core/types.h`
- `drivers/analog/adc`
- `drivers/timers/tick`