# PIC PLATFORM - Universal Drivers (C18 + XC8)

Бібліотека універсальних драйверів для PIC (фокус: PIC18).

## Підтримувані компілятори

- MPLAB C18
- MPLAB XC8

## Архітектура

- Спільний API для кожного драйвера
- Universal entrypoint у `drivers/`
- Compiler-specific реалізації у `C18/` і `XC8/`
- Fallback реалізація, якщо override не вибрано

## 1-Wire Driver (Base)

1-Wire базовий драйвер реалізовано через GPIO bit-banging.

Що підтримується:

- `onewire_init(port, tris, pin)`
- `onewire_reset()`

Як працює reset/presence:

1. Master тягне лінію LOW ~480us
2. Відпускає лінію (input/high-Z)
3. Чекає ~70us
4. Читає presence (LOW => device present)
5. Чекає ~410us

Вимоги до timing:

- мікросекундні затримки через `core/delay.h`
- точність `_XTAL_FREQ / DRV_XTAL_FREQ` критична

Важливо:

- потрібен зовнішній pull-up резистор на 1-Wire лінії
- драйвер не використовує hardware peripheral

### 1-Wire Core Operations

Додано core операції біт/байт:

- `onewire_write_bit(uint8_t bit)`
- `onewire_read_bit(void)`
- `onewire_write_byte(uint8_t byte)`
- `onewire_read_byte(void)`

Таймінги слотів (приблизні):

- Write '1': LOW ~6us, release, wait ~64us
- Write '0': LOW ~60us, release, wait ~10us
- Read: LOW ~6us, release, wait ~9us, sample, wait ~55us

Порядок бітів:

- LSB-first для `onewire_write_byte()` та `onewire_read_byte()`

Стабільність таймінгів:

- На час кожного біт-слоту переривання тимчасово вимикаються через `DRV_INT_DISABLE()`
- Після завершення слоту переривання знову вмикаються через `DRV_INT_ENABLE()`

### 1-Wire ROM Commands

Додано ROM-рівень команд:

- `onewire_skip_rom()` — команда `0xCC` для broadcast доступу
- `onewire_match_rom(uint8_t* rom)` — команда `0x55` + 64-bit ROM адреса
- `onewire_read_rom(uint8_t* rom)` — команда `0x33` (працює для single-device шини)
- `onewire_search_rom(uint8_t (*roms)[8], uint8_t max_devices)` — API для multi-device пошуку

Multi-device зауваження:

- `search_rom` наразі реалізовано як базовий placeholder (повертає `0`)
- API вже готовий для подальшого розширення без зміни інтерфейсу

### 1-Wire Architecture

- `drivers/onewire/onewire.c` — universal entrypoint + fallback
- `C18/drivers/onewire/onewire.c` — C18-specific implementation
- `XC8/drivers/onewire/onewire.c` — XC8-specific implementation

### Example

- `drivers/onewire/example.c` — reset + presence detect + write/read byte + ROM read + search API usage

## DS18B20 Driver

Драйвер `DS18B20` реалізований поверх `drivers/onewire` без дублювання low-level 1-Wire логіки.

Що підтримується:

- `ds18b20_start_conversion(uint8_t* rom)`
- `ds18b20_read_scratchpad(uint8_t* rom, uint8_t* data)`
- `ds18b20_get_temperature_raw(uint8_t* rom)`
- `ds18b20_get_temperature_celsius_x10(uint8_t* rom)`

Команди DS18B20:

- `0x44` — Convert T
- `0xBE` — Read Scratchpad

Режими адресації:

- `rom == NULL` -> `Skip ROM` (single device)
- `rom != NULL` -> `Match ROM` (multi-device)

Scratchpad і CRC:

- читається 9 байт scratchpad
- `data[8]` перевіряється через CRC8 Dallas/Maxim
- при помилці CRC функції читання повертають `0` (error)

Температура без float:

- raw: `int16_t temp = (data[1] << 8) | data[0]`
- масштаб: `temp_c_x10 = (temp * 10) / 16`

Зауваження:

- конверсія очікується через `DRV_DELAY_MS(750)`
- для C18 `DRV_DELAY_MS` треба забезпечити проєктною реалізацією delay

Приклади:

- `drivers/ds18b20/example.c` — single + multi-device сценарії
- `C18/examples/ds18b20_example.c`
- `XC8/examples/ds18b20_example.c`

