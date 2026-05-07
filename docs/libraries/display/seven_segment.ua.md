[English version](./seven_segment.md)

# Бібліотека семисегментного індикатора

## Опис
Повторно використовувана бібліотека для однорозрядного семисегментного індикатора.
Підтримує індикатори із загальним катодом та загальним анодом з налаштовуваним мапуванням пінів.

## Публічний API
- `seven_segment_init()`
- `seven_segment_set_digit()`
- `seven_segment_set_hex()`
- `seven_segment_set_raw()`
- `seven_segment_clear()`
- `seven_segment_set_dot()`

## Підтримувані типи індикаторів
- Загальний катод
- Загальний анод

## Підтримка відображення
- цифри `0..9`
- hex `A..F`
- знак мінуса (raw pattern `0x40`)
- порожній стан (`clear`)

## Позначення сегментів
Порядок бітів у raw pattern:
- bit0: A
- bit1: B
- bit2: C
- bit3: D
- bit4: E
- bit5: F
- bit6: G

## Модель конфігурації
Використовуйте `seven_segment_config_t`:
- піни сегментів `seg_a..seg_g`
- необов’язковий пін `dot` + `has_dot`
- тип індикатора (`SEVEN_SEGMENT_COMMON_CATHODE` або `SEVEN_SEGMENT_COMMON_ANODE`)

## Приклад підключення
Типове підключення однорозрядного індикатора:
- MCU GPIO -> резистори сегментів -> A..G (і необов’язково DP)
- Загальний пін:
  - GND для загального катода
  - VCC для загального анода

## Приклад GPIO-мапування
```c
static const seven_segment_config_t cfg =
{
    { &PORTB, &TRISB, 0u },
    { &PORTB, &TRISB, 1u },
    { &PORTB, &TRISB, 2u },
    { &PORTB, &TRISB, 3u },
    { &PORTB, &TRISB, 4u },
    { &PORTB, &TRISB, 5u },
    { &PORTB, &TRISB, 6u },
    { &PORTB, &TRISB, 7u },
    1u,
    SEVEN_SEGMENT_COMMON_CATHODE
};
```

## Підготовка до мультиплексування
Поточна реалізація підтримує лише один розряд.
API та модель конфігурації підходять для майбутнього розширення з підтримкою багаторозрядного мультиплексування.

## Залежності
- `core/*`
- `drivers/gpio/*`
