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

”в≥мкненн€:

```c
#define DRV_DEBUG_ENABLE 1
#define DRV_USE_UART 1
```

## RS485 Driver

RS485 Ч UART-based protocol layer.
Frame format: `[START][LEN][DATA...][CRC]`, де `START = 0xAA`, `CRC = CRC8`.

### RS485 Driver Architecture

- `drivers/rs485/rs485.c` Ч universal entry point + fallback
- `C18/drivers/rs485/rs485.c` Ч C18-specific implementation
- `XC8/drivers/rs485/rs485.c` Ч XC8-specific implementation

## ADC Driver

ADC driver призначений дл€ сенсор≥в, ADC-buttons та вим≥рюванн€ напруги.
ѕ≥дтримуЇ single read, multi read та averaging.

API:

- `void adc_init(void);`
- `uint16_t adc_read(uint8_t channel);`
- `uint16_t adc_read_avg(uint8_t channel, uint8_t samples);`
- `void adc_read_multiple(uint8_t* channels, uint16_t* results, uint8_t count);`
- `uint16_t adc_to_millivolts(uint16_t adc_value, uint16_t vref_mv);`
- `uint16_t adc_read_voltage(uint8_t channel);`
- `int16_t adc_read_thermistor(uint8_t channel);`
- `uint8_t adc_read_button(uint8_t channel);`

Ќотатки дл€ сенсор≥в:

- `adc_read_voltage()` повертаЇ м≥л≥вольти без float
- `adc_read_thermistor()` використовуЇ просте наближенн€ (розширюЇтьс€ п≥зн≥ше)
- `adc_read_button()` використовуЇ table-like threshold mapping дл€ ADC ladder

### ADC Driver Architecture

- `drivers/adc/adc.c` Ч universal entry point + fallback
- `C18/drivers/adc/adc.c` Ч C18-specific implementation
- `XC8/drivers/adc/adc.c` Ч XC8-specific implementation

## ѕ≥дтримуван≥ комп≥л€тори

- MPLAB C18
- MPLAB XC8