## Timing / Delay

Шар `core/delay` є базою для bit-banging драйверів (1-Wire, software SPI тощо).

Що надається:

- `delay_us(uint16_t us)`
- `delay_ms(uint16_t ms)`
- сумісні макроси `DRV_DELAY_US(...)` і `DRV_DELAY_MS(...)`

Реалізація:

- XC8: короткі кроки через `__delay_us(1)` і `__delay_ms(1)` у циклі
- C18: calibrated blocking loops + `NOP`, з `volatile` лічильниками для захисту від оптимізації

Частота:

- джерело істини: `DRV_XTAL_FREQ` з `core/device.h`

Важливо:

- delay є blocking
- hardware timers не використовуються в цьому шарі
- точність критична для 1-Wire слотів

Приклад:

- `examples/common/delay_example.c` (blink + short us pulse)

## 1-Wire Update Notes

- `onewire_reset()` now runs in a critical section (`DRV_INT_DISABLE` / `DRV_INT_ENABLE`) to protect timing-sensitive reset/presence slots.
- Bit slot timing is kept as:
  - write '1': low ~6us, release, wait ~64us
  - write '0': low ~60us, release, wait ~10us
  - read slot: low ~6us, release, wait ~9us, sample, wait ~55us
- Added `onewire_crc8(const uint8_t* data, uint8_t len)` (Dallas/Maxim CRC8, reversed polynomial `0x8C`).
- `Search ROM not implemented yet` (API placeholder returns `0`).

## DS18B20 Update Notes

- DS18B20 uses only 1-Wire API (no low-level pin duplication).
- Scratchpad read is 9 bytes with CRC check:
  - `onewire_crc8(data, 8) == data[8]`
- Temperature conversion:
  - raw = `(data[1] << 8) | data[0]`
  - celsius x10 = `(raw * 10) / 16`
- New status-based API to avoid `0C vs error` ambiguity:
  - `uint8_t ds18b20_read_temperature_raw(uint8_t* rom, int16_t* out_raw)`
  - `uint8_t ds18b20_read_temperature_celsius(uint8_t* rom, int16_t* out_temp_x10)`
- Backward-compatible wrappers remain:
  - `ds18b20_get_temperature_raw()`
  - `ds18b20_get_temperature_celsius_x10()`

Note about naming:
- In C, function overloading is not supported, so a single function name cannot have both one-argument and two-argument signatures.

## Scheduler

Легкий scheduler реалізовано поверх `drivers/tick` для non-blocking архітектури без RTOS.

Файли:

- `core/scheduler/scheduler.h`
- `core/scheduler/scheduler.c`
- `core/scheduler/example.c`

API:

- `scheduler_init()`
- `scheduler_add_task(task_t* task)`
- `scheduler_run()`

Модель роботи:

- використовує `tick_get()`
- для кожної задачі перевіряє `now - last_run >= interval`
- при спрацюванні викликає callback і оновлює `last_run`

Особливості:

- статичний реєстр задач, без `malloc`
- `MAX_TASKS = 10`
- мінімальний overhead

Правила використання:

- `scheduler_run()` викликати часто в `while(1)`
- callback-и мають бути короткими і неблокуючими
- довгі `delay` всередині задач небажані

Приклад задач:

- blink LED (500 ms)
- read DS18B20 (1000 ms)
- debug print (2000 ms)

### LCD HD44780

Драйвер `drivers/lcd_hd44780` реалізує 4-bit режим через GPIO bit-banging.

API:

- `lcd_init_pins(...)`
- `lcd_init()`
- `lcd_clear()`
- `lcd_set_cursor(row, col)`
- `lcd_write_char(c)`
- `lcd_write_string(str)`

4-bit wiring:

- RS та EN задаються окремими pin
- D4..D7 задаються через `lcd_init_pins(...)`
- дані передаються nibble-by-nibble: старший, потім молодший

Ініціалізація:

- `0x03`, `0x03`, `0x03`, `0x02` (перехід у 4-bit)
- далі: function set, display on, clear, entry mode

Таймінги:

- EN pulse ~2us
- command delay ~40us
- clear/home delay ~2ms

Приклад:

- `drivers/lcd_hd44780/example.c` ("Temp: 23.5C" / "System OK")
