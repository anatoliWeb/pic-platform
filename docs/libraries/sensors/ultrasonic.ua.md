# Бібліотека Ultrasonic (HC-SR04 стиль)

## Опис

`ultrasonic` — легка бібліотека вимірювання відстані для сенсорів типу HC-SR04.
Побудована як універсальний шар, який можна розширити для сумісних моделей (наприклад JSN-SR04T).

## Публічний API

- `ultrasonic_init()`
- `ultrasonic_trigger()`
- `ultrasonic_read()`
- `ultrasonic_get_distance_mm()`
- `ultrasonic_get_distance_cm()`
- `ultrasonic_is_valid()`

## Модель таймінгів

Типовий цикл вимірювання:

1. Подати trigger імпульс (`~10us` HIGH).
2. Дочекатися переходу echo в HIGH.
3. Виміряти тривалість HIGH імпульсу echo.
4. Перевести час у відстань.

Формула в бібліотеці:

`distance_mm ~= echo_us * 343 / 2000`

## Модель timeout

Усі очікування захищені timeout:

- timeout очікування ECHO HIGH
- timeout очікування ECHO LOW
- перевірка мін/макс тривалості імпульсу

Це гарантує, що бібліотека не зависає назавжди при відсутності echo.

## Конфігурація

- trigger port/tris/pin
- echo port/tris/pin
- `timeout_us`
- `min_pulse_us`
- `max_pulse_us`

Без хардкоду пінів.

## Обробка помилок

`ultrasonic_read()` повертає статус:

- `ULTRASONIC_STATUS_OK`
- `ULTRASONIC_STATUS_TIMEOUT_WAIT_HIGH`
- `ULTRASONIC_STATUS_TIMEOUT_WAIT_LOW`
- `ULTRASONIC_STATUS_INVALID_PULSE`
- `ULTRASONIC_STATUS_OVERFLOW`
- `ULTRASONIC_STATUS_ERROR`

## Підключення

- TRIG -> MCU output
- ECHO -> MCU input
- спільна GND з MCU
- живлення згідно з модулем

Для довгих ліній і шумного середовища важливі якісне заземлення та фільтрація.

## Діапазон і обмеження

- Реальний діапазон залежить від моделі сенсора та умов.
- HC-SR04 зазвичай до ~4м в ідеальних умовах.
- М'які або нахилені об'єкти можуть давати нестабільний echo.

## Залежності

- `core/*`
- `drivers/gpio`
- `drivers/timers/*` (часто `tick` для періодичного опитування в application)
