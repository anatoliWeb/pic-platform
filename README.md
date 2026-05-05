# PIC PLATFORM - Universal Drivers (C18 + XC8)

Ѕ≥бл≥отека ун≥версальних драйвер≥в дл€ PIC (фокус: PIC18).

## ѕ≥дтримуван≥ комп≥л€тори

- MPLAB C18
- MPLAB XC8

## јрх≥тектура

- —п≥льний API дл€ кожного драйвера
- Universal entrypoint у `drivers/`
- Compiler-specific реал≥зац≥њ у `C18/` ≥ `XC8/`
- Fallback реал≥зац≥€, €кщо override не вибрано

## 1-Wire Driver (Base)

1-Wire базовий драйвер реал≥зовано через GPIO bit-banging.

ўо п≥дтримуЇтьс€:

- `onewire_init(port, tris, pin)`
- `onewire_reset()`

як працюЇ reset/presence:

1. Master т€гне л≥н≥ю LOW ~480us
2. ¬≥дпускаЇ л≥н≥ю (input/high-Z)
3. „екаЇ ~70us
4. „итаЇ presence (LOW => device present)
5. „екаЇ ~410us

¬имоги до timing:

- м≥кросекундн≥ затримки через `core/delay.h`
- точн≥сть `_XTAL_FREQ / DRV_XTAL_FREQ` критична

¬ажливо:

- потр≥бен зовн≥шн≥й pull-up резистор на 1-Wire л≥н≥њ
- драйвер не використовуЇ hardware peripheral

### 1-Wire Architecture

- `drivers/onewire/onewire.c` Ч universal entrypoint + fallback
- `C18/drivers/onewire/onewire.c` Ч C18-specific implementation
- `XC8/drivers/onewire/onewire.c` Ч XC8-specific implementation

### Example

- `drivers/onewire/example.c` Ч reset + presence detect + debug print
