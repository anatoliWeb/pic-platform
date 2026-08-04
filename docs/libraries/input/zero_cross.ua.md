[English version](./zero_cross.md)

# Детектор переходу через нуль

## Опис

Багаторазовий детектор крайового переходу через нуль з придушенням глітчів, вимірюванням напівперіоду, визначенням частоти (50/60 Гц), таймаутом і відновленням. Власних пінів, таймера та виходів не має. Користувач подає рівні («фронти») із монотонним мікросекундним часом і розсилає створені події будь-якій кількості споживачів.

## API

- `zero_cross_init(zc, config)`
- `zero_cross_on_edge(zc, now_us, event)`
- `zero_cross_process(zc, now_us)`
- `zero_cross_is_alive(zc)`
- `zero_cross_get_status(zc)`
- `zero_cross_get_frequency(zc)`
- `zero_cross_get_half_cycle_us(zc)`
- `zero_cross_get_sequence(zc)`
- `zero_cross_reset(zc)`

## Приклад

```c
zero_cross_config_t cfg = {
    .timeout_ms = 300u,
    .min_half_cycle_us = 8000u,
    .max_half_cycle_us = 12000u,
    .glitch_reject_us = 500u,
    .recovery_event_count = 2u,
};
zero_cross_t zc;
zero_cross_init(&zc, &cfg);

/* В ISR переходу через нуль: */
zero_cross_event_t event;
if (zero_cross_on_edge(&zc, now_us(), &event))
{
    dispatch_to_consumers(&event);
}
```

## Примітки

- Бібліотека не володіє тактовим джерелом; користувач передає монотонні мікросекунди в `now_us`.
- Перший фронт лише «озброює» детектор; дійсний напівперіод вимагає двох фронтів.
- Дійсний фронт створює одну подію та переводить детектор у стан `ALIVE`.
- Таймаут оновлюється з головного циклу через `zero_cross_process()`.
- `ac_phase_control` використовує `zero_cross` як спільну синхронізуючу область.
- Блоки функцій (таймаут, визначення частоти, фільтр глітчів) можна вимкнути макросами `ZERO_CROSS_DISABLE_*`.

## Залежності

- `core/types.h`
- `core/compiler.h`

## Супутні приклади

- `libraries/input/zero_cross/example.c`
- `libraries/output/ac_phase_control/example.c`