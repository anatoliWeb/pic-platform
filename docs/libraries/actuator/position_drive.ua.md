[English version](./position_drive.md)

# Бібліотека Позиційного Приводу

## Опис

`position_drive` — неблокуюча бібліотека замкненого керування положенням для двигуна з
редуктором і датчиком положення. Вона керує напрямком обертання двигуна, поки виміряна позиція
не досягне заданого цільового кута.

Бібліотека ніколи не працює з апаратурою напряму. Увесь доступ до заліза відбувається через
callback'и, які надає застосунок, тож одна логіка керування працює з будь-яким розведенням чи
драйверним стеком.

## Бекенди датчиків

Бекенд датчика обирається на етапі компіляції через `POSITION_DRIVE_SENSOR_TYPE`:

| Бекенд | Статус |
|--------|--------|
| `POSITION_DRIVE_SENSOR_ADC` | реалізовано |
| `POSITION_DRIVE_SENSOR_ENCODER` | заглушка, `init()` повертає `DRV_STATUS_UNSUPPORTED` |
| `POSITION_DRIVE_SENSOR_NONE` | заглушка, `init()` повертає `DRV_STATUS_UNSUPPORTED` |

## Публічний API

- `position_drive_init()`
- `position_drive_process()`
- `position_drive_move_to_deg()`
- `position_drive_stop()`
- `position_drive_emergency_stop()`
- `position_drive_set_speed_percent()`
- `position_drive_get_current_deg()`
- `position_drive_get_target_deg()`
- `position_drive_get_current_raw()`
- `position_drive_get_state()`
- `position_drive_get_error()`
- `position_drive_clear_error()`

## Callback'и

- `position_drive_get_tick_fn_t` — мілісекундне джерело часу (напр. `tick_get`)
- `position_drive_read_raw_fn_t` — зчитування сирого значення датчика
- `position_drive_motor_cb_t` — напрямок двигуна (STOP/FORWARD/REVERSE)
- `position_drive_set_speed_cb_t` — опційна PWM-швидкість, потрібна лише коли PWM увімкнено
- `position_drive_debug_cb_t` — опційний debug-вивід, використовується лише з UART debug

Кожен callback отримує однаковий вказівник `context`, який застосунок зберіг у конфігурації.

## Конфігурація

`position_drive_config_t`:

- діапазон raw датчика (`sensor_raw_min`, `sensor_raw_max`) зіставлений з діапазоном кутів
  (`angle_min_deg`, `angle_max_deg`)
- мертва зона навколо цілі (`target_tolerance_deg`)
- timeout руху (`move_timeout_ms`)
- виявлення застрягання (`stuck_timeout_ms`, `stuck_min_delta_raw`)
- інверсія полярності (`direction_inverted`)
- діапазон швидкості (`speed_min_percent`, `speed_max_percent`, `speed_default_percent`)

## Модель Керування

`position_drive_move_to_deg()` запускає асинхронний рух. Застосунок повинен регулярно
викликати `position_drive_process()` у головному циклі. Бібліотека:

1. зчитує датчик і переводить raw-значення в градуси (лише цілочисельна математика)
2. застосовує bang-bang керування напрямком із корекцією перельоту
3. виявляє timeout і механічне застрягання
4. перевіряє, що датчик рухається у заданому напрямку

За будь-якої помилки мотор негайно зупиняється, а привод переходить у
`POSITION_DRIVE_STATE_ERROR`.

## Опції Компіляції

Дефолти в `core/pic_platform_config.h`, перевизначаються через `-D` прапорці компілятора:

| Опція | Дефолт |
|-------|--------|
| `POSITION_DRIVE_SENSOR_TYPE` | `POSITION_DRIVE_SENSOR_ADC` |
| `POSITION_DRIVE_ENABLE_PWM` | `0` |
| `POSITION_DRIVE_ENABLE_TIMEOUT` | `1` |
| `POSITION_DRIVE_ENABLE_STUCK_DETECTION` | `1` |
| `POSITION_DRIVE_ENABLE_DIRECTION_CHECK` | `1` |
| `POSITION_DRIVE_ENABLE_UART_DEBUG` | `0` |

Перевизначайте їх через прапорці збірки, а не через `project_config.h`, щоб трансляційний
модуль бібліотеки бачив те саме значення.

## Приклад

`example.c` демонструє послідовність рухів (30° -> 120°) з потенціометром як датчиком і
H-мостом. Повний проєкт MPLAB X знаходиться у `examples-projects/xc8/actuator/position_drive_adc.X`.

## Безпека

- У `position_drive_init()` мотор примусово переводиться у STOP до будь-якої валідації.
- Невдала ініціалізація ніколи не лишає мотор увімкненим.
- `position_drive_emergency_stop()` негайно зупиняє мотор і PWM, зберігаючи стан помилки
  для аналізу.
- `position_drive_clear_error()` виходить зі стану помилки, але ніколи не запускає мотор.

## Залежності

- `core/*` (compiler, types, platform config)
- опційне джерело часу через `drivers/timers/tick`
- опційний ADC-датчик через `drivers/analog/adc`
- опційна PWM-швидкість через `drivers/timers/pwm`

## Обмеження для Embedded

- Без dynamic memory
- Лише integer-математика
- Невеликий стан на кожен екземпляр
