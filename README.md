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

### 1-Wire Architecture

- `drivers/onewire/onewire.c` — universal entrypoint + fallback
- `C18/drivers/onewire/onewire.c` — C18-specific implementation
- `XC8/drivers/onewire/onewire.c` — XC8-specific implementation

### Example

- `drivers/onewire/example.c` — reset + presence detect + write/read byte test + debug print
