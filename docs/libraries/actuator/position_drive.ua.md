# Бібліотека Позиційного Приводу

[English version](./position_drive.md)

## Опис

`position_drive` — неблокуюча бібліотека замкненого керування положенням для двигуна з
редуктором і датчиком положення. Вона керує напрямком обертання двигуна, поки виміряна позиція
не досягне заданого цільового кута. Бібліотека ніколи не працює з апаратурою напряму: увесь
доступ до заліза відбувається через callback'и, які надає застосунок, тож одна логіка керування
працює з будь-яким розведенням чи драйверним стеком.

## Коли використовувати

- Позиціонування важеля, клапана, підвісу камери чи антени, які приводить DC-мотор із редуктором.
- Коли датчиком положення є потенціометр, який читається через ADC (перша версія).
- Коли головний цикл застосунку неблокуючий і має продовжувати роботу, поки мотор рухається.
- Коли потрібні жорсткі межі, timeout руху, виявлення застрягання та гарантоване вимкнення
  мотора за будь-якої помилки.

## Підтримуване залізо

- МК: будь-який PIC18 платформи (еталон: PIC18F452).
- Мотор: DC-мотор із редуктором, який керується H-мостом (два піни напрямку, опційний пін
  enable/PWM).
- Датчик: потенціометр на ADC-каналі (бекенд `POSITION_DRIVE_SENSOR_ADC`).
- Час: мілісекундне джерело часу (платформний `drivers/timers/tick`).

## Архітектура

- Апаратну частину володіє застосунок, а бібліотеці передаються callback'и.
- `position_drive_process()` викликається з головного циклу і ніколи не блокує.
- Реалізований лише ADC-бекенд; encoder-бекенд є лише заглушкою.
- Debug-вивід callback'ний, тому бібліотека не залежить від UART або display-коду, коли debug вимкнений.

## Бекенди датчиків

Бекенд датчика обирається на етапі компіляції через `POSITION_DRIVE_SENSOR_TYPE`:

| Бекенд | Статус |
|--------|--------|
| `POSITION_DRIVE_SENSOR_ADC` | реалізовано |
| `POSITION_DRIVE_SENSOR_ENCODER` | заглушка, `init()` повертає `DRV_STATUS_UNSUPPORTED` |
| `POSITION_DRIVE_SENSOR_NONE` | заглушка, `init()` повертає `DRV_STATUS_UNSUPPORTED` |

Encoder-бекенд зарезервований на майбутнє. Він не реалізований: `init()` повертає
`DRV_STATUS_UNSUPPORTED`, а не мовчки робить не те.

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
- `position_drive_debug_cb_t` — опційний debug-вивід, який застосунок може спрямувати в UART, display або тестовий логер

Кожен callback отримує однаковий вказівник `context`, який застосунок зберіг у конфігурації.

## Структура Конфігурації

`position_drive_config_t`:

- діапазон raw датчика (`sensor_raw_min`, `sensor_raw_max`) зіставлений з діапазоном кутів
  (`angle_min_deg`, `angle_max_deg`)
- мертва зона навколо цілі (`target_tolerance_deg`)
- timeout руху (`move_timeout_ms`)
- виявлення застрягання (`stuck_timeout_ms`, `stuck_min_delta_raw`)
- інверсія полярності (`direction_inverted`)
- діапазон швидкості (`speed_min_percent`, `speed_max_percent`, `speed_default_percent`)

`init()` відхиляє невалідну конфігурацію (відсутні callback'и, порожній raw/angle діапазон,
нульовий допуск, нульовий timeout, порушений порядок швидкостей) і перед поверненням зупиняє мотор.

## Debug Configuration

Поточна реалізація використовує module-local compile-time switch:

| Опція | Дефолт | Примітки |
| --- | --- | --- |
| `POSITION_DRIVE_ENABLE_UART_DEBUG` | `0` | вмикає `debug_cb` і форматування debug |
| `POSITION_DRIVE_DEBUG_LEVEL_ERROR` | `1` | повідомлення про помилки |
| `POSITION_DRIVE_DEBUG_LEVEL_INFO` | `2` | повідомлення про зміни стану |
| `POSITION_DRIVE_DEBUG_LEVEL_TRACE` | `3` | стан + raw/angle/target/direction |
| `POSITION_DRIVE_DEBUG_LEVEL` | info when debug is enabled | можна зменшити для меншого коду |

Debug sinks на рівні застосунку:

