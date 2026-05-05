# PIC PLATFORM - Universal Drivers (C18 + XC8)

÷е база ун≥версальних драйвер≥в дл€ PIC (фокус: PIC18).
–епозитор≥й не Ї application-проЇктом.

## ÷≥л≥

- ”н≥версальн≥ перевикористовуван≥ драйвери
- ѕ≥дтримка MPLAB C18 та MPLAB XC8
- ћ≥н≥мальна залежн≥сть драйвер≥в в≥д комп≥л€тора
- ”с≥ compiler-specific в≥дм≥нност≥ винесен≥ в `core/compiler.h`

## —труктура

- `core/`
- `drivers/`
- `examples/`
- `docs/`
- `C18/`
- `XC8/`

## як створити новий драйвер

1. ѕрочитати `DRIVER_GUIDELINES.md`
2. —коп≥ювати `/drivers/_template`
3. ѕерейменувати п≥д назву драйвера
4. –еал≥зувати API та внутр≥шню лог≥ку драйвера
5. ƒодати/адаптувати `example.c`

## ѕриклади дл€ комп≥л€тор≥в

- `examples/common` - ун≥версальн≥ приклади
- `C18/examples` - приклади п≥д MPLAB C18
- `XC8/examples` - приклади п≥д MPLAB XC8

## Core Layer

`core/` Ч це базовий шар, в≥д €кого залежать ус≥ драйвери.
¬≥н надаЇ сп≥льн≥ типи, макроси, конф≥гурац≥ю та комп≥л€торну абстракц≥ю.

‘айли Core Layer:

- `core/compiler.h` - абстракц≥€ C18/XC8 ≥ базов≥ compiler wrappers
- `core/types.h` - сп≥льн≥ типи статус≥в та булевих значень
- `core/bit_utils.h` - ун≥версальн≥ б≥тов≥ макроси
- `core/delay.h` - Їдин≥ wrappers дл€ затримок
- `core/device.h` - базов≥ device/clock налаштуванн€
- `core/interrupts.h` - helper-макроси дл€ global interrupts
- `core/config.h` - compile-time feature flags

## GPIO Driver

### GPIO Driver Architecture

- `drivers/gpio/gpio.c` Ч universal entry point
- `C18/drivers/gpio/gpio.c` Ч C18-specific implementation
- `XC8/drivers/gpio/gpio.c` Ч XC8-specific implementation

## UART Driver

### UART Driver Architecture

- `drivers/uart/uart.c` Ч universal entry point
- `C18/drivers/uart/uart.c` Ч C18-specific implementation
- `XC8/drivers/uart/uart.c` Ч XC8-specific implementation

## UART Debug Module

UART Debug Ч це опц≥ональний модуль поверх UART дл€ коротких debug-пов≥домлень без `printf`.

”в≥мкненн€:

```c
#define DRV_DEBUG_ENABLE 1
#define DRV_USE_UART 1
```

Ѕазове використанн€:

```c
DBG_PRINT("Hello");
DBG_PRINTLN(" UART");
DBG_PRINT_INT(123);
DBG_PRINT_HEX(0xAB);
```

Zero-cost when disabled:

- €кщо `DRV_DEBUG_ENABLE=0` або `DRV_USE_UART=0`, debug макроси розгортаютьс€ в порожн≥ `do { } while (0)`
- код виклик≥в не генеруЇтьс€, runtime/heap overhead в≥дсутн≥й

јрх≥тектура:

- `drivers/uart_debug/uart_debug.h` Ч macro API + enable/disable logic
- `drivers/uart_debug/uart_debug.c` Ч universal entry/fallback
- `C18/drivers/uart_debug/uart_debug.c` Ч C18-specific implementation
- `XC8/drivers/uart_debug/uart_debug.c` Ч XC8-specific implementation

## RS485 Driver

RS485 Ч UART-based protocol layer.
¬икористовуЇ UART дл€ передач≥ байт≥в ≥ GPIO дл€ керуванн€ напр€мком DE/RE.

Frame format:

- `[START][LEN][DATA...][CRC]`
- `START = 0xAA`
- `LEN = payload length`
- `CRC = CRC8 (poly 0x07)`

ћожливост≥:

- direction control: `rs485_set_tx()` / `rs485_set_rx()`
- send/receive frames: `rs485_send_frame()` / `rs485_receive_frame()`
- CRC check: `rs485_crc8()`
- simple timeout on receive (delay-based)

Ѕазовий master/slave сценар≥й:

- master формуЇ payload ≥ викликаЇ `rs485_send_frame(...)`
- slave читаЇ через `rs485_receive_frame(...)`
- при CRC mismatch або timeout повертаЇтьс€ `0`

### RS485 Driver Architecture

- `drivers/rs485/rs485.c` Ч universal entry point + fallback
- `C18/drivers/rs485/rs485.c` Ч C18-specific implementation
- `XC8/drivers/rs485/rs485.c` Ч XC8-specific implementation

Fallback logic у `drivers/rs485/rs485.c`:

- дл€ `DRV_COMPILER_C18` п≥дключаЇтьс€ `C18` реал≥зац≥€
- дл€ `DRV_COMPILER_XC8` п≥дключаЇтьс€ `XC8` реал≥зац≥€
- ≥накше використовуЇтьс€ вбудований universal fallback

## ѕ≥дтримуван≥ комп≥л€тори

- MPLAB C18
- MPLAB XC8
