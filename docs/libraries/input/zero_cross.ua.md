[English version](./zero_cross.md)

# Детектор переходу через нуль

## Опис

Багаторазовий детектор переходу через нуль з придушенням глітчів, вимірюванням напівперіоду, визначенням частоти (50/60 Гц), таймаутом і відновленням. Власних пінів, таймера та виходів не має. Користувач подає фронти з монотонним мікросекундним часом і розсилає створені події будь-якій кількості споживачів.

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
- `armed` є окремим станом; `timestamp_us == 0` є валідним значенням, а не sentinel.
- Перший фронт лише «озброює» детектор; дійсний напівперіод вимагає двох фронтів.
- Після `LOST` перший фронт знову озброює детектор і не створює подію.
- Відновлення вимагає `recovery_event_count` валідних напівперіодів перед поверненням у `ALIVE`.
- `sequence` збільшується лише для подій, які реально розсилаються споживачам.
- Таймаут оновлюється з головного циклу через `zero_cross_process()`.
- Для різниць timestamp використовується wrap-safe subtraction.
- Блоки функцій (таймаут, визначення частоти, фільтр глітчів) можна вимкнути макросами `ZERO_CROSS_DISABLE_*`.

## Безпека

- Бібліотека не забезпечує гальванічну ізоляцію.
- MCU не можна підключати напряму до мережі 220/230 В.
- Детектор має бути побудований з правильною ізоляцією та вхідним формуванням сигналу.
- Фільтр глітчів не замінює коректну схему вимірювання мережі.

## Залежності

- `core/types.h`
- `core/compiler.h`

## Супутні приклади

- `libraries/input/zero_cross/example.c`
- `examples-projects/xc8/input/zero_cross.X`
- `libraries/output/ac_phase_control/example.c`