- none: debug вимкнений
- UART: `debug_cb` пересилається в `libraries/system/uart_debug`
- display: `debug_cb` пересилається в LCD або seven-segment адаптер
- callback: `debug_cb` спрямовується в тестовий логер чи інший транспорт

## Модель станів

```mermaid
stateDiagram-v2
    [*] --> IDLE: init ok
    IDLE --> MOVING: move_to_deg
    MOVING --> TARGET_REACHED: у межах допуску
    MOVING --> ERROR: sensor / timeout / stuck / direction mismatch
    MOVING --> IDLE: stop / emergency_stop
    TARGET_REACHED --> MOVING: новий move_to_deg
    TARGET_REACHED --> IDLE: stop
    ERROR --> IDLE: clear_error
```

Переходи виконує `position_drive_process()`; дій від застосунку під час руху не потрібно.
Назви станів відповідають enum у `position_drive.h`: `POSITION_DRIVE_STATE_IDLE`, `MOVING`,
`TARGET_REACHED`, `ERROR`. Невдалий `init()` або невірна команда фіксує код помилки через
`position_drive_get_error()`, при цьому привод лишається неініціалізованим і публічний стан
залишається `IDLE`; стан `ERROR` встановлює `process()` при runtime-помилці (sensor / timeout /
stuck / direction mismatch).

## Модель Керування

`position_drive_move_to_deg()` запускає асинхронний рух. Застосунок повинен регулярно
викликати `position_drive_process()` у головному циклі. Бібліотека:

1. зчитує датчик і переводить raw-значення в градуси (лише цілочисельна математика)
2. застосовує bang-bang керування напрямком із корекцією перельоту
3. виявляє timeout і механічне застрягання
4. перевіряє, що датчик рухається у заданому напрямку

За будь-якої помилки мотор негайно зупиняється, а привод переходить у
`POSITION_DRIVE_STATE_ERROR`.

## Потік виконання

`position_drive_init()` перевіряє конфіг, зупиняє мотор, один раз зчитує датчик і переходить у
`IDLE`. Далі застосунок викликає `position_drive_move_to_deg()` і виконує `position_drive_process()`
у головному циклі, поки ціль не буде досягнута.

```mermaid
flowchart TD
    INIT[Ініціалізація приводу] --> VALIDATE[Перевірка конфігу]
    VALIDATE --> READ0[Зчитування початкового значення датчика]
    READ0 --> OK{Датчик валідний?}
    OK -- no --> INITERR[Зупинити мотор + помилка ініціалізації]
    OK -- yes --> IDLE[IDLE]

    IDLE --> CMD{Команда отримана?}
    CMD -- move_to_deg --> TARGET[Зберегти цільовий кут]
    TARGET --> DIR[Вибрати напрямок]
    DIR --> START[Запустити мотор]
    START --> LOOP[process loop]

    LOOP --> READ[Зчитати датчик]
    READ --> CHECK[Перевірки безпеки]
    CHECK --> REACHED{Ціль досягнута?}
    REACHED -- yes --> STOP[Зупинити мотор]
    STOP --> DONE[TARGET_REACHED]
    REACHED -- no --> DRIVE[Продовжити рух]
    DRIVE --> LOOP
```

## Movement Algorithm

```mermaid
flowchart TD
    START[process] --> READ[Зчитати датчик положення]
    READ --> RANGE{Raw у діапазоні?}
    RANGE -- no --> ESTOP[Зупинити мотор + помилка датчика]
    RANGE -- yes --> ANGLE[Перевести raw у градуси]
    ANGLE --> TARGET{У межах допуску?}
    TARGET -- yes --> STOP[Зупинити мотор + ціль досягнута]
    TARGET -- no --> TIMEOUT{Timeout?}
    TIMEOUT -- yes --> ERR1[Зупинити + помилка timeout]
    TIMEOUT -- no --> STUCK{Застрягання?}
    STUCK -- yes --> ERR2[Зупинити + помилка застрягання]
    STUCK -- no --> DIR{Напрямок валідний?}
    DIR -- no --> ERR3[Зупинити + розбіжність напрямку]
    DIR -- yes --> DRIVE[Керувати вперед/назад]
```

Поточна реалізація — bang-bang керування з корекцією перельоту, не PID.

## Безпека та обробка помилок

Кожна виявлена несправність проходить один шлях: спочатку зупиняються виходи мотора, стан
переходить у `ERROR`, фіксується код помилки, і застосунок може його прочитати та очистити.
`position_drive_clear_error()` лише повертає стан у `IDLE`; вона ніколи не запускає мотор сама.

