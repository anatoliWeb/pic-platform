# AC Phase Control / Zero-Cross Output

## Опис

`ac_phase_control` — це low-voltage, Proteus-first багатоканальна група керування фазою для PIC18F452.
Вона відокремлена від `software_pwm`, тому що не генерує безперервний duty-cycle сигнал. Замість цього один спільний zero-cross запускає нову півхвилю, а кожен увімкнений канал формує один короткий gate pulse зі своєю фазовою затримкою.

Опційний per-channel relay bypass перебирає керування виходом на повній потужності: triac перестає комутувати, а реле через весь напівперіод живить навантаження.

## Архітектура

- Один `ac_phase_control_group_t` володіє спільним pointer на detector як синхронізуючою областю та масивом каналів.
- Виявлення переходу через нуль, придушення глітчів, валідація напівперіоду, таймаут і відновлення живуть у багаторазовій бібліотеці `libraries/input/zero_cross`.
- Legacy mode використовує внутрішній detector групи.
- Shared-detector mode використовує `ac_phase_control_bind_zero_cross()` і зовнішній `zero_cross_t` як source of truth.
- Програма подає фронти в bound detector та розсилає створену подію в групу (і, за потреби, іншим споживачам).
- `ac_phase_control_on_zero_cross_event()` розпочинає нову півхвилю зі спільної події лише коли bound detector живий.
- Підтримується до `AC_PHASE_CONTROL_MAX_CHANNELS` каналів.
- Кожен канал має власний gate pin, enabled state, power percent і delay.
- Вибір таймера передається через `ac_phase_control_init_group()`.
- `ac_phase_control_attach_channel()` очікує gate **bit mask**, наприклад `(1U << 0U)` для `RD0`.
- `ac_phase_control_attach_channel_relay()` очікує relay **bit mask**, наприклад `(1U << 4U)` для `RD4`.
- Вибір timer-driver і reload logic залишаються всередині бібліотеки.
- Програма викликає лише public `ac_phase_control_irq_handler()` зі свого ISR.
- Dynamic allocation не використовується.
- Використовується лише integer-only timing.
- Група активується лише після zero-cross і стає неактивною після завершення півхвилі.
- Переходи relay використовують non-blocking state machine, який рухається через `ac_phase_control_process()`, тому blocking delay не потрібен.

## Relay Mode

- Канал може працювати лише в phase mode або в phase mode з приєднаним relay bypass.
- Реле вмикається, коли `power_percent >= relay_on_threshold_percent` (за замовчування `98%`).
- Реле вимикається, коли `power_percent <= relay_off_threshold_percent` (за замовчування `96%`).
- Різниця між thresholds дає hysteresis і запобігає relay chatter у зоні переходу.
- `relay_break_before_make_ms` (за замовчування `50 ms`) змушує gate мовчати перед увімкненням реле і тримає реле вимкненим після зняття запиту.
- `relay_min_on_ms` (за замовчування `200 ms`) і `relay_min_off_ms` (за замовчування `200 ms`) накладають мінімальний час утримання.
- Поки relay вмикається, тримається або вимикається, gate pulses каналу блокуються, тож triac і реле ніколи не живлять одне навантаження одночасно.
- На `100%` gate мовчить, а реле комутилює всю півхвилю.
- `ac_phase_control_is_channel_in_relay_mode()` повідомне, чи канал зараз тримугує своє реле увімкненому стані.

## Zero-Cross Timeout

- Група ініціалізує внутрішній detector за замовчуванням і може бути переприв'язана до зовнішнього shared detector через `ac_phase_control_bind_zero_cross()`.
- Bound detector є source of truth у shared-detector mode.
- Легасі-обгортка `ac_phase_control_on_zero_cross()` подає фронт через поточний bound detector.
- `ac_phase_control_process()` викликає `zero_cross_process()` на bound detector; коли він переходить у `ZERO_CROSS_STATUS_LOST`, група переходить у `AC_PHASE_STATUS_ZERO_CROSS_LOST`, очищає всі gate pulses, вимикає всі реле і встановлює стан повного off.
- Група автоматично відновлюється після `AC_PHASE_CONTROL_ZERO_CROSS_RECOVERY_EVENTS` (за замовчуванням `2`) свіжих zero-cross подій.
- `ac_phase_control_get_status()` повертає статус групи, а `ac_phase_control_is_zero_cross_alive()` повідомляє, чи живий bound zero-cross stream.
- Жодна подія не запускає імпульс, поки bound detector не повідомить `zero_cross_is_alive()`.

