[🇬🇧 English version](./adc.md)

# ADC Драйвер

## Опис

Драйвер Analog-to-Digital Converter (ADC) для платформи PIC18.
Використовується для сенсорів, ADC-кнопок (resistor ladder) та загальних аналогових вимірювань.

## API

- `adc_init()`
- `adc_read()`
- `adc_read_avg()`
- `adc_read_multiple()`

## Приклад

```c
#include "drivers/analog/adc/adc.h"

void main(void)
{
    uint16_t pot;
    int16_t therm;

    adc_init();

    pot = adc_read(0u);              // потенціометр
    therm = adc_read_thermistor(1u); // helper для термістора
}
```

## Особливості

- Результат АЦП має роздільну здатність 10 біт.
- Операції читання блокуючі.
- Час конверсії залежить від частоти MCU і налаштувань ADC.

## Залежності

- `core/compiler.h`
- `core/types.h`
- `core/device.h`