```mermaid
flowchart TD
    ERR[Виявлено помилку] --> STOP[Негайно зупинити виходи мотора]
    STOP --> STATE[Стан = ERROR]
    STATE --> CODE[Зафіксувати код помилки]
    CODE --> REPORT[Застосунок читає помилку]
    REPORT --> CLEAR{clear_error викликано?}
    CLEAR -- no --> HOLD[Залишитись у ERROR]
    CLEAR -- yes --> IDLE[Повернутись у IDLE без запуску мотора]
```

## Перерахунок raw у градуси

Цілочисельний, безпечний, без float:

```text
deg = angle_min_deg + ((raw - sensor_raw_min) * (angle_max_deg - angle_min_deg))
                        / (sensor_raw_max - sensor_raw_min)
```

`init()` відхиляє конфігурації, де `raw_span * angle_span` переповнює `int32_t`, тож
перетворення не може переповнитись на PIC18.

## Калібрування датчика

1. Вручну переведіть важіль у механічний мінімум і запишіть raw-значення ADC у
   `sensor_raw_min`.
2. Переведіть у механічний максимум і запишіть значення у `sensor_raw_max`.
3. Оберіть `angle_min_deg` / `angle_max_deg` відповідно до механічного ходу.
4. Встановіть `target_tolerance_deg` як найменшу мертву зону без коливань.
5. Якщо напрямок реакції протилежний розведенню, встановіть `direction_inverted = 1`.

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
| `POSITION_DRIVE_DEBUG_LEVEL` | info when debug is enabled |

Перевизначайте їх через прапорці збірки, а не через `project_config.h`, щоб трансляційний
модуль бібліотеки бачив те саме значення.

Опції розв'язуються так:

```mermaid
flowchart TD
    CFG[core/pic_platform_config.h + -D flags] --> SENSOR{POSITION_DRIVE_SENSOR_TYPE}
    SENSOR -- ADC --> ADC[ADC-бекенд потенціометра]
    SENSOR -- ENCODER --> ENC[Заглушка, init повертає DRV_STATUS_UNSUPPORTED]
    SENSOR -- NONE --> NOBACKEND[Немає бекенда, init повертає DRV_STATUS_UNSUPPORTED]

    CFG --> PWM{POSITION_DRIVE_ENABLE_PWM}
    PWM -- 0 --> P0[Немає PWM-коду]
    PWM -- 1 --> P1[Шлях швидкості/PWM, потрібен set_speed_cb]

    CFG --> DBG{POSITION_DRIVE_ENABLE_UART_DEBUG}
    DBG -- 0 --> D0[Немає debug-коду чи виводу]
    DBG -- 1 --> D1[Компілювати debug-звіт]
    D1 --> LEVEL{POSITION_DRIVE_DEBUG_LEVEL}
    LEVEL -- ERROR --> L1[Лише повідомлення про помилки]
    LEVEL -- INFO --> L2[Повідомлення про зміну стану]
    LEVEL -- TRACE --> L3[Деталі raw/angle/target/direction]
```

## Приклад Проєкту

`example.c` демонструє послідовність рухів (30° -> 120°) з потенціометром як датчиком і
H-мостом. Повний проєкт MPLAB X знаходиться у `examples-projects/xc8/actuator/position_drive_adc.X`.

## Інтеграція з C18

Вихідний код бібліотеки не залежить від компілятора і збирається з MPLAB C18 через include-стаб
`C18/libraries/actuator/position_drive/position_drive.c`. Щоб використати його у C18-проєкті,
додайте цей стаб у `Source Files`, надайте ті самі callback'и, що й у XC8-прикладі
(`read_raw`, `get_tick`, `motor`), і передавайте переривання таймера у `timer1_irq_handler()`.

Окремий C18-приклад (`examples-projects/c18/actuator/position_drive_adc/`) запланований,
але ще не згенерований. Не додавайте власноруч зроблений C18-приклад, який не був
перевірений збіркою.

## Підключення у Proteus

Схема та примітки для симуляції описані у
`examples-projects/proteus/actuator/position_drive_adc/README.md`.

```mermaid
flowchart LR
    POT[Потенціометр] -->|повзунок RA0/AN0| PIC[PIC18F452]
    PIC -->|RD0 IN1| HBRIDGE[Драйвер H-моста]
    PIC -->|RD1 IN2| HBRIDGE
    PIC -. RD2 EN/PWM опційно .-> HBRIDGE
    HBRIDGE --> MOTOR[DC-мотор із редуктором]
    PIC -->|RC6/TX ніжка 25| VT[Virtual Terminal RXD]
    GND[Спільний GND] --- PIC
    GND --- HBRIDGE
    GND --- VT
```