## Public API

- `ac_phase_control_init_group()`
- `ac_phase_control_bind_zero_cross()`
- `ac_phase_control_attach_channel()`
- `ac_phase_control_attach_channel_relay()`
- `ac_phase_control_detach_channel()`
- `ac_phase_control_set_power_percent()`
- `ac_phase_control_set_delay_us()`
- `ac_phase_control_enable_channel()`
- `ac_phase_control_get_power_percent()`
- `ac_phase_control_is_channel_enabled()`
- `ac_phase_control_is_channel_in_relay_mode()`
- `ac_phase_control_on_zero_cross()`
- `ac_phase_control_on_zero_cross_event()`
- `ac_phase_control_update_us()`
- `ac_phase_control_process()`
- `ac_phase_control_all_off()`
- `ac_phase_control_stop_group()`
- `ac_phase_control_irq_handler()`
- `ac_phase_control_get_tick_ms()`
- `ac_phase_control_is_any_channel_active()`
- `ac_phase_control_get_status()`
- `ac_phase_control_is_zero_cross_alive()`

## Вибір таймера

- Enum таймера бібліотеки:
- `AC_PHASE_CONTROL_TIMER2`
- Alias за замовчування:
  - `AC_PHASE_CONTROL_SELECTED_TIMER`
- Значення за замовчуванням:
  - `AC_PHASE_CONTROL_TIMER2`
- На цьому етапі реалізовано лише перевірений `Timer2` шлях.
- Доступ до timer driver у коді прикладу не потрібен.
- У прикладі `Timer2` лишається перевіреним значенням за замовчуванням і використовується лише я існуючий timer API.

## Модель таймінгу

- Типова ціль симуляції AC — `50 Hz`.
- Один повний цикл — `20 ms`.
- Одна півхвиля — `10 ms`.
- `ac_phase_control_on_zero_cross_event()` починає нову півхвилю зі спільної zero-cross події.
- `ac_phase_control_update_us()` рухає elapsed time без блокування.
- Кожен увімкнений канал може сформувати не більше одного короткого gate pulse за півхвилю.
- Великі значення elapsed обробляються через ширші тимчасові типи, щоб уникнути overflow.
- Gate output повертається в LOW після завершення pulse або після завершення півхвиль.
- Zero-cross pulse source на `100 Hz` відповідає цілі `50 Hz`: один pulse за півхвилю.

## Приклади

- `examples-projects/xc8/output/ac_phase_control.X`: чотири phase-only канали з slow fade на каналі `0`.
- `examples-projects/xc8/output/multi_channel_relay.X`: чотири канали, де канали `0..2` також мають relay bypass; канал `0` проходить `50% -> 97% -> 98% -> 100% -> 50%`, щоб протестувати перехід relay, а `RB1` дає emergency all-off.

## Статус валідації

- `Timer2`: реалізований і використовується як значення за замовчуванням.
- `Timer0/1/3` зарезервовані для майбутньої роботи і не входять до цього етапу.
- Proteus-валідація multi-channel: pending.
- Proteus-валідація relay-bypass: pending.
- Proteus-валідація slow fade: pending.

## Примітки для Proteus

- Використовуйте один fake zero-cross pulse source на `RB0 / INT0`.
- MCU не забезпечує ізоляцію.
- Detector hardware має бути гальванічно ізольованим від мережі.
- Glitch filter не є межею безпеки.
- Перевіряйте `RD0..RD3` через oscilloscope або logic analyzer.
- Додаткові diagnostic pin:
  - `RC0`: toggle для zero-cross
  - `RC1`: toggle для timer callback
  - `RC2`: індикатор gate-active
- Усі канали мають синхронізуватись по одному zero-cross edge.
- Менша power означає пізніший gate pulse.
- `0%` не формує pulse.
- `100%` формує pulse близько до `min_delay_us`.
- Жоден вихід не повинен залишатись HIGH.
- Реальні mains та `220V AC` виходять за межі цього модуля.

## Статус

Готово до перевірки в Proteus.
