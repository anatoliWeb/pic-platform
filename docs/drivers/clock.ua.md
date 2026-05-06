[🇬🇧 English version](./clock.md)

# Clock Helper

## Опис
Центральний helper частоти, який є джерелом істини для таймінгових драйверів.

## API
- `CLOCK_FREQUENCY`
- `clock_get_frequency()`

## Приклад
```c
freq = clock_get_frequency();
```

## Особливості
- Частота задається через `DRV_XTAL_FREQ` у device/core конфігурації.
- Впливає на delay, UART, I2C та інші таймінги.

## Залежності
- `core/compiler.h`
- `core/device.h`