Ключові моменти:

- Кінці потенціометра до +5V і GND, повзунок до RA0/AN0.
- IN1/IN2 H-моста з RD0/RD1; EN/PWM опційно на RD2.
- Живлення драйвера мотора за напругою мотора, спільний GND із PIC.
- UART TX на RC6 -> Virtual Terminal RXD, 9600 8N1.
- MCLR підтягнутий через 10k; VDD/VSS за DIP-40.

## Граф залежностей

```mermaid
flowchart TD
    APP[Код застосунку] --> PD[position_drive]
    PD --> MOTOR[Callback виводу мотора]
    PD --> SENSOR[Callback датчика положення]
    PD --> TICK[Джерело часу / tick]
    PD -. опційно .-> PWM[Вивід PWM-швидкості]
    PD -. опційно .-> DBG[Debug-адаптер]

    SENSOR --> ADC[ADC-бекенд потенціометра]
    MOTOR --> HBRIDGE[H-міст / драйвер мотора]

    DBG -. UART sink .-> UART[UART debug / Virtual Terminal]
    DBG -. display sink .-> DISP[Display або LCD-адаптер]
    DBG -. callback sink .-> CB[Callback застосунку]
```

## Debug Routing

```mermaid
flowchart LR
    PD[position_drive] --> D{Debug увімкнено?}
    D -- no --> NONE[Немає коду/виводу]
    D -- yes --> L{Рівень debug}
    L --> ERR[Лише помилки]
    L --> INFO[Зміни стану]
    L --> TRACE[Деталі raw/angle]
    ERR --> S{Sink}
    INFO --> S
    TRACE --> S
    S --> UART[UART / Virtual Terminal]
    S --> DISPLAY[Display / LCD-адаптер - documented pattern]
    S --> CB[Callback застосунку - implemented]
```

- реалізовано: callback routing і compile-time enable/level control
- supported pattern: UART, display або test logger sink через application callback
- planned: спільна platform-wide debug abstraction
- not implemented yet: direct display sink усередині `position_drive`

## Безпека

- У `position_drive_init()` мотор примусово переводиться у STOP до будь-якої валідації.
- Невдала ініціалізація ніколи не лишає мотор увімкненим.
- Помилка зчитування датчика або raw поза діапазоном зупиняє мотор.
- `position_drive_emergency_stop()` негайно зупиняє мотор і PWM, зберігаючи стан помилки
  для аналізу.
- `position_drive_clear_error()` виходить зі стану помилки, але ніколи не запускає мотор.
- Timeout і виявлення застрягання зупиняють мотор і фіксують помилку.

## Конфлікти ресурсів

- `Timer1` належить `tick`; у проєкті приводу не можна призначати `Timer1` іншому бекенду.
- `RA0/AN0` аналоговий, поки активний; інші аналогові піни лишаються цифровими через `ADCON1`.
- PSP-режим `PORTD` потрібно вимкнути, перш ніж `RD0/RD1` стануть виходами H-моста.
- Лише один бекенд датчика на проєкт; ADC-бекенд володіє ADC-драйвером і каналом.

## Відомі обмеження

- Немає PID; керування bang-bang із мертвою зоною і корекцією перельоту.
- Без dynamic memory, без float-математики.
- Одна ціль за раз; без планування траєкторії.
- Encoder-бекенд у цій версії — заглушка.

## Майбутня підтримка encoder

Абстракція датчика (callback `read_raw`) та ідентифікатор `POSITION_DRIVE_SENSOR_ENCODER` уже є.
Коли в платформі з'явиться готовий encoder-драйвер, encoder-бекенд можна реалізувати за тим
самим callback'ом і моделлю станів без зміни публічного API.

## Залежності

- `core/*` (compiler, types, platform config)
- опційне джерело часу через `drivers/timers/tick`
- опційний ADC-датчик через `drivers/analog/adc`
- опційна PWM-швидкість через `drivers/timers/pwm`

## HEX Generation

```mermaid
flowchart TD
    SRC[С-код і заголовки] --> MPLAB[Проєкт MPLAB X]
    MPLAB --> BUILD[Збірка XC8]
    BUILD --> DIST[dist/default/production/*.production.hex]
    DIST --> ART[examples-projects/hex/xc8/actuator/*.production.hex]
    ART --> PROTEUS[Симуляція у Proteus]
```

Точні команди та відображення копіювання HEX дивіться у
[workflow генерації](../architecture/generation-workflow.ua.md).
